#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
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

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders, MetadataRecorder);

namespace ScoreSaber::ReplaySystem::Recorders
{
    void MetadataRecorder::ctor(AudioTimeSyncController* audioTimeSyncController, GameplayCoreSceneSetupData* gameplayCoreSceneSetupData, BeatmapObjectSpawnController::InitData* beatmapObjectSpawnControllerInitData, IGameEnergyCounter* gameEnergyCounter, BeatSaber::GameSettings::MainSettingsHandler* mainSettingsHandler)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _beatmapObjectSpawnControllerInitData = beatmapObjectSpawnControllerInitData;
        _gameEnergyCounter = gameEnergyCounter;
        _gameplayCoreSceneSetupData = gameplayCoreSceneSetupData;
        _mainSettingsHandler = mainSettingsHandler;
    }

    void MetadataRecorder::Initialize()
    {
        gameEnergyDidReach0Delegate = custom_types::MakeDelegate<System::Action*>((function<void()>)[&]() {GameEnergyCounter_gameEnergyDidReach0Event();});
        _gameEnergyCounter->add_gameEnergyDidReach0Event(gameEnergyDidReach0Delegate);
    }

    void MetadataRecorder::Dispose()
    {
        _gameEnergyCounter->remove_gameEnergyDidReach0Event(gameEnergyDidReach0Delegate);
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
        metadata->Environment = (string)_gameplayCoreSceneSetupData->environmentInfo->serializedName;
        metadata->Modifiers = ScoreSaber::Services::UploadService::GetModifierList(_gameplayCoreSceneSetupData->gameplayModifiers, -1);
        metadata->NoteSpawnOffset = _beatmapObjectSpawnControllerInitData->noteJumpValue;
        metadata->LeftHanded = _gameplayCoreSceneSetupData->playerSpecificSettings->leftHanded;
        metadata->InitialHeight = _gameplayCoreSceneSetupData->playerSpecificSettings->playerHeight;
        metadata->RoomRotation = _mainSettingsHandler->instance->roomRotation;
        metadata->RoomCenter = VRPosition(_mainSettingsHandler->instance->roomCenter.x, _mainSettingsHandler->instance->roomCenter.y, _mainSettingsHandler->instance->roomCenter.z);
        metadata->FailTime = _failTime;
        return metadata;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders