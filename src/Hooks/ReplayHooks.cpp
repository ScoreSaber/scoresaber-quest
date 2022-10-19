#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController_InitData.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnMovementData.hpp"
#include "GlobalNamespace/ComboController.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/PlayerHeightDetector.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PlayerTransforms.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "GlobalNamespace/SinglePlayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/StandardGameplayInstaller.hpp"
#include "GlobalNamespace/StandardLevelGameplayManager.hpp"
#include "ReplaySystem/Recorders/EnergyEventRecorder.hpp"
#include "ReplaySystem/Recorders/HeightEventRecorder.hpp"
#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "ReplaySystem/Recorders/PoseRecorder.hpp"
#include "ReplaySystem/Recorders/ScoreEventRecorder.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "System/Action.hpp"
#include "UnityEngine/Resources.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/MemoryPoolIdInitialSizeMaxSizeBinder_1.hpp"
#include "Zenject/MemoryPool_1.hpp"
#include "Zenject/SceneContext.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"

using namespace UnityEngine;
using namespace QuestUI;
using namespace Zenject;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;
using namespace ScoreSaber::ReplaySystem;

GameplayCoreSceneSetupData* _gameplayCoreSceneSetupData;
BeatmapObjectSpawnController::InitData* _beatmapObjectSpawnControllerInitData;
AudioTimeSyncController* _audioTimeSyncController;

MAKE_AUTO_HOOK_MATCH(AudioTimeSyncController_Start, &AudioTimeSyncController::Start, void, AudioTimeSyncController* self)
{
    AudioTimeSyncController_Start(self);
    _audioTimeSyncController = self;

    if (!ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        MainSettingsModelSO* mainSettingsModelSO = ArrayUtil::First(Resources::FindObjectsOfTypeAll<MainSettingsModelSO*>());
        SaberManager* saberManager = ArrayUtil::First(Resources::FindObjectsOfTypeAll<SaberManager*>());
        PlayerHeightDetector* playerHeightDetector = ArrayUtil::First(Resources::FindObjectsOfTypeAll<PlayerHeightDetector*>());
        ScoreController* scoreController = ArrayUtil::First(Resources::FindObjectsOfTypeAll<ScoreController*>());
        ComboController* comboController = ArrayUtil::First(Resources::FindObjectsOfTypeAll<ComboController*>());
        GameEnergyCounter* gameEnergyCountry = ArrayUtil::First(Resources::FindObjectsOfTypeAll<GameEnergyCounter*>());

        Recorders::MetadataRecorder::LevelStarted(_gameplayCoreSceneSetupData->difficultyBeatmap, mainSettingsModelSO,
                                                  _gameplayCoreSceneSetupData->previewBeatmapLevel, _audioTimeSyncController,
                                                  _gameplayCoreSceneSetupData, _beatmapObjectSpawnControllerInitData);
        Recorders::PoseRecorder::LevelStarted(saberManager, _audioTimeSyncController);
        Recorders::HeightEventRecorder::LevelStarted(playerHeightDetector, _audioTimeSyncController);
        Recorders::ScoreEventRecorder::LevelStarted(scoreController, _audioTimeSyncController, comboController);
        Recorders::EnergyEventRecorder::LevelStarted(gameEnergyCountry, _audioTimeSyncController);
        Recorders::NoteEventRecorder::LevelStarted(scoreController, _audioTimeSyncController);
    }
}

MAKE_AUTO_HOOK_FIND_CLASS_UNSAFE_INSTANCE(GameplayCoreSceneSetupData_ctor, "", "GameplayCoreSceneSetupData", ".ctor", void,
                                          GameplayCoreSceneSetupData* self, IDifficultyBeatmap* difficultyBeatmap, IPreviewBeatmapLevel* previewBeatmapLevel,
                                          GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings,
                                          bool useTestNoteCutSoundEffects, EnvironmentInfoSO* environmentInfo, ColorScheme* colorScheme, MainSettingsModelSO* mainSettingsModel)
{
    GameplayCoreSceneSetupData_ctor(self, difficultyBeatmap, previewBeatmapLevel, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects,
                                    environmentInfo, colorScheme, mainSettingsModel);
    _gameplayCoreSceneSetupData = self;
}

MAKE_AUTO_HOOK_FIND_CLASS_UNSAFE_INSTANCE(BeatmapObjectSpawnControllerInitData_ctor, "", "BeatmapObjectSpawnController/InitData", ".ctor", void,
                                          BeatmapObjectSpawnController::InitData* self, float beatsPerMinute,
                                          int noteLinesCount, float noteJumpMovementSpeed, BeatmapObjectSpawnMovementData::NoteJumpValueType noteJumpValueType, float noteJumpValue)
{
    BeatmapObjectSpawnControllerInitData_ctor(self, beatsPerMinute, noteLinesCount, noteJumpMovementSpeed, noteJumpValueType, noteJumpValue);
    _beatmapObjectSpawnControllerInitData = self;
}

MAKE_AUTO_HOOK_MATCH(PlayerTransforms_Update, &PlayerTransforms::Update, void, PlayerTransforms* self)
{
    PlayerTransforms_Update(self);

    if (_audioTimeSyncController != nullptr && !ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        Recorders::PoseRecorder::Tick(self->get_headPseudoLocalPos(), self->get_headPseudoLocalRot());
    }
}

MAKE_AUTO_HOOK_MATCH(StandardLevelGameplayManager_HandleGameEnergyDidReach0, &StandardLevelGameplayManager::HandleGameEnergyDidReach0, void,
                     StandardLevelGameplayManager* self)
{
    StandardLevelGameplayManager_HandleGameEnergyDidReach0(self);
    if (!ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        Recorders::MetadataRecorder::LevelFailed();
    }
}

MAKE_AUTO_HOOK_MATCH(ScoreController_HandleNoteWasCut, &ScoreController::HandleNoteWasCut, void, ScoreController* self,
                     GlobalNamespace::NoteController* noteController, ByRef<GlobalNamespace::NoteCutInfo> noteCutInfo)
{
    ScoreController_HandleNoteWasCut(self, noteController, noteCutInfo);
    if (!ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        Recorders::NoteEventRecorder::BadCutInfoCollector(noteController, noteCutInfo.heldRef);
    }
}