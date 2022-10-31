#include "ReplaySystem/ReplayLoader.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayReader.hpp"
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
    ScoreSaber::ReplaySystem::Playback::NotePlayer* NotePlayerInstance;

    bool IsPlaying;

    void LoadReplay()
    {
        // 76561198283584459-eden-Hard-Standard-64688EE557A6C0916D2108AA3B39DD79C8468DB3.dat
        // std::string replayFileName = "76561198283584459-eden-Hard-Standard-64688EE557A6C0916D2108AA3B39DD79C8468DB3.dat";
        // LoadedReplay = ScoreSaber::Data::Private::ReplayReader::Read(ScoreSaber::Static::REPLAY_DIR + "/" + replayFileName, replayFileName);
        std::string replayFileName = "3225556157461414-Exit This Earth's Atomosphere-ExpertPlus-Standard-14149559186C24D676EC5032070B52006843923A.tmp";
        LoadedReplay = ScoreSaber::Data::Private::ReplayReader::Read(ScoreSaber::Static::REPLAY_TMP_DIR + "/" + replayFileName, replayFileName);
    }

    void StartReplay(IDifficultyBeatmap* beatmap)
    {
        INFO("1");
        IsPlaying = true;
        INFO("2");
        if (playerDataModel == nullptr)
        {
            playerDataModel = UnityEngine::Object::FindObjectOfType<PlayerDataModel*>();
        }

        INFO("3");

        if (menuTransitionsHelper == nullptr)
        {
            menuTransitionsHelper = UnityEngine::Object::FindObjectOfType<MenuTransitionsHelper*>();
        }

        INFO("4");

        auto playerData = playerDataModel->playerData;

        INFO("5");

        auto localPlayerSettings = playerData->playerSpecificSettings;

        INFO("6");

        auto playerSettings = PlayerSpecificSettings::New_ctor(LoadedReplay->metadata->LeftHanded, localPlayerSettings->playerHeight,
                                                               localPlayerSettings->automaticPlayerHeight, localPlayerSettings->sfxVolume,
                                                               localPlayerSettings->reduceDebris, localPlayerSettings->noTextsAndHuds, localPlayerSettings->noFailEffects,
                                                               localPlayerSettings->advancedHud, localPlayerSettings->autoRestart, localPlayerSettings->saberTrailIntensity,
                                                               localPlayerSettings->noteJumpDurationTypeSettings, localPlayerSettings->noteJumpFixedDuration,
                                                               localPlayerSettings->noteJumpStartBeatOffset, localPlayerSettings->hideNoteSpawnEffect, localPlayerSettings->adaptiveSfx,
                                                               localPlayerSettings->environmentEffectsFilterDefaultPreset, localPlayerSettings->environmentEffectsFilterExpertPlusPreset);

        INFO("7");

        std::function<void(StandardLevelScenesTransitionSetupDataSO*, LevelCompletionResults*)> ReplayEndCallback = [&](StandardLevelScenesTransitionSetupDataSO* standardLevelSceneSetupData, LevelCompletionResults* levelCompletionResults) {
            IsPlaying = false;
        };

        INFO("8");

        auto replayEndDelegate = custom_types::MakeDelegate<System::Action_2<StandardLevelScenesTransitionSetupDataSO*, LevelCompletionResults*>*>(
            classof(System::Action_2<StandardLevelScenesTransitionSetupDataSO*, LevelCompletionResults*>*),
            ReplayEndCallback);

        INFO("9");

        auto previewBeatmapLevel = reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(beatmap->get_level());

        INFO("10");

        menuTransitionsHelper->StartStandardLevel("Replay", beatmap, previewBeatmapLevel,
                                                  playerData->overrideEnvironmentSettings,
                                                  playerData->colorSchemesSettings->GetSelectedColorScheme(), BeatmapUtils::GetModifiersFromStrings(LoadedReplay->metadata->Modifiers),
                                                  playerSettings, nullptr, "Exit Replay", false, false, nullptr, replayEndDelegate, nullptr);

        INFO("11");
    }

    void GetReplayData(GlobalNamespace::IDifficultyBeatmap* beatmap, int leaderboardId, ScoreSaber::Data::Score& score, const std::function<void(bool)>& finished)
    {
        // TODO: Implement local replay
        INFO("1");
        auto previewBeatmapLevel = reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(beatmap->get_level());
        INFO("2");
        std::string levelHash = StringUtils::GetFormattedHash(previewBeatmapLevel->get_levelID());
        INFO("3");
        std::string characteristic = beatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName;
        INFO("4");
        std::string songName = previewBeatmapLevel->get_songName();
        INFO("5");
        std::string difficultyName = GlobalNamespace::BeatmapDifficultySerializedMethods::SerializedName(beatmap->get_difficulty());
        INFO("6");
        std::string fileName = ScoreSaber::Services::FileService::GetReplayFileName(levelHash, difficultyName, characteristic, score.leaderboardPlayerInfo.id.value(), songName);
        INFO("7");

        std::string tmpFilePath = ScoreSaber::Static::REPLAY_TMP_DIR + "/" + fileName + ".tmp";
        INFO("8");

        std::thread t([tmpFilePath, fileName, leaderboardId, score, finished] {
            INFO("9");
            std::string url = string_format("%s/api/game/telemetry/downloadReplay?playerId=%s&leaderboardId=%d", ScoreSaber::Static::BASE_URL.c_str(), score.leaderboardPlayerInfo.id.value().c_str(), leaderboardId);
            INFO("URL: %s", url.c_str());
            long response = WebUtils::DownloadFileSync(url, tmpFilePath, 64);
            INFO("10");
            if (response == 200)
            {
                INFO("11");
                LoadedReplay = ScoreSaber::Data::Private::ReplayReader::Read(tmpFilePath, fileName);
                INFO("12");
                finished(true);
            }
            else
            {
                finished(false);
            }
        });
        t.detach();
    }

} // namespace ScoreSaber::ReplaySystem::ReplayLoader