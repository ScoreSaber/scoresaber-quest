#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/GameplayCoreSceneSetupData.hpp>
#include <GlobalNamespace/PlayerHeightDetector.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

#define INTERFACES                                                       \
    {                                                                    \
        classof(Zenject::ITickable*), classof(Zenject::IInitializable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Playback, HeightPlayer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Playback", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(int, _nextIndex);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::PlayerHeightDetector*, _playerHeightDetector);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::GameplayCoreSceneSetupData*, _gameplayCoreSceneSetupData);
                                    DECLARE_CTOR(ctor, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::PlayerHeightDetector* playerHeightDetector, GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
                                    DECLARE_INSTANCE_METHOD(void, TimeUpdate, float songTime);
                                    vector<Data::Private::HeightEvent> _sortedHeightEvents;)

#undef INTERFACES
