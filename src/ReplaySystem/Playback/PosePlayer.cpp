// #include "ReplaySystem/Playback/PosePlayer.hpp"
// #include "GlobalNamespace/Saber.hpp"
// #include "ReplaySystem/ReplayLoader.hpp"
// #include "UnityEngine/Mathf.hpp"
// #include "UnityEngine/Quaternion.hpp"

// #include "UnityEngine/Vector3.hpp"
// // GlobalNamespace::SaberManager
// #include "GlobalNamespace/SaberManager.hpp"
// #include "logging.hpp"

// using namespace UnityEngine;
// using namespace ScoreSaber::Data::Private;

// DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, PosePlayer);

// namespace ScoreSaber::ReplaySystem::Playback
// {
//     void PosePlayer::ctor(GlobalNamespace::MainCamera* mainCamera, GlobalNamespace::SaberManager* saberManager, GlobalNamespace::IReturnToMenuController* returnToMenuController, GlobalNamespace::PlayerTransforms* playerTransforms, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController)
//     {
//         INFO("1");
//         INVOKE_CTOR();
//         INFO("1");
//         _sortedPoses = ReplayLoader::LoadedReplay->poseKeyframes;
//         INFO("2");
//         _mainCamera = mainCamera;
//         INFO("3");
//         _saberManager = saberManager;
//         INFO("4");
//         _returnToMenuController = returnToMenuController;
//         INFO("5");
//         _playerTransforms = playerTransforms;
//         INFO("6");
//         _audioTimeSyncController = audioTimeSyncController;
//         INFO("7");
//         INFO("%d", _sortedPoses.size());
//         INFO("%p", _mainCamera);
//         INFO("%p", _saberManager);
//         INFO("%p", _returnToMenuController);
//         INFO("%p", _playerTransforms);
//         INFO("%p", _audioTimeSyncController);
//     }

//     void PosePlayer::Initialize()
//     {
//     }
//     void PosePlayer::Tick()
//     {
//         if (ReachedEnd())
//         {
//             _returnToMenuController->ReturnToMenu();
//             return;
//         }
//         bool foundPoseThisFrame = false;
//         while (_audioTimeSyncController->songTime >= _sortedPoses[_lastIndex].Time)
//         {
//             foundPoseThisFrame = true;
//             VRPoseGroup activePose = _sortedPoses[_lastIndex++];
//             if (ReachedEnd())
//             {
//                 return;
//             }

//             VRPoseGroup nextPose = _sortedPoses[_lastIndex + 1];
//             UpdatePoses(activePose, nextPose);
//         }
//         if (foundPoseThisFrame)
//         {
//             return;
//         }
//         else if (_lastIndex > 0 && !ReachedEnd())
//         {
//             VRPoseGroup activePose = _sortedPoses[_lastIndex - 1];
//             VRPoseGroup nextPose = _sortedPoses[_lastIndex];
//             UpdatePoses(activePose, nextPose);
//         }
//     }
//     void PosePlayer::Dispose()
//     {
//     }
//     void PosePlayer::UpdatePoses(Data::Private::VRPoseGroup activePose, Data::Private::VRPoseGroup nextPose)
//     {
//         float lerpTime = (_audioTimeSyncController->get_songTime() - activePose.Time) / Mathf::Max(0.0001f, nextPose.Time - activePose.Time);

//         _saberManager->leftSaber->OverridePositionAndRotation(Vector3::Lerp(VRVector3(activePose.Left.Position), VRVector3(nextPose.Left.Position), lerpTime),
//                                                               Quaternion::Lerp(VRQuaternion(activePose.Left.Rotation), VRQuaternion(nextPose.Left.Rotation), lerpTime));

//         _saberManager->rightSaber->OverridePositionAndRotation(Vector3::Lerp(VRVector3(activePose.Right.Position), VRVector3(nextPose.Right.Position), lerpTime),
//                                                                Quaternion::Lerp(VRQuaternion(activePose.Right.Rotation), VRQuaternion(nextPose.Right.Rotation), lerpTime));
//         // TODO: Move head
//     }

//     bool PosePlayer::ReachedEnd()
//     {
//         return _lastIndex >= _sortedPoses.size() - 1;
//     }

//     void PosePlayer::TimeUpdate(float newTime)
//     {
//         for (int c = 0; c < _sortedPoses.size(); c++)
//         {
//             if (_sortedPoses[c].Time > newTime)
//             {
//                 _lastIndex = c;
//                 Tick();
//                 return;
//             }
//         }
//     }
// } // namespace ScoreSaber::ReplaySystem::Playback