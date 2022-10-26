#include "ReplaySystem/UI/ImberSpecsReporter.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberMovementData.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/RectTransformUtility.hpp"
#include "UnityEngine/Vector2.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, ImberSpecsReporter);

namespace ScoreSaber::ReplaySystem::UI
{
    void ImberSpecsReporter::ctor(ScoreSaber::ReplaySystem::Playback::PosePlayer* posePlayer, SaberManager* saberManager)
    {
        INVOKE_CTOR();
        _posePlayer = posePlayer;
        _saberManager = saberManager;
    }
    void ImberSpecsReporter::Initialize()
    {
        _posePlayer->AddCallback([&](ScoreSaber::Data::Private::VRPoseGroup pose) {
            PosePlayer_DidUpdatePose(pose);
        });
    }
    void ImberSpecsReporter::PosePlayer_DidUpdatePose(ScoreSaber::Data::Private::VRPoseGroup pose)
    {
        if (DidReport != nullptr)
        {
            DidReport(pose.FPS, _saberManager->leftSaber->movementData->get_bladeSpeed(), _saberManager->rightSaber->movementData->get_bladeSpeed());
        }
    }
    void ImberSpecsReporter::Dispose()
    {
        _posePlayer->AddCallback(nullptr);
    }
} // namespace ScoreSaber::ReplaySystem::UI