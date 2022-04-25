#include "Services/UploadService.hpp"
#include "Data/InternalLeaderboard.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ScoreSaberUploadData.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/BeatmapDifficultySerializedMethods.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_ScoresScope.hpp"
#include "GlobalNamespace/PracticeViewController.hpp"
#include "ReplaySystem/Recorders/MainRecorder.hpp"
#include "Services/FileService.hpp"
#include "Services/LeaderboardService.hpp"
#include "Services/PlayerService.hpp"
#include "UI/Other/ScoreSaberLeaderboardView.hpp"
#include "UnityEngine/Resources.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/WebUtils.hpp"
#include "Utils/md5.h"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"
#include "static.hpp"
#include <thread>

using namespace StringUtils;

using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;
using namespace ScoreSaber::Static;
using namespace ScoreSaber::ReplaySystem;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::Services::UploadService
{
    bool uploading;

    void Five(GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* standardLevelScenesTransitionSetupData,
              GlobalNamespace::LevelCompletionResults* levelCompletionResults)
    {
        PracticeViewController* practiceViewController = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<PracticeViewController*>());
        if (!practiceViewController->get_isInViewControllerHierarchy())
        {
            if (Il2cppStrToStr(standardLevelScenesTransitionSetupData->gameMode) == "Solo")
            {
                if (standardLevelScenesTransitionSetupData->practiceSettings != nullptr)
                {
                    // We are in practice mode
                    return;
                }

                if (levelCompletionResults->levelEndAction != LevelCompletionResults::LevelEndAction::None)
                {
                    // Write replay
                    return;
                }

                if (levelCompletionResults->levelEndAction != LevelCompletionResults::LevelEndStateType::Cleared)
                {
                    if (levelCompletionResults->gameplayModifiers->noFailOn0Energy)
                    {
                        Six(standardLevelScenesTransitionSetupData->difficultyBeatmap, levelCompletionResults);
                        return;
                    }
                    else
                    {
                        // Player failed level, write replay don't continue to upload phase
                        return;
                    }
                }

                // Continue to upload phase
                Six(standardLevelScenesTransitionSetupData->difficultyBeatmap, levelCompletionResults);
            }
        }
        else
        {
            // We are in practice mode, still write the replay
        }
    }

    void Six(GlobalNamespace::IDifficultyBeatmap* beatmap, GlobalNamespace::LevelCompletionResults* levelCompletionResults)
    {

        std::string encryptedPacket = CreateScorePacket(beatmap, levelCompletionResults->rawScore, levelCompletionResults->modifiedScore,
                                                        levelCompletionResults->fullCombo, levelCompletionResults->badCutsCount, levelCompletionResults->missedCount,
                                                        levelCompletionResults->maxCombo, levelCompletionResults->energy, levelCompletionResults->gameplayModifiers);

        auto previewBeatmapLevel = reinterpret_cast<IPreviewBeatmapLevel*>(beatmap->get_level());
        std::string levelHash = Il2cppStrToStr(previewBeatmapLevel->get_levelID()->Replace(StrToIl2cppStr("custom_level_"), Il2CppString::_get_Empty())->ToUpper());
        std::string characteristic = Il2cppStrToStr(beatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName);
        std::string songName = Il2cppStrToStr(previewBeatmapLevel->get_songName());
        std::string difficultyName = Il2cppStrToStr(BeatmapDifficultySerializedMethods::SerializedName(beatmap->get_difficulty()));

        std::string replayFileName = ScoreSaber::Services::FileService::GetReplayFileName(levelHash, difficultyName, characteristic,
                                                                                          ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.id, songName);
        Seven(beatmap, levelCompletionResults->modifiedScore, encryptedPacket, replayFileName);
    }

    void Seven(IDifficultyBeatmap* beatmap, int modifiedScore, std::string uploadPacket, std::string replayFileName)
    {
        INFO("Getting current leaderboard data for leaderboard");
        LeaderboardService::GetLeaderboardData(
            beatmap, PlatformLeaderboardsModel::ScoresScope::Global, 1, [=](Data::InternalLeaderboard internalLeaderboard) {
                bool ranked = true;

                if (internalLeaderboard.leaderboard.has_value())
                {
                    ranked = internalLeaderboard.leaderboard.value().leaderboardInfo.ranked;
                    if (internalLeaderboard.leaderboard.value().leaderboardInfo.playerScore.has_value())
                    {
                        if (modifiedScore < internalLeaderboard.leaderboard.value().leaderboardInfo.playerScore.value().modifiedScore)
                        {
                            ERROR("Didn't beat score not uploading");
                            return;
                        }
                    }
                }
                else
                {
                    ERROR("Failed to get leaderboards ranked status");
                }

                ReplayFile* replay = Recorders::MainRecorder::ExportCurrentReplay();

                std::thread t([replay, replayFileName, uploadPacket, ranked] {
                    std::string serializedReplayPath = ScoreSaber::Data::Private::ReplayWriter::Write(replay, replayFileName);

                    std::string url = BASE_URL + "/api/game/upload";
                    std::string postData = "data=" + uploadPacket;

                    int attempts = 0;
                    bool done = false;
                    bool failed = false;

                    while (!done)
                    {
                        uploading = true;
                        if (!ranked)
                        {
                            auto [responseCode, response] = WebUtils::PostSync(url, postData, 30000);
                            if (responseCode == 200)
                            {
                                INFO("Score uploaded successfully");
                                done = true;
                            }
                            if (responseCode == 403)
                            {
                                INFO("Player banned, score didn't upload");
                                done = true;
                                failed = true;
                            }
                        }
                        else
                        {
                            INFO("Uploading ranked score...");
                            auto [responseCode, response] = WebUtils::PostWithFileSync(url, serializedReplayPath, uploadPacket, 30000);
                            if (responseCode == 200)
                            {
                                INFO("Score uploaded successfully");
                                done = true;
                            }
                            if (responseCode == 403)
                            {
                                INFO("Player banned, score didn't upload");
                                done = true;
                                failed = true;
                            }
                        }

                        if (!done)
                        {
                            if (attempts < 4)
                            {
                                // Failed but retry
                                ERROR("Score failed to upload, retrying");
                                attempts++;
                                std::this_thread::sleep_for(2000ms);
                            }
                            else
                            {
                                done = true;
                                failed = true;
                            }
                        }

                    } // We out the loop now

                    if (done && !failed)
                    {
                        // Score uploaded successfully
                        // Save local replay
                        INFO("Score uploaded");
                        MoveReplay(serializedReplayPath, replayFileName);
                        ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, true);
                    }

                    if (failed)
                    {
                        ERROR("Failed to upload score");
                        ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, false);
                        // Failed to upload score, tell user
                    }

                    uploading = false;
                });
                t.detach();
            },
            false);
    }

    void MoveReplay(std::string replayPath, std::string replayFileName)
    {
        std::string newFilePath = ScoreSaber::Static::REPLAY_DIR + "/" + replayFileName + ".dat";
        if (std::rename(replayPath.c_str(), newFilePath.c_str()) != 0)
        {
            ERROR("Failed to save local replay");
        }
    }

    std::string CreateScorePacket(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, int rawScore,
                                  int modifiedScore, bool fullCombo, int badCutsCount, int missedCount, int maxCombo, float energy,
                                  GlobalNamespace::GameplayModifiers* gameplayModifiers)
    {
        auto previewBeatmapLevel = reinterpret_cast<IPreviewBeatmapLevel*>(difficultyBeatmap->get_level());
        std::string levelHash = Il2cppStrToStr(previewBeatmapLevel->get_levelID()->Replace(StrToIl2cppStr("custom_level_"), Il2CppString::_get_Empty())->ToUpper());
        std::string gameMode = string_format("Solo%s", Il2cppStrToStr(difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName).c_str());
        int difficulty = BeatmapDifficultyMethods::DefaultRating(difficultyBeatmap->get_difficulty());

        std::string songName = Il2cppStrToStr(previewBeatmapLevel->get_songName());
        std::string songSubName = Il2cppStrToStr(previewBeatmapLevel->get_songSubName());
        std::string songAuthorName = Il2cppStrToStr(previewBeatmapLevel->get_songAuthorName());
        std::string levelAuthorName = Il2cppStrToStr(previewBeatmapLevel->get_levelAuthorName());
        int bpm = previewBeatmapLevel->get_beatsPerMinute();

        std::u16string playerName = ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.name;
        std::string playerId = ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.id;

        auto modifiers = GetModifierList(gameplayModifiers, energy);
        int hmd = 32;

        ScoreSaberUploadData data(playerName, playerId, rawScore, levelHash, songName, songSubName, levelAuthorName, songAuthorName, bpm,
                                  difficulty, "infoHash", modifiers, gameMode, badCutsCount, missedCount, maxCombo, fullCombo, hmd);

        std::string uploadData = data.serialize();

        // UMBY: Obfuscate
        std::string key = md5("f0b4a81c9bd3ded1081b365f7628781f-" + ScoreSaber::Services::PlayerService::playerInfo.playerKey + "-" + playerId + "-f0b4a81c9bd3ded1081b365f7628781f");

        std::vector<unsigned char> keyBytes(key.begin(), key.end());
        std::vector<unsigned char> uploadDataBytes(uploadData.begin(), uploadData.end());
        std::vector<unsigned char> encrypted = Swap(uploadDataBytes, keyBytes);
        std::string result = ConvertToHex(encrypted);
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    std::vector<std::string> GetModifierList(GlobalNamespace::GameplayModifiers* gameplayModifiers, float energy)
    {
        std::vector<std::string> results;
        if (gameplayModifiers->energyType == GameplayModifiers::EnergyType::Battery)
        {
            results.push_back("BE");
        }
        if (gameplayModifiers->noFailOn0Energy && energy == 0)
        {
            results.push_back("NF");
        }
        if (gameplayModifiers->noFailOn0Energy && energy == -1)
        {
            results.push_back("NF");
        }
        if (gameplayModifiers->instaFail)
        {
            results.push_back("IF");
        }
        if (gameplayModifiers->failOnSaberClash)
        {
            results.push_back("SC");
        }
        if (gameplayModifiers->enabledObstacleType == GameplayModifiers::EnabledObstacleType::NoObstacles)
        {
            results.push_back("NO");
        }
        if (gameplayModifiers->noBombs)
        {
            results.push_back("NB");
        }
        if (gameplayModifiers->strictAngles)
        {
            results.push_back("SA");
        }
        if (gameplayModifiers->disappearingArrows)
        {
            results.push_back("DA");
        }
        if (gameplayModifiers->ghostNotes)
        {
            results.push_back("GN");
        }
        if (gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::Slower)
        {
            results.push_back("SS");
        }
        if (gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::Faster)
        {
            results.push_back("FS");
        }
        if (gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::SuperFast)
        {
            results.push_back("SF");
        }
        if (gameplayModifiers->smallCubes)
        {
            results.push_back("SC");
        }
        if (gameplayModifiers->strictAngles)
        {
            results.push_back("SA");
        }
        if (gameplayModifiers->proMode)
        {
            results.push_back("PM");
        }
        if (gameplayModifiers->noArrows)
        {
            results.push_back("NA");
        }
        return results;
    }

    std::vector<unsigned char> Swap(std::vector<unsigned char> panda1, std::vector<unsigned char> panda2)
    {
        int N1 = 11;
        int N2 = 13;
        int NS = 257;

        for (int i = 0; i <= panda2.size() - 1; i++)
        {
            NS += NS % (panda2[i] + 1);
        }

        std::vector<unsigned char> T(panda1.size());
        for (int i = 0; i <= panda1.size() - 1; i++)
        {
            NS = panda2[i % panda2.size()] + NS;
            N1 = (NS + 5) * (N1 & 255) + (N1 >> 8);
            N2 = (NS + 7) * (N2 & 255) + (N2 >> 8);
            NS = ((N1 << 8) + N2) & 255;

            T[i] = static_cast<unsigned char>(panda1[i] ^ static_cast<unsigned char>(NS));
        }

        return T;
    }

    std::string ConvertToHex(const std::vector<unsigned char>& v)
    {
        std::stringstream buffer;
        for (int i = 0; i < v.size(); i++)
        {
            buffer << std::hex << std::setfill('0');
            buffer << std::setw(2) << static_cast<unsigned>(v[i]);
        }
        return buffer.str();
    }
} // namespace ScoreSaber::Services::UploadService
