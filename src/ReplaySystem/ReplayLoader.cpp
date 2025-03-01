#include "ReplaySystem/ReplayLoader.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayReader.hpp"
#include "Data/Score.hpp"
#include <GlobalNamespace/BeatmapCharacteristicSO.hpp>
#include <GlobalNamespace/BeatmapDifficultySerializedMethods.hpp>
#include <GlobalNamespace/ColorScheme.hpp>
#include <GlobalNamespace/ColorSchemesSettings.hpp>
#include <GlobalNamespace/EnvironmentsListModel.hpp>
#include <GlobalNamespace/MenuTransitionsHelper.hpp>
#include <GlobalNamespace/PlayerData.hpp>
#include <GlobalNamespace/PlayerDataModel.hpp>
#include <GlobalNamespace/PlayerSpecificSettings.hpp>
#include <GlobalNamespace/RecordingToolManager.hpp>
#include "Utils/BeatmapUtils.hpp"
#include "Utils/WebUtils.hpp"

#include "ReplaySystem/Playback/NotePlayer.hpp"
#include <System/Action.hpp>
#include <System/Action_2.hpp>
#include <bsml/shared/BSML/MainThreadScheduler.hpp>
#include <bsml/shared/Helpers/getters.hpp>
#include <custom-types/shared/delegate.hpp>
#include "logging.hpp"
#include "static.hpp"

using namespace GlobalNamespace;
using namespace BSML;
using namespace BSML::Helpers;

namespace ScoreSaber::ReplaySystem::ReplayLoader
{

    GlobalNamespace::PlayerDataModel* playerDataModel;
    GlobalNamespace::MenuTransitionsHelper* menuTransitionsHelper;

    std::shared_ptr<ScoreSaber::Data::Private::ReplayFile> LoadedReplay;
    GlobalNamespace::BeatmapKey CurrentBeatmapKey;
    GlobalNamespace::BeatmapLevel* CurrentBeatmapLevel;
    std::u16string CurrentPlayerName;
    std::string CurrentModifiers;
    ScoreSaber::ReplaySystem::Playback::NotePlayer* NotePlayerInstance;

    bool IsPlaying;
    
    void OnSoftRestart() {
        playerDataModel = nullptr;
        menuTransitionsHelper = nullptr;
        LoadedReplay = nullptr;
        CurrentBeatmapLevel = nullptr;
        CurrentBeatmapKey = GlobalNamespace::BeatmapKey();
        CurrentPlayerName.clear();
        CurrentModifiers.clear();
        NotePlayerInstance = nullptr;
    }

    void StartReplay(GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey)
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
                                                               localPlayerSettings->arcsHapticFeedback, localPlayerSettings->arcVisibility,
                                                               localPlayerSettings->environmentEffectsFilterDefaultPreset, localPlayerSettings->environmentEffectsFilterExpertPlusPreset,
                                                               localPlayerSettings->headsetHapticIntensity);

        std::function<void(UnityW<StandardLevelScenesTransitionSetupDataSO>, LevelCompletionResults*)> ReplayEndCallback = [&](UnityW<StandardLevelScenesTransitionSetupDataSO> standardLevelSceneSetupData, LevelCompletionResults* levelCompletionResults) {
            IsPlaying = false;
        };

        auto replayEndDelegate = custom_types::MakeDelegate<System::Action_2<UnityW<StandardLevelScenesTransitionSetupDataSO>, LevelCompletionResults*>*>(
            classof(System::Action_2<UnityW<StandardLevelScenesTransitionSetupDataSO>, LevelCompletionResults*>*),
            ReplayEndCallback);

        auto _environmentsListModel = Helpers::GetDiContainer()->Resolve<GlobalNamespace::EnvironmentsListModel*>();

        menuTransitionsHelper->StartStandardLevel("Replay", // gameMode
                                                  byref(beatmapKey), // beatmapKey
                                                  beatmapLevel, // beatmapLevel
                                                  playerData->overrideEnvironmentSettings, // overrideEnvironmentSettings
                                                  playerData->colorSchemesSettings->GetOverrideColorScheme(), // overrideColorScheme
                                                  beatmapLevel->GetColorScheme(beatmapKey.beatmapCharacteristic, beatmapKey.difficulty), // beatmapOverrideColorScheme
                                                  BeatmapUtils::GetModifiersFromStrings(LoadedReplay->metadata->Modifiers), // gameplayModifiers
                                                  playerSettings, // playerSpecificSettings
                                                  nullptr, // practiceSettings
                                                  _environmentsListModel, // environmentsListModel
                                                  "Exit Replay", // backButtonText
                                                  false, // useTestNoteCutSoundEffects
                                                  false, // startPaused
                                                  nullptr, // beforeSceneSwitchToGameplayCallback
                                                  nullptr, // afterSceneSwitchToGameplayCallback
                                                  replayEndDelegate, // levelFinishedCallback
                                                  nullptr, // levelRestartedCallback
                                                  {false, {}} // recordingToolData (set to null)
                                                  ); 
        IsPlaying = true;
    }

    void Load(const std::vector<char> &replayData, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, std::string modifiers, std::u16string playerName) {
        CurrentBeatmapLevel = beatmapLevel;
        CurrentBeatmapKey = beatmapKey;
        CurrentPlayerName = playerName;
        CurrentModifiers = modifiers;

        LoadedReplay = ScoreSaber::Data::Private::ReplayReader::Read(replayData);

        MainThreadScheduler::Schedule([=]() {
            StartReplay(beatmapLevel, beatmapKey);
        });
    }

    void GetReplayData(GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, int leaderboardId, std::string replayFileName, ScoreSaber::Data::Score& score, const std::function<void(bool)>& finished)
    {
        std::string localPath = ScoreSaber::Static::REPLAY_DIR + "/" + replayFileName + ".dat";

        CurrentBeatmapLevel = beatmapLevel;
        CurrentBeatmapKey = beatmapKey;
        CurrentPlayerName = score.leaderboardPlayerInfo.name.value_or(u"unknown");
        CurrentModifiers = score.modifiers;

        il2cpp_utils::il2cpp_aware_thread([localPath, replayFileName, leaderboardId, score, finished]() {
            if (fileexists(localPath))
            {
                INFO("Trying to load local replay: {:s}", localPath.c_str());
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
                std::string url = fmt::format("{:s}/api/game/telemetry/downloadReplay?playerId={:s}&leaderboardId={:d}", ScoreSaber::Static::BASE_URL.c_str(), score.leaderboardPlayerInfo.id.value().c_str(), leaderboardId);
                std::vector<char> replayData;
                INFO("Starting replay download");
                long response = WebUtils::DownloadReplaySync(url, replayData, 64);
                if (response == 200)
                {
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
                    ERROR("Got HTTP error {:d}", response);
                    finished(false);
                }
            }
        }).detach();
    }

} // namespace ScoreSaber::ReplaySystem::ReplayLoader