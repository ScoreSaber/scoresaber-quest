#include "ReplaySystem/Recorders/PoseRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/MainCamera.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"

using namespace UnityEngine;
using namespace QuestUI;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders, PoseRecorder);

namespace ScoreSaber::ReplaySystem::Recorders
{
    void PoseRecorder::ctor(AudioTimeSyncController* audioTimeSyncController, MainCamera* mainCamera, SaberManager* saberManager)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _mainCamera = mainCamera;
        _controllerLeft = saberManager->leftSaber->GetComponentInParent<VRController*>();
        _controllerRight = saberManager->rightSaber->GetComponentInParent<VRController*>();
    }

    void PoseRecorder::Tick()
    {
        Vector3 headPos = _mainCamera->get_position();
        Quaternion headRot = _mainCamera->get_rotation();

        Vector3 leftControllerPosition = _controllerLeft->get_position();
        Quaternion leftControllerRotation = _controllerLeft->get_rotation();

        Vector3 rightControllerPosition = _controllerRight->get_position();
        Quaternion rightControllerRotation = _controllerRight->get_rotation();

        int fps = (int)(1 / Time::get_unscaledDeltaTime());
        _vrPoseGroup.push_back(VRPoseGroup(
            VRPose(VRPosition(headPos.x, headPos.y,
                              headPos.z),
                   VRRotation(headRot.x,
                              headRot.y, headRot.z, headRot.w)),
            VRPose(VRPosition(leftControllerPosition.x, leftControllerPosition.y,
                              leftControllerPosition.z),
                   VRRotation(leftControllerRotation.x,
                              leftControllerRotation.y, leftControllerRotation.z, leftControllerRotation.w)),
            VRPose(VRPosition(rightControllerPosition.x, rightControllerPosition.y, rightControllerPosition.z),
                   VRRotation(rightControllerRotation.x, rightControllerRotation.y, rightControllerRotation.z,
                              rightControllerRotation.w)),
            fps, _audioTimeSyncController->songTime));
    }

    vector<VRPoseGroup> PoseRecorder::Export()
    {
        return _vrPoseGroup;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders