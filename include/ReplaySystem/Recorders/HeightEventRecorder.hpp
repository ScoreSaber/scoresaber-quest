#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/PlayerHeightDetector.hpp"
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

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, HeightEventRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_PRIVATE_FIELD(PlayerHeightDetector*, _playerHeightDetector);
                                    DECLARE_CTOR(ctor, AudioTimeSyncController* audioTimeSyncController, Zenject::DiContainer* container);
                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    DECLARE_INSTANCE_METHOD(void, PlayerHeightDetector_playerHeightDidChangeEvent, float newHeight);
                                    std::vector<Data::Private::HeightEvent> _heightKeyframes;
                                    System::Action_1<float> *playerHeightDidChangeDelegate;
                                    public:
                                    std::vector<Data::Private::HeightEvent> Export();
                                    )

#undef INTERFACES