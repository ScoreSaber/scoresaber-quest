#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/IReturnToMenuController.hpp>
#include <GlobalNamespace/MainCamera.hpp>
#include <GlobalNamespace/PlayerTransforms.hpp>
#include <GlobalNamespace/SaberManager.hpp>
#include <BeatSaber/GameSettings/MainSettingsHandler.hpp>
#include <System/IDisposable.hpp>
#include <UnityEngine/Camera.hpp>
#include <UnityEngine/Vector3.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

#define INTERFACES                                                                                       \
    {                                                                                                    \
        classof(Zenject::ITickable*), classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Playback, PosePlayer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Playback", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::MainCamera>, _mainCamera);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::SaberManager>, _saberManager);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IReturnToMenuController*, _returnToMenuController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::PlayerTransforms>, _playerTransforms);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(int, _nextIndex);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Camera*, _spectatorCamera);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Camera*, _desktopCamera);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Vector3, _spectatorOffset);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(BeatSaber::GameSettings::MainSettingsHandler*, _mainSettingsHandler);
                                    DECLARE_CTOR(ctor, GlobalNamespace::MainCamera* mainCamera, GlobalNamespace::SaberManager* saberManager, GlobalNamespace::IReturnToMenuController* returnToMenuController, GlobalNamespace::PlayerTransforms* playerTransforms, BeatSaber::GameSettings::MainSettingsHandler* mainSettingsHandler, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
                                    DECLARE_INSTANCE_METHOD(void, TimeUpdate, float newTime);
                                    vector<Data::Private::VRPoseGroup> _sortedPoses;
                                    std::function<void(ScoreSaber::Data::Private::VRPoseGroup)> DidUpdatePose;
                                    void UpdatePoses(Data::Private::VRPoseGroup activePose, Data::Private::VRPoseGroup nextPose);
                                    bool ReachedEnd();
                                    void SetupCameras();
                                    public:
                                    void SetSpectatorOffset(UnityEngine::Vector3 value);
                                    void AddCallback(std::function<void(ScoreSaber::Data::Private::VRPoseGroup)> callback);

);

#undef INTERFACES
