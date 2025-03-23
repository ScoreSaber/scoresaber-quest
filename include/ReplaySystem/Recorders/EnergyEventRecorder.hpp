#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/GameEnergyCounter.hpp>
#include <System/IDisposable.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include <vector>
#include "Utils/DelegateUtils.hpp"

using namespace GlobalNamespace;

#define INTERFACES                                                        \
    {                                                                     \
        classof(System::IDisposable*), classof(Zenject::IInitializable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, EnergyEventRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<AudioTimeSyncController>, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GameEnergyCounter>, _gameEnergyCounter);
                                    DECLARE_CTOR(ctor, AudioTimeSyncController* audioTimeSyncController, GameEnergyCounter* gameEnergyCounter);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
                                    DECLARE_INSTANCE_METHOD(void, GameEnergyCounter_gameEnergyDidChangeEvent, float energy);
                                    std::vector<Data::Private::EnergyEvent> _energyKeyframes;
                                    DelegateUtils::DelegateW<System::Action_1<float>> gameEnergyDidChangeDelegate;
                                    public:
                                    std::vector<Data::Private::EnergyEvent> Export();
                                    )

#undef INTERFACES
