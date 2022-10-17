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
#include "UnityEngine/Resources.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/obfuscation.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"

using namespace UnityEngine;
using namespace QuestUI;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::ReplaySystem::Recorders::MetadataRecorder
{

    float _failTime;

    IDifficultyBeatmap* _difficultyBeatmap;
    MainSettingsModelSO* _mainSettingsModelSO;
    IPreviewBeatmapLevel* _previewBeatmapLevel;
    AudioTimeSyncController* _audioTimeSyncController;
    GameplayCoreSceneSetupData* _gameplayCoreSceneSetupData;
    BeatmapObjectSpawnController::InitData* _beatmapObjectSpawnControllerInitData;

    void LevelStarted(IDifficultyBeatmap* difficultyBeatmap, MainSettingsModelSO* mainSettingsModelSO, IPreviewBeatmapLevel* previewBeatmapLevel,
                      AudioTimeSyncController* audioTimeSyncController, GameplayCoreSceneSetupData* gameplayCoreSceneSetupData,
                      BeatmapObjectSpawnController::InitData* beatmapObjectSpawnControllerInitData)
    {
        _failTime = 0;
        _difficultyBeatmap = difficultyBeatmap;
        _mainSettingsModelSO = mainSettingsModelSO;
        _previewBeatmapLevel = previewBeatmapLevel;
        _audioTimeSyncController = audioTimeSyncController;
        _gameplayCoreSceneSetupData = gameplayCoreSceneSetupData;
        _beatmapObjectSpawnControllerInitData = beatmapObjectSpawnControllerInitData;
    }

    void LevelFailed()
    {
        _failTime = _audioTimeSyncController->songTime;
    }

    Metadata* Export()
    {
        auto metadata = new Metadata();
        metadata->Version = ENCRYPT_STRING_AUTO_A(encoder, "2.0.0");
        metadata->LevelID = static_cast<std::string>(_previewBeatmapLevel->get_levelID());
        metadata->Difficulty = BeatmapDifficultyMethods::DefaultRating(_difficultyBeatmap->get_difficulty());
        metadata->Characteristic = static_cast<std::string>(_difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName);
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
} // namespace ScoreSaber::ReplaySystem::Recorders::MetadataRecorder