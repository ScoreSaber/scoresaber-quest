#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/PlayerTransforms.hpp>
#include <UnityEngine/Quaternion.hpp>
#include <UnityEngine/Vector3.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include <vector>

using namespace GlobalNamespace;
using namespace UnityEngine;

#define INTERFACES                    \
    {                                 \
        classof(Zenject::ITickable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, PoseRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<AudioTimeSyncController>, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<PlayerTransforms>, _playerTransforms);
                                    DECLARE_CTOR(ctor, AudioTimeSyncController* audioTimeSyncController, PlayerTransforms* playerTransforms);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
                                    std::vector<Data::Private::VRPoseGroup> _vrPoseGroup;
                                    volatile bool _recording;
                                    public:
                                    std::vector<Data::Private::VRPoseGroup> Export();
                                    void StopRecording();
                                    )

#undef INTERFACES