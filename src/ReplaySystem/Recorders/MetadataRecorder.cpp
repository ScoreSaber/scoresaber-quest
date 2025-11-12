#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "Services/UploadService.hpp"
#include <GlobalNamespace/BeatmapLevel.hpp>
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/BeatmapCharacteristicSO.hpp>
#include <GlobalNamespace/BeatmapDifficulty.hpp>
#include <GlobalNamespace/BeatmapDifficultyMethods.hpp>
#include <GlobalNamespace/EnvironmentInfoSO.hpp>
#include <GlobalNamespace/GameEnergyCounter.hpp>
#include <GlobalNamespace/GameplayCoreSceneSetupData.hpp>
#include <GlobalNamespace/PlayerSpecificSettings.hpp>
#include <GlobalNamespace/SinglePlayerLevelSelectionFlowCoordinator.hpp>
#include <System/Action.hpp>
#include <custom-types/shared/delegate.hpp>
#include <functional>
#include "logging.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders, MetadataRecorder);

namespace ScoreSaber::ReplaySystem::Recorders
{
    void MetadataRecorder::ctor(AudioTimeSyncController* audioTimeSyncController, GameplayCoreSceneSetupData* gameplayCoreSceneSetupData, BeatmapObjectSpawnController::InitData* beatmapObjectSpawnControllerInitData, GameEnergyCounter* gameEnergyCounter, GlobalNamespace::SettingsManager* settingsManager)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _beatmapObjectSpawnControllerInitData = beatmapObjectSpawnControllerInitData;
        _gameEnergyCounter = gameEnergyCounter;
        _gameplayCoreSceneSetupData = gameplayCoreSceneSetupData;
        _settingsManager = settingsManager;
    }

    void MetadataRecorder::Initialize()
    {
        gameEnergyDidReach0Delegate = { &MetadataRecorder::GameEnergyCounter_gameEnergyDidReach0Event, this };
        _gameEnergyCounter->___gameEnergyDidReach0Event += gameEnergyDidReach0Delegate;
    }

    void MetadataRecorder::Dispose()
    {
        if(_gameEnergyCounter)
            _gameEnergyCounter->___gameEnergyDidReach0Event -= gameEnergyDidReach0Delegate;
    }

    void MetadataRecorder::GameEnergyCounter_gameEnergyDidReach0Event()
    {
        _failTime = _audioTimeSyncController->songTime;
    }
    
    std::shared_ptr<Metadata> MetadataRecorder::Export()
    {
        StringW csc;
        auto metadata = make_shared<Metadata>();
        metadata->Version = "3.0.0";
        metadata->LevelID = (string)_gameplayCoreSceneSetupData->beatmapLevel->levelID;
        metadata->Difficulty = BeatmapDifficultyMethods::DefaultRating(_gameplayCoreSceneSetupData->beatmapKey.difficulty);
        metadata->Characteristic = (string)_gameplayCoreSceneSetupData->beatmapKey.beatmapCharacteristic->serializedName;
        metadata->Environment = (string)_gameplayCoreSceneSetupData->targetEnvironmentInfo->serializedName;
        metadata->Modifiers = ScoreSaber::Services::UploadService::GetModifierList(_gameplayCoreSceneSetupData->gameplayModifiers, -1);
        metadata->NoteSpawnOffset = _beatmapObjectSpawnControllerInitData->noteJumpValue;
        metadata->LeftHanded = _gameplayCoreSceneSetupData->playerSpecificSettings->leftHanded;
        metadata->InitialHeight = _gameplayCoreSceneSetupData->playerSpecificSettings->playerHeight;
        metadata->RoomRotation = _settingsManager->settings.room.rotation;
        metadata->RoomCenter = VRPosition(_settingsManager->settings.room.center.x, _settingsManager->settings.room.center.y, _settingsManager->settings.room.center.z);
        metadata->FailTime = _failTime;
        return metadata;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders