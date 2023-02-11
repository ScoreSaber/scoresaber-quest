#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/MainCamera.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Vector3.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/ITickable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include <vector>

using namespace GlobalNamespace;
using namespace UnityEngine;

#define INTERFACES                    \
    {                                 \
        classof(Zenject::ITickable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, PoseRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_PRIVATE_FIELD(MainCamera*, _mainCamera);
                                    DECLARE_PRIVATE_FIELD(VRController*, _controllerLeft);
                                    DECLARE_PRIVATE_FIELD(VRController*, _controllerRight);
                                    DECLARE_CTOR(ctor, AudioTimeSyncController* audioTimeSyncController, MainCamera* mainCamera, SaberManager* saberManager);
                                    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());
                                    std::vector<Data::Private::VRPoseGroup> _vrPoseGroup;
                                    public:
                                    std::vector<Data::Private::VRPoseGroup> Export();
                                    )

#undef INTERFACES