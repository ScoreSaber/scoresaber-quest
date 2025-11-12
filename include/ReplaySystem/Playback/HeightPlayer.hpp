#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "System/zzzz__Object_def.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/GameplayCoreSceneSetupData.hpp>
#include <GlobalNamespace/PlayerHeightDetector.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::ReplaySystem::Playback,
        HeightPlayer,
        System::Object,
        Zenject::ITickable*,
        Zenject::IInitializable*) {
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _nextIndex);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::PlayerHeightDetector>, _playerHeightDetector);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::GameplayCoreSceneSetupData*, _gameplayCoreSceneSetupData);
    DECLARE_CTOR(ctor, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::PlayerHeightDetector* playerHeightDetector, GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
    DECLARE_INSTANCE_METHOD(void, TimeUpdate, float songTime);
    vector<Data::Private::HeightEvent> _sortedHeightEvents;
};
