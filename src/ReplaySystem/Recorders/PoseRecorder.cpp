#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
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

namespace ScoreSaber::ReplaySystem::Recorders::PoseRecorder
{
    SaberManager* _saberManager;
    VRController* _controllerLeft;
    VRController* _controllerRight;
    AudioTimeSyncController* _audioTimeSyncController;

    vector<VRPoseGroup> _vrPoseGroup;

    void LevelStarted(SaberManager* saberManager, AudioTimeSyncController* audioTimeSyncController)
    {
        _vrPoseGroup.empty();
        _audioTimeSyncController = audioTimeSyncController;
        _controllerLeft = saberManager->leftSaber->GetComponentInParent<VRController*>();
        _controllerRight = saberManager->rightSaber->GetComponentInParent<VRController*>();
    }

    void Tick(Vector3 headPos, Quaternion headRot)
    {
        int fps = (int)(1 / Time::get_unscaledDeltaTime());
        _vrPoseGroup.push_back(VRPoseGroup(
            VRPose(VRPosition(headPos.x, headPos.y,
                              headPos.z),
                   VRRotation(headRot.x,
                              headRot.y, headRot.z, headRot.w)),
            VRPose(VRPosition(_controllerLeft->get_position().x, _controllerLeft->get_position().y,
                              _controllerLeft->get_position().z),
                   VRRotation(_controllerLeft->get_rotation().x,
                              _controllerLeft->get_rotation().y, _controllerLeft->get_rotation().z, _controllerLeft->get_rotation().w)),
            VRPose(VRPosition(_controllerRight->get_position().x, _controllerRight->get_position().y, _controllerRight->get_position().z),
                   VRRotation(_controllerRight->get_rotation().x, _controllerRight->get_rotation().y, _controllerRight->get_rotation().z,
                              _controllerRight->get_rotation().w)),
            fps, _audioTimeSyncController->songTime));
    }

    vector<VRPoseGroup> Export()
    {
        return _vrPoseGroup;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders::PoseRecorder