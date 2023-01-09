#include "ReplaySystem/ReplayLoader.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayReader.hpp"
#include "Data/Score.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficultySerializedMethods.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/ColorSchemesSettings.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "Utils/BeatmapUtils.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/WebUtils.hpp"

#include "ReplaySystem/Playback/NotePlayer.hpp"
#include "Services/FileService.hpp"
#include "System/Action_2.hpp"

#include "logging.hpp"
#include "static.hpp"
#include <custom-types/shared/delegate.hpp>

using namespace GlobalNamespace;
namespace ScoreSaber::ReplaySystem::ReplayLoader
{

    GlobalNamespace::PlayerDataModel* playerDataModel;
    GlobalNamespace::MenuTransitionsHelper* menuTransitionsHelper;

    ScoreSaber::Data::Private::ReplayFile* LoadedReplay;
    GlobalNamespace::IDifficultyBeatmap* CurrentLevel;
    std::u16string CurrentPlayerName;
    std::string CurrentModifiers;
    ScoreSaber::ReplaySystem::Playback::NotePlayer* NotePlayerInstance;

    bool IsPlaying;

    void StartReplay(IDifficultyBeatmap* beatmap)
    {
        if (playerDataModel == nullptr)
        {
            playerDataModel = UnityEngine::Object::FindObjectOfType<PlayerDataModel*>();
        }

        if (menuTransitionsHelper == nullptr)
        {
            menuTransitionsHelper = UnityEngine::Object::FindObjectOfType<MenuTransitionsHelper*>();
        }

        auto playerData = playerDataModel->playerData;
        auto localPlayerSettings = playerData->playerSpecificSettings;

        auto playerSettings = PlayerSpecificSettings::New_ctor(LoadedReplay->metadata->LeftHanded, LoadedReplay->metadata->InitialHeight,
                                                               LoadedReplay->heightKeyframes.size() > 0, localPlayerSettings->sfxVolume,
                                                               localPlayerSettings->reduceDebris, localPlayerSettings->noTextsAndHuds, localPlayerSettings->noFailEffects,
                                                               localPlayerSettings->advancedHud, localPlayerSettings->autoRestart, localPlayerSettings->saberTrailIntensity,
                                                               localPlayerSettings->noteJumpDurationTypeSettings, localPlayerSettings->noteJumpFixedDuration,
                                                               localPlayerSettings->noteJumpStartBeatOffset, localPlayerSettings->hideNoteSpawnEffect, localPlayerSettings->adaptiveSfx,
                                                               localPlayerSettings->environmentEffectsFilterDefaultPreset, localPlayerSettings->environmentEffectsFilterExpertPlusPreset);

        std::function<void(StandardLevelScenesTransitionSetupDataSO*, LevelCompletionResults*)> ReplayEndCallback = [&](StandardLevelScenesTransitionSetupDataSO* standardLevelSceneSetupData, LevelCompletionResults* levelCompletionResults) {
            IsPlaying = false;
        };

        auto replayEndDelegate = custom_types::MakeDelegate<System::Action_2<StandardLevelScenesTransitionSetupDataSO*, LevelCompletionResults*>*>(
            classof(System::Action_2<StandardLevelScenesTransitionSetupDataSO*, LevelCompletionResults*>*),
            ReplayEndCallback);

        auto previewBeatmapLevel = reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(beatmap->get_level());
        menuTransitionsHelper->StartStandardLevel("Replay", beatmap, previewBeatmapLevel,
                                                  playerData->overrideEnvironmentSettings,
                                                  playerData->colorSchemesSettings->GetSelectedColorScheme(), BeatmapUtils::GetModifiersFromStrings(LoadedReplay->metadata->Modifiers),
                                                  playerSettings, nullptr, "Exit Replay", false, false, nullptr, replayEndDelegate, nullptr);
        IsPlaying = true;
    }

    void GetReplayData(GlobalNamespace::IDifficultyBeatmap* beatmap, int leaderboardId, std::string replayFileName, ScoreSaber::Data::Score& score, const std::function<void(bool)>& finished)
    {
        std::string tmpFilePath = ScoreSaber::Static::REPLAY_TMP_DIR + "/" + replayFileName + ".tmp";
        std::string localPath = ScoreSaber::Static::REPLAY_DIR + "/" + replayFileName + ".dat";

        CurrentLevel = beatmap;
        CurrentPlayerName = score.leaderboardPlayerInfo.name.value_or(u"unknown");
        CurrentModifiers = score.modifiers;

        std::thread t([tmpFilePath, localPath, replayFileName, leaderboardId, score, finished] {
            if (fileexists(localPath))
            {
                std::ifstream replayFile(localPath, ios::binary);
                std::vector<char> replayData((std::istreambuf_iterator<char>(replayFile)), std::istreambuf_iterator<char>());
                LoadedReplay = ScoreSaber::Data::Private::ReplayReader::Read(replayData);
                if (LoadedReplay != nullptr)
                {
                    finished(true);
                }
                else
                {
                    finished(false);
                }
            }
            else
            {
                std::string url = string_format("%s/api/game/telemetry/downloadReplay?playerId=%s&leaderboardId=%d", ScoreSaber::Static::BASE_URL.c_str(), score.leaderboardPlayerInfo.id.value().c_str(), leaderboardId);
                std::vector<char> replayData;
                long response = WebUtils::DownloadReplaySync(url, replayData, 64);
                if (response == 200)
                {
                    LoadedReplay = ScoreSaber::Data::Private::ReplayReader::Read(replayData);

                    unlink(tmpFilePath.c_str());
                    if (LoadedReplay != nullptr)
                    {
                        finished(true);
                    }
                    else
                    {
                        finished(false);
                    }
                }
                else
                {
                    finished(false);
                }
            }
        });
        t.detach();
    }

} // namespace ScoreSaber::ReplaySystem::ReplayLoader