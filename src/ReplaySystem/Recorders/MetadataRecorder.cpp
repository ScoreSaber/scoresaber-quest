#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController_InitData.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/FloatSO.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/SinglePlayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/Vector3SO.hpp"
#include "Services/UploadService.hpp"
#include "System/Action.hpp"
#include "UnityEngine/Resources.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"
#include "custom-types/shared/delegate.hpp"
#include <functional>

using namespace UnityEngine;
using namespace QuestUI;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders, MetadataRecorder);

namespace ScoreSaber::ReplaySystem::Recorders
{
    void MetadataRecorder::ctor(AudioTimeSyncController* audioTimeSyncController, GameplayCoreSceneSetupData* gameplayCoreSceneSetupData, BeatmapObjectSpawnController::InitData* beatmapObjectSpawnControllerInitData, IGameEnergyCounter* gameEnergyCounter)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _beatmapObjectSpawnControllerInitData = beatmapObjectSpawnControllerInitData;
        _mainSettingsModelSO = ArrayUtil::First(Resources::FindObjectsOfTypeAll<MainSettingsModelSO*>());
        _gameEnergyCounter = gameEnergyCounter;
        _gameplayCoreSceneSetupData = gameplayCoreSceneSetupData;
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
    
    Metadata* MetadataRecorder::Export()
    {
        auto metadata = new Metadata();
        metadata->Version = "2.0.0";
        metadata->LevelID = static_cast<std::string>(_gameplayCoreSceneSetupData->difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID());
        metadata->Difficulty = BeatmapDifficultyMethods::DefaultRating(_gameplayCoreSceneSetupData->difficultyBeatmap->get_difficulty());
        metadata->Characteristic = static_cast<std::string>(_gameplayCoreSceneSetupData->difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName);
        metadata->Environment = static_cast<std::string>(_gameplayCoreSceneSetupData->environmentInfo->serializedName);
        metadata->Modifiers = ScoreSaber::Services::UploadService::GetModifierList(_gameplayCoreSceneSetupData->gameplayModifiers, -1);
        metadata->NoteSpawnOffset = _beatmapObjectSpawnControllerInitData->noteJumpValue;
        metadata->LeftHanded = _gameplayCoreSceneSetupData->playerSpecificSettings->leftHanded;
        metadata->InitialHeight = _gameplayCoreSceneSetupData->playerSpecificSettings->playerHeight;
        metadata->RoomRotation = _mainSettingsModelSO->roomRotation->value;
        metadata->RoomCenter = VRPosition(_mainSettingsModelSO->roomCenter->value.x, _mainSettingsModelSO->roomCenter->value.y,
                                          _mainSettingsModelSO->roomCenter->value.z);
        metadata->FailTime = _failTime;
        return metadata;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders