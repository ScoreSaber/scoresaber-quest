#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/IReturnToMenuController.hpp"
#include "GlobalNamespace/MainCamera.hpp"
#include "GlobalNamespace/PlayerTransforms.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/IInitializable.hpp"
#include "Zenject/ITickable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#define INTERFACES                                                                                       \
    {                                                                                                    \
        classof(Zenject::ITickable*), classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Playback, PosePlayer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Playback", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::MainCamera*, _mainCamera);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::SaberManager*, _saberManager);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::IReturnToMenuController*, _returnToMenuController);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::PlayerTransforms*, _playerTransforms);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_PRIVATE_FIELD(int, _lastIndex);
                                    DECLARE_CTOR(ctor, GlobalNamespace::MainCamera* mainCamera, GlobalNamespace::SaberManager* saberManager, GlobalNamespace::IReturnToMenuController* returnToMenuController, GlobalNamespace::PlayerTransforms* playerTransforms, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController);
                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    vector<Data::Private::VRPoseGroup> _sortedPoses;
                                    void UpdatePoses(Data::Private::VRPoseGroup activePose, Data::Private::VRPoseGroup nextPose);
                                    bool ReachedEnd();
                                    void TimeUpdate(float newTime);

)

#undef INTERFACES
