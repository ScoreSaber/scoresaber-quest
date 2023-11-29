#include "ReplaySystem/Playback/PosePlayer.hpp"
#include "GlobalNamespace/FloatSO.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "GlobalNamespace/Vector3SO.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/StereoTargetEyeMask.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"

#include "GlobalNamespace/SaberManager.hpp"
#include "UnityEngine/Vector3.hpp"
#include "logging.hpp"

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, PosePlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void PosePlayer::ctor(GlobalNamespace::MainCamera* mainCamera, GlobalNamespace::SaberManager* saberManager, GlobalNamespace::IReturnToMenuController* returnToMenuController, GlobalNamespace::PlayerTransforms* playerTransforms, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController)
    {
        INVOKE_CTOR();
        _sortedPoses = ReplayLoader::LoadedReplay->poseKeyframes;
        _mainCamera = mainCamera;
        _saberManager = saberManager;
        _returnToMenuController = returnToMenuController;
        _playerTransforms = playerTransforms;
        _audioTimeSyncController = audioTimeSyncController;
        _spectatorOffset = Vector3(0, 0, -2);
        _mainSettingsModelSO = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MainSettingsModelSO*>()->values[0];
    }
    void PosePlayer::Initialize()
    {
        SetupCameras();
        _saberManager->leftSaber->get_transform()->GetComponentInParent<GlobalNamespace::VRController*>()->set_enabled(false);
        _saberManager->rightSaber->get_transform()->GetComponentInParent<GlobalNamespace::VRController*>()->set_enabled(false);
    }
    void PosePlayer::SetupCameras()
    {
        _mainCamera->set_enabled(false);
        _mainCamera->get_gameObject()->SetActive(false);
        _desktopCamera = Resources::FindObjectsOfTypeAll<Camera*>().First([&](Camera* camera) {
            return camera->get_name() == "RecorderCamera";
        });

        _desktopCamera->set_fieldOfView(65.0f);
        auto desktopCameraTransform = _desktopCamera->get_transform();
        desktopCameraTransform->set_position(Vector3(desktopCameraTransform->get_position().x, desktopCameraTransform->get_position().y, desktopCameraTransform->get_position().z));
        // _desktopCamera->get_gameObject()->SetActive(true);
        _desktopCamera->set_tag("MainCamera");
        _desktopCamera->set_depth(1);

        //_mainCamera->camera = _desktopCamera;

        GameObject* spectatorObject = GameObject::New_ctor("SpectatorParent");
        _spectatorCamera = UnityEngine::Object::Instantiate(_desktopCamera);
        spectatorObject->get_transform()->set_position(Vector3(_mainSettingsModelSO->roomCenter->value.x + _spectatorOffset.x, _mainSettingsModelSO->roomCenter->value.y + _spectatorOffset.y, _mainSettingsModelSO->roomCenter->value.z + _spectatorOffset.z));
        Quaternion rotation = Quaternion::Euler(0.0f, _mainSettingsModelSO->roomRotation->value, 0.0f);
        spectatorObject->get_transform()->set_rotation(rotation);
        _spectatorCamera->set_stereoTargetEye(StereoTargetEyeMask::Both);
        _spectatorCamera->get_gameObject()->SetActive(true);
        _spectatorCamera->set_depth(0);
        _spectatorCamera->get_transform()->SetParent(spectatorObject->get_transform());
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
        float lerpTime = (_audioTimeSyncController->get_songTime() - activePose.Time) / Mathf::Max(0.0001f, nextPose.Time - activePose.Time);

        _playerTransforms->headTransform->SetPositionAndRotation(VRVector3(activePose.Head.Position), VRQuaternion(activePose.Head.Rotation));

        _saberManager->leftSaber->OverridePositionAndRotation(Vector3::Lerp(VRVector3(activePose.Left.Position), VRVector3(nextPose.Left.Position), lerpTime),
                                                              Quaternion::Lerp(VRQuaternion(activePose.Left.Rotation), VRQuaternion(nextPose.Left.Rotation), lerpTime));

        _saberManager->rightSaber->OverridePositionAndRotation(Vector3::Lerp(VRVector3(activePose.Right.Position), VRVector3(nextPose.Right.Position), lerpTime),
                                                               Quaternion::Lerp(VRQuaternion(activePose.Right.Rotation), VRQuaternion(nextPose.Right.Rotation), lerpTime));

        auto pos = Vector3::Lerp(VRVector3(activePose.Head.Position), VRVector3(nextPose.Head.Position), lerpTime);
        auto rot = Quaternion::Lerp(VRQuaternion(activePose.Head.Rotation), VRQuaternion(nextPose.Head.Rotation), lerpTime);

        auto eulerAngles = rot.get_eulerAngles();
        // TODO: Apply rotation offset

        float t2 = 4.0f == 0.0f ? 1.0f : Time::get_deltaTime() * 6.0f;
        _desktopCamera->get_transform()->SetPositionAndRotation(Vector3::Lerp(_desktopCamera->get_transform()->get_position(), pos, t2), Quaternion::Lerp(_desktopCamera->get_transform()->get_rotation(), rot, t2));

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
        _spectatorCamera->get_transform()->get_parent()->set_position(Vector3(_mainSettingsModelSO->roomCenter->value.x + value.x, _mainSettingsModelSO->roomCenter->value.y + value.y, _mainSettingsModelSO->roomCenter->value.z + value.z));
        _spectatorOffset = value;
    }

    void PosePlayer::AddCallback(std::function<void(ScoreSaber::Data::Private::VRPoseGroup)> callback)
    {
        DidUpdatePose = callback;
    }
} // namespace ScoreSaber::ReplaySystem::Playback