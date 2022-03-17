#pragma once
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Vector3.hpp"

using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;
using namespace UnityEngine;

namespace ScoreSaber::ReplaySystem::Recorders::PoseRecorder
{
    void LevelStarted(SaberManager* saberManager, AudioTimeSyncController* audioTimeSyncController);
    void Tick(Vector3 headPos, Quaternion headRot);
    vector<VRPoseGroup> Export();
} // namespace ScoreSaber::ReplaySystem::Recorders::PoseRecorder