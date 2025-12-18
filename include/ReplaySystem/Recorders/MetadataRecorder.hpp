#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "Utils/DelegateUtils.hpp"
#include <GlobalNamespace/SettingsManager.hpp>
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/BeatmapObjectSpawnController.hpp>
#include <GlobalNamespace/GameplayCoreSceneSetupData.hpp>
#include <GlobalNamespace/GameEnergyCounter.hpp>
#include <System/IDisposable.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include "Utils/DelegateUtils.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::ReplaySystem::Recorders,
        MetadataRecorder,
        System::Object,
        System::IDisposable*,
        Zenject::IInitializable*) {
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapObjectSpawnController::InitData*, _beatmapObjectSpawnControllerInitData);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::GameplayCoreSceneSetupData*, _gameplayCoreSceneSetupData);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::SettingsManager*, _settingsManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::GameEnergyCounter>, _gameEnergyCounter);
    DECLARE_CTOR(ctor, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData, GlobalNamespace::BeatmapObjectSpawnController::InitData* beatmapObjectSpawnControllerInitData, GlobalNamespace::GameEnergyCounter* gameEnergyCounter, GlobalNamespace::SettingsManager* settingsManager);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_INSTANCE_METHOD(void, GameEnergyCounter_gameEnergyDidReach0Event);
    float _failTime;
    DelegateUtils::DelegateW<System::Action> gameEnergyDidReach0Delegate;
public:
    std::shared_ptr<Data::Private::Metadata> Export();
};