#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController_InitData.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/SinglePlayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/StandardLevelGameplayManager.hpp"
#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "System/Action.hpp"
#include "UnityEngine/Resources.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"

using namespace UnityEngine;
using namespace QuestUI;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;
using namespace ScoreSaber::ReplaySystem;

static ReplayFile* replay;

GlobalNamespace::GameplayCoreSceneSetupData* _gameplayCoreSceneSetupData;
BeatmapObjectSpawnController::InitData* _beatmapObjectSpawnControllerInitData;

MAKE_AUTO_HOOK_MATCH(AudioTimeSyncController_Start, &AudioTimeSyncController::Start, void, AudioTimeSyncController* self)
{
    AudioTimeSyncController_Start(self);
    MainSettingsModelSO* mainSettingsModelSO = ArrayUtil::First(Resources::FindObjectsOfTypeAll<MainSettingsModelSO*>());
    Recorders::MetadataRecorder::LevelStarted(_gameplayCoreSceneSetupData->difficultyBeatmap, mainSettingsModelSO,
                                              _gameplayCoreSceneSetupData->previewBeatmapLevel, self,
                                              _gameplayCoreSceneSetupData, _beatmapObjectSpawnControllerInitData);
}

MAKE_AUTO_HOOK_FIND_CLASS_UNSAFE_INSTANCE(GameplayCoreSceneSetupData_ctor, "", "GameplayCoreSceneSetupData", ".ctor", void,
                                          GlobalNamespace::GameplayCoreSceneSetupData* self, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap,
                                          GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel,
                                          GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
                                          GlobalNamespace::PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects,
                                          GlobalNamespace::EnvironmentInfoSO* environmentInfo, GlobalNamespace::ColorScheme* colorScheme)
{
    INFO("GameplayCoreSceneSetupData ctor hit");
    GameplayCoreSceneSetupData_ctor(self, difficultyBeatmap, previewBeatmapLevel, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects, environmentInfo, colorScheme);
    _gameplayCoreSceneSetupData = self;
}

MAKE_AUTO_HOOK_FIND_CLASS_UNSAFE_INSTANCE(BeatmapObjectSpawnControllerInitData_ctor, "", "BeatmapObjectSpawnController/InitData", ".ctor", void,
                                          BeatmapObjectSpawnController::InitData* self, float beatsPerMinute,
                                          int noteLinesCount, float noteJumpMovementSpeed, float noteJumpStartBeatOffset, float jumpOffsetY)
{
    INFO("BeatmapObjectSpawnControllerInitData ctor hit");
    BeatmapObjectSpawnControllerInitData_ctor(self, beatsPerMinute, noteLinesCount, noteJumpMovementSpeed, noteJumpStartBeatOffset, jumpOffsetY);
    _beatmapObjectSpawnControllerInitData = self;
}

MAKE_AUTO_HOOK_MATCH(StandardLevelGameplayManager_HandleGameEnergyDidReach0, &StandardLevelGameplayManager::HandleGameEnergyDidReach0, void,
                     StandardLevelGameplayManager* self)
{
    INFO("levelFailed hit");
    StandardLevelGameplayManager_HandleGameEnergyDidReach0(self);
    Recorders::MetadataRecorder::LevelFailed();
}