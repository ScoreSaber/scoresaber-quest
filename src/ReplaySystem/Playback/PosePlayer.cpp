#include "ReplaySystem/Playback/PosePlayer.hpp"
#include <GlobalNamespace/Saber.hpp>
#include <GlobalNamespace/VRController.hpp>
#include <BeatSaber/GameSettings/MainSettings.hpp>
#include "ReplaySystem/ReplayLoader.hpp"
#include <UnityEngine/Component.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/Object.hpp>
#include <UnityEngine/Quaternion.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/StereoTargetEyeMask.hpp>
#include <UnityEngine/Time.hpp>
#include <UnityEngine/Transform.hpp>
#include <UnityEngine/Vector3.hpp>
#include <UnityEngine/SpatialTracking/TrackedPoseDriver.hpp>

#include <GlobalNamespace/SaberManager.hpp>
#include <UnityEngine/Vector3.hpp>
#include "logging.hpp"

using namespace UnityEngine;
using namespace UnityEngine::SpatialTracking;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, PosePlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void PosePlayer::ctor(GlobalNamespace::MainCamera* mainCamera, GlobalNamespace::SaberManager* saberManager, GlobalNamespace::IReturnToMenuController* returnToMenuController, GlobalNamespace::PlayerTransforms* playerTransforms, BeatSaber::GameSettings::MainSettingsHandler* mainSettingsHandler, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController)
    {
        INVOKE_CTOR();
        _mainCamera = mainCamera;
        _saberManager = saberManager;
        _sortedPoses = ReplayLoader::LoadedReplay->poseKeyframes;
        _returnToMenuController = returnToMenuController;
        _spectatorOffset = Vector3(0, 0, -2);
        _playerTransforms = playerTransforms;
        _mainSettingsHandler = mainSettingsHandler;
        _audioTimeSyncController = audioTimeSyncController;
    }
    void PosePlayer::Initialize()
    {
        SetupCameras();
        _saberManager->leftSaber->transform->GetComponentInParent<GlobalNamespace::VRController*>()->enabled = false;
        _saberManager->rightSaber->transform->GetComponentInParent<GlobalNamespace::VRController*>()->enabled = false;
    }
    void PosePlayer::SetupCameras()
    {
        _mainCamera->enabled = false;
        _mainCamera->gameObject->SetActive(false);
        _desktopCamera = Resources::FindObjectsOfTypeAll<Camera*>()->First([](Camera* camera) {
            return camera->name == "RecorderCamera";
        });

        _desktopCamera->fieldOfView = 65.0f;
        auto desktopCameraTransform = _desktopCamera->transform;
        desktopCameraTransform->position = Vector3(desktopCameraTransform->position.x, desktopCameraTransform->position.y, desktopCameraTransform->position.z);
        // _desktopCamera->gameObject->SetActive(true);
        _desktopCamera->tag = "MainCamera";
        _desktopCamera->depth = 1;

        //_mainCamera->camera = _desktopCamera;

        GameObject* spectatorObject = GameObject::New_ctor("SpectatorParent");
        _spectatorCamera = UnityEngine::Object::Instantiate(_desktopCamera);
        spectatorObject->transform->position = Vector3(_mainSettingsHandler->instance->roomCenter.x + _spectatorOffset.x, _mainSettingsHandler->instance->roomCenter.y + _spectatorOffset.y, _mainSettingsHandler->instance->roomCenter.z + _spectatorOffset.z);
        Quaternion rotation = Quaternion::Euler(0.0f, _mainSettingsHandler->instance->roomRotation, 0.0f);
        spectatorObject->transform->rotation = rotation;
        _spectatorCamera->stereoTargetEye = StereoTargetEyeMask::Both;

        // copy headset tracking to spectator camera
        auto oldTrackedPoseDriver = _mainCamera->gameObject->GetComponent<TrackedPoseDriver*>();
        auto newTrackedPoseDriver = _spectatorCamera->gameObject->AddComponent<TrackedPoseDriver*>();
        newTrackedPoseDriver->UseRelativeTransform = oldTrackedPoseDriver->UseRelativeTransform;
        newTrackedPoseDriver->deviceType = oldTrackedPoseDriver->deviceType;
        newTrackedPoseDriver->m_Device = oldTrackedPoseDriver->m_Device;
        newTrackedPoseDriver->m_OriginPose = oldTrackedPoseDriver->m_OriginPose;
        newTrackedPoseDriver->m_PoseProviderComponent = oldTrackedPoseDriver->m_PoseProviderComponent;
        newTrackedPoseDriver->m_PoseSource = oldTrackedPoseDriver->m_PoseSource;
        newTrackedPoseDriver->m_TrackingType = oldTrackedPoseDriver->m_TrackingType;
        newTrackedPoseDriver->m_UpdateType = oldTrackedPoseDriver->m_UpdateType;
        newTrackedPoseDriver->m_UseRelativeTransform = oldTrackedPoseDriver->m_UseRelativeTransform;
        newTrackedPoseDriver->originPose = oldTrackedPoseDriver->originPose;
        newTrackedPoseDriver->poseProviderComponent = oldTrackedPoseDriver->poseProviderComponent;
        newTrackedPoseDriver->poseSource = oldTrackedPoseDriver->poseSource;
        newTrackedPoseDriver->trackingType = oldTrackedPoseDriver->trackingType;
        newTrackedPoseDriver->updateType = oldTrackedPoseDriver->updateType;


        _spectatorCamera->gameObject->SetActive(true);
        _spectatorCamera->depth = 0;
        _spectatorCamera->transform->SetParent(spectatorObject->transform);
    }
    void PosePlayer::Tick()
    {
        if (ReachedEnd()) {
            _returnToMenuController->ReturnToMenu();
            return;
        }
        bool foundPoseThisFrame = false;
        while (_audioTimeSyncController->songTime >= _sortedPoses[_nextIndex].Time)
        {
            foundPoseThisFrame = true;
            VRPoseGroup activePose = _sortedPoses[_nextIndex++];
            if (ReachedEnd()) {
                return;
            }

            VRPoseGroup nextPose = _sortedPoses[_nextIndex];
            UpdatePoses(activePose, nextPose);
        }
        if (foundPoseThisFrame) {
            return;
        } else if (_nextIndex > 0 && !ReachedEnd()) {
            VRPoseGroup activePose = _sortedPoses[_nextIndex - 1];
            VRPoseGroup nextPose = _sortedPoses[_nextIndex];
            UpdatePoses(activePose, nextPose);
        }
    }
    void PosePlayer::Dispose()
    {
    }
    void PosePlayer::UpdatePoses(Data::Private::VRPoseGroup activePose, Data::Private::VRPoseGroup nextPose)
    {
        float lerpTime = (_audioTimeSyncController->songTime - activePose.Time) / Mathf::Max(0.0001f, nextPose.Time - activePose.Time);

        _playerTransforms->_headTransform->SetPositionAndRotation(VRVector3(activePose.Head.Position), VRQuaternion(activePose.Head.Rotation));

        _saberManager->leftSaber->OverridePositionAndRotation(Vector3::Lerp(VRVector3(activePose.Left.Position), VRVector3(nextPose.Left.Position), lerpTime),
                                                              Quaternion::Lerp(VRQuaternion(activePose.Left.Rotation), VRQuaternion(nextPose.Left.Rotation), lerpTime));

        _saberManager->rightSaber->OverridePositionAndRotation(Vector3::Lerp(VRVector3(activePose.Right.Position), VRVector3(nextPose.Right.Position), lerpTime),
                                                               Quaternion::Lerp(VRQuaternion(activePose.Right.Rotation), VRQuaternion(nextPose.Right.Rotation), lerpTime));

        auto pos = Vector3::Lerp(VRVector3(activePose.Head.Position), VRVector3(nextPose.Head.Position), lerpTime);
        auto rot = Quaternion::Lerp(VRQuaternion(activePose.Head.Rotation), VRQuaternion(nextPose.Head.Rotation), lerpTime);

        auto eulerAngles = rot.eulerAngles;
        // TODO: Apply rotation offset

        float t2 = 4.0f == 0.0f ? 1.0f : Time::get_deltaTime() * 6.0f;
        _desktopCamera->transform->SetPositionAndRotation(Vector3::Lerp(_desktopCamera->transform->position, pos, t2), Quaternion::Lerp(_desktopCamera->transform->rotation, rot, t2));

        // TODO: Move camera

        if (DidUpdatePose != nullptr)
        {
            DidUpdatePose(activePose);
        }
    }

    bool PosePlayer::ReachedEnd()
    {
        return _nextIndex >= _sortedPoses.size();
    }

    void PosePlayer::TimeUpdate(float newTime)
    {
        for (int c = 0; c < _sortedPoses.size(); c++) {
            if (_sortedPoses[c].Time > newTime) {
                _nextIndex = c;
                return;
            }
        }
        _nextIndex = _sortedPoses.size();
    }

    void PosePlayer::SetSpectatorOffset(Vector3 value) {
        _spectatorCamera->transform->parent->position = Vector3(_mainSettingsHandler->instance->roomCenter.x + value.x, _mainSettingsHandler->instance->roomCenter.y + value.y, _mainSettingsHandler->instance->roomCenter.z + value.z);
        _spectatorOffset = value;
    }

    void PosePlayer::AddCallback(std::function<void(ScoreSaber::Data::Private::VRPoseGroup)> callback)
    {
        DidUpdatePose = callback;
    }
} // namespace ScoreSaber::ReplaySystem::Playback