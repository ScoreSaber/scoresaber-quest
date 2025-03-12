#include "Services/UploadService.hpp"
#include "Data/InternalLeaderboard.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ScoreSaberUploadData.hpp"
#include "Data/Private/Settings.hpp"
#include <GlobalNamespace/BeatmapCharacteristicSO.hpp>
#include <GlobalNamespace/BeatmapDifficulty.hpp>
#include <GlobalNamespace/BeatmapDifficultyMethods.hpp>
#include <GlobalNamespace/BeatmapDifficultySerializedMethods.hpp>
#include <GlobalNamespace/MultiplayerLevelCompletionResults.hpp>
#include <GlobalNamespace/MultiplayerPlayerResultsData.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel.hpp>
#include <GlobalNamespace/PracticeViewController.hpp>
#include <GlobalNamespace/OVRPlugin.hpp>
#include "ReplaySystem/Recorders/MainRecorder.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "Services/FileService.hpp"
#include "Services/LeaderboardService.hpp"
#include "Services/PlayerService.hpp"
#include "Services/ReplayService.hpp"
#include <System/Action.hpp>
#include "UI/Other/ScoreSaberLeaderboardView.hpp"
#include <UnityEngine/Application.hpp>
#include <UnityEngine/Resources.hpp>
#include "Utils/BeatmapUtils.hpp"
#include "Utils/SafePtr.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/WebUtils.hpp"
#include "Utils/md5.h"
#include "logging.hpp"
#include <custom-types/shared/delegate.hpp>
#include <bsml/shared/Helpers/getters.hpp>
#include "static.hpp"
#include "Utils/MaxScoreCache.hpp"
#include <chrono>
#include <thread>

using namespace StringUtils;

using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;
using namespace ScoreSaber::Static;
using namespace ScoreSaber::ReplaySystem;
using namespace ScoreSaber::Data::Private;
using namespace BSML::Helpers;

namespace ScoreSaber::Services::UploadService
{
    bool uploading;

    
    void Three(StandardLevelScenesTransitionSetupDataSO* standardLevelScenesTransitionSetupDataSO, LevelCompletionResults* levelCompletionResults)
    {
        Five(standardLevelScenesTransitionSetupDataSO->gameMode, standardLevelScenesTransitionSetupDataSO->beatmapLevel, standardLevelScenesTransitionSetupDataSO->beatmapKey, levelCompletionResults, standardLevelScenesTransitionSetupDataSO->practiceSettings != nullptr);
    }
    
    void Four(MultiplayerLevelScenesTransitionSetupDataSO* multiplayerLevelScenesTransitionSetupDataSO, MultiplayerResultsData* multiplayerResultsData)
    {
        if(multiplayerLevelScenesTransitionSetupDataSO->beatmapLevel == nullptr)
            return;
        if(multiplayerResultsData->localPlayerResultData->multiplayerLevelCompletionResults->levelCompletionResults == nullptr)
            return;
        if(multiplayerResultsData->localPlayerResultData->multiplayerLevelCompletionResults->playerLevelEndReason == MultiplayerLevelCompletionResults::MultiplayerPlayerLevelEndReason::HostEndedLevel)
            return;
        if(multiplayerResultsData->localPlayerResultData->multiplayerLevelCompletionResults->levelCompletionResults->levelEndStateType != LevelCompletionResults::LevelEndStateType::Cleared)
            return;

        Five(multiplayerLevelScenesTransitionSetupDataSO->gameMode, multiplayerLevelScenesTransitionSetupDataSO->beatmapLevel, multiplayerLevelScenesTransitionSetupDataSO->beatmapKey, multiplayerResultsData->localPlayerResultData->multiplayerLevelCompletionResults->levelCompletionResults, false);
    }


    void Five(StringW gameMode, BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, LevelCompletionResults* levelCompletionResults, bool practicing)
    {

        if (StringUtils::GetEnv("disable_ss_upload") == "1")
        {
            return;
        }

        if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
        {
            return;
        }

        PracticeViewController* practiceViewController = UnityEngine::Resources::FindObjectsOfTypeAll<PracticeViewController*>()->First();
        if (practiceViewController->isInViewControllerHierarchy)
        {
            ReplayService::WriteSerializedReplay();
            return;
        }

        if (gameMode == "Solo" || gameMode == "Multiplayer")
        {
            INFO("Starting upload process for {:s}:{:s}", beatmapKey.levelId, beatmapLevel->songName);
            if (practicing) {
                ReplayService::WriteSerializedReplay();
                return;
            }
            if (levelCompletionResults->levelEndAction != LevelCompletionResults::LevelEndAction::None) {
                if (levelCompletionResults->levelEndAction == LevelCompletionResults::LevelEndAction::Restart) {
                    INFO("Level was restarted before it was finished, don't write replay");
                } else {
                    ReplayService::WriteSerializedReplay();
                }
                return;
            }
            if (levelCompletionResults->levelEndStateType != LevelCompletionResults::LevelEndStateType::Cleared) {
                ReplayService::WriteSerializedReplay();
                return;
            }

            ReplayService::WriteSerializedReplay();
            // Continue to upload phase
            Six(beatmapLevel, beatmapKey, levelCompletionResults);
        }
    }

    void Six(BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, LevelCompletionResults* levelCompletionResults)
    {

        std::string encryptedPacket = CreateScorePacket(beatmapLevel, beatmapKey, levelCompletionResults->multipliedScore, levelCompletionResults->modifiedScore,
                                                        levelCompletionResults->fullCombo, levelCompletionResults->badCutsCount, levelCompletionResults->missedCount,
                                                        levelCompletionResults->maxCombo, levelCompletionResults->energy, levelCompletionResults->gameplayModifiers);

        std::string levelHash = GetFormattedHash(beatmapKey.levelId);

        std::string characteristic = beatmapKey.beatmapCharacteristic->serializedName;
        std::string songName = beatmapLevel->songName;
        std::string difficultyName = BeatmapDifficultySerializedMethods::SerializedName(beatmapKey.difficulty);

        std::string replayFileName = ScoreSaber::Services::FileService::GetReplayFileName(levelHash, difficultyName, characteristic,
                                                                                          ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.id, songName);
        Seven(beatmapLevel, beatmapKey, levelCompletionResults->modifiedScore, levelCompletionResults->multipliedScore, encryptedPacket, replayFileName);
    }

    void Seven(BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, int modifiedScore, int multipliedScore, std::string uploadPacket, std::string replayFileName)
    {
        FixedSafePtr<BeatmapLevel> beatmapLevelSafe(beatmapLevel);

        il2cpp_utils::il2cpp_aware_thread([beatmapLevelSafe, beatmapKey, modifiedScore, multipliedScore, uploadPacket, replayFileName] {
            ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(true, false);

            auto _maxScoreCache = BSML::Helpers::GetDiContainer()->Resolve<ScoreSaber::Utils::MaxScoreCache*>();
            _maxScoreCache->GetMaxScore(beatmapLevelSafe.ptr(), beatmapKey, [beatmapLevelSafe, beatmapKey, modifiedScore, multipliedScore, uploadPacket, replayFileName](int maxScore) {
                LeaderboardService::GetLeaderboardData(maxScore,
                    beatmapLevelSafe.ptr(), beatmapKey, PlatformLeaderboardsModel::ScoresScope::Global, 1, [=](Data::InternalLeaderboard internalLeaderboard) {
                    bool ranked = true;
                    if (internalLeaderboard.leaderboard.has_value())
                    {
                        ranked = internalLeaderboard.leaderboard.value().leaderboardInfo.ranked;
                        if (internalLeaderboard.leaderboard.value().leaderboardInfo.playerScore.has_value())
                        {
                            if (modifiedScore < internalLeaderboard.leaderboard.value().leaderboardInfo.playerScore.value().modifiedScore)
                            {
                                ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, false, "<color=#fc8181>Didn't beat score, not uploading</color>");
                                uploading = false;
                                return;
                            }
                        }
                    }
                    else
                    {
                        INFO("Failed to get leaderboards ranked status");
                    }

                    bool done = false;
                    bool failed = false;
                    int attempts = 0;

                    if(multipliedScore > maxScore) {
                        ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, false, "<color=#fc8181>Failed to upload (score was impossible)</color>");
                        INFO("Score was better than possible, not uploading!");
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
                    std::string url = BASE_URL + "/api/game/upload";
                    std::string postData = "data=" + uploadPacket;

                    while (!done)
                    {
                        uploading = true;
                        INFO("Uploading score...");
                        auto [responseCode, response] = WebUtils::PostWithReplaySync(url, ReplayService::CurrentSerializedReplay, uploadPacket, 30000);
                        INFO("Server response:\nHTTP code {:d}\nContent: {:s}", responseCode, response.c_str());
                        if (responseCode == 200)
                        {
                            INFO("Score uploaded successfully");
                            done = true;
                        }
                        else if (responseCode == 403)
                        {
                            INFO("Player banned, score didn't upload");
                            done = true;
                            failed = true;
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
                        SaveReplay(ReplayService::CurrentSerializedReplay, replayFileName);
                        ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, true);
                    }

                    if (failed)
                    {
                        ERROR("Failed to upload score");
                        ScoreSaber::UI::Other::ScoreSaberLeaderboardView::SetUploadState(false, false);
                        // Failed to upload score, tell user
                    }

                    uploading = false;
                },
                false);
            });
        }).detach();
    }

    void SaveReplay(const std::vector<char>& replay, std::string replayFileName)
    {
        if (!Settings::saveLocalReplays)
            return;
            
        std::string filePath = ScoreSaber::Static::REPLAY_DIR + "/" + replayFileName + ".dat";
        std::ofstream file(filePath, ios::binary);
        file.write(replay.data(), replay.size());
    }

    
    StringW friendlyLevelAuthorName(ArrayW<StringW> mappers, ArrayW<StringW> lighters) {
        vector<StringW> mappersAndLighters;
        for(auto mapper : mappers) {
            mappersAndLighters.push_back(mapper);
        }
        for(auto mapper : lighters) {
            mappersAndLighters.push_back(mapper);
        }

        if(mappersAndLighters.size() == 0) {
            return "";
        }
        if(mappersAndLighters.size() == 1) {
            return mappersAndLighters.front();
        }
        StringW result;
        for(int i = 0; i < mappersAndLighters.size() - 1; i++) {
            result += mappersAndLighters[i];
            if(i < mappersAndLighters.size() - 2) {
                result += ", ";
            }
        }
        result += " & " + mappersAndLighters.back();
        return result;
    }

    std::string CreateScorePacket(BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, int rawScore,
                                  int modifiedScore, bool fullCombo, int badCutsCount, int missedCount, int maxCombo, float energy,
                                  GameplayModifiers* gameplayModifiers)
    {
        std::string levelHash = GetFormattedHash(beatmapKey.levelId);

        std::string gameMode = "Solo" + beatmapKey.beatmapCharacteristic->serializedName;
        int difficulty = BeatmapDifficultyMethods::DefaultRating(beatmapKey.difficulty);

        std::string songName = beatmapLevel->songName;
        std::string songSubName = beatmapLevel->songSubName;
        std::string songAuthorName = beatmapLevel->songAuthorName;
        std::string levelAuthorName = friendlyLevelAuthorName(beatmapLevel->allMappers, beatmapLevel->allLighters);
        int bpm = beatmapLevel->beatsPerMinute;

        std::u16string playerName = ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.name;
        std::string playerId = ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.id;

        auto modifiers = GetModifierList(gameplayModifiers, energy);

        // TODO go back to these versions after the unity upgrade (if it works then)
        // std::string deviceHmd = fmt::format("standalone_hmd:(ovrplugin):{:s}({:d})", std::string(OVRPlugin::GetSystemHeadsetType().i_Enum()->ToString()).c_str(), (int)OVRPlugin::GetSystemHeadsetType());
        // std::string deviceController = fmt::format("standalone_controller:(ovrplugin):{:s}({:d})", std::string(OVRPlugin::GetActiveController().i_Enum()->ToString()).c_str(), (int)OVRPlugin::GetActiveController());

        std::string deviceHmd = fmt::format("standalone_hmd:(ovrplugin):{:s}({:d})", stringify_OVRPlugin_SystemHeadset(OVRPlugin::GetSystemHeadsetType()), (int)OVRPlugin::GetSystemHeadsetType());
        std::string deviceController = fmt::format("standalone_controller:(ovrplugin):{:s}({:d})", stringify_OVRPlugin_Controller(OVRPlugin::GetActiveController()), (int)OVRPlugin::GetActiveController());

        std::string infoHash = GetVersionHash();

        ScoreSaberUploadData data(playerName, playerId, rawScore, levelHash, songName, songSubName, levelAuthorName, songAuthorName, bpm,
                                  difficulty, infoHash, modifiers, gameMode, badCutsCount, missedCount, maxCombo, fullCombo, deviceHmd, deviceController, deviceController);

        std::string uploadData = data.serialize();

        std::string key = md5("f0b4a81c9bd3ded1081b365f7628781f-" + ScoreSaber::Services::PlayerService::playerInfo.playerKey + "-" + playerId + "-f0b4a81c9bd3ded1081b365f7628781f");

        std::vector<unsigned char> keyBytes(key.begin(), key.end());
        std::vector<unsigned char> uploadDataBytes(uploadData.begin(), uploadData.end());
        std::vector<unsigned char> encrypted = Swap(uploadDataBytes, keyBytes);
        std::string result = ConvertToHex(encrypted);
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    std::vector<std::string> GetModifierList(GameplayModifiers* gameplayModifiers, float energy)
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

    std::string GetVersionHash()
    {
        return md5(std::string("Quest") + VERSION + (std::string)UnityEngine::Application::get_version());
    }
} // namespace ScoreSaber::Services::UploadService
