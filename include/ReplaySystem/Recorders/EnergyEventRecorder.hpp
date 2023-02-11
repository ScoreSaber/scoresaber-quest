#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/IInitializable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include <vector>

using namespace GlobalNamespace;

#define INTERFACES                                                        \
    {                                                                     \
        classof(System::IDisposable*), classof(Zenject::IInitializable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, EnergyEventRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_PRIVATE_FIELD(GameEnergyCounter*, _gameEnergyCounter);
                                    DECLARE_CTOR(ctor, AudioTimeSyncController* audioTimeSyncController, GameEnergyCounter* gameEnergyCounter);
                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    DECLARE_INSTANCE_METHOD(void, GameEnergyCounter_gameEnergyDidChangeEvent, float energy);
                                    std::vector<Data::Private::EnergyEvent> _energyKeyframes;
                                    System::Action_1<float> *gameEnergyDidChangeDelegate;
                                    public:
                                    std::vector<Data::Private::EnergyEvent> Export();
                                    )

#undef INTERFACES
