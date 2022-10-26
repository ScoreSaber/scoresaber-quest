#include "ReplaySystem/UI/VRControllerAccessor.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/RectTransformUtility.hpp"
#include "UnityEngine/Vector2.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, VRControllerAccessor);

namespace ScoreSaber::ReplaySystem::UI
{
    void VRControllerAccessor::ctor(GlobalNamespace::PauseMenuManager* pauseMenuManager)
    {
        INVOKE_CTOR();
        _leftController = pauseMenuManager->get_transform()->Find("MenuControllers/ControllerLeft")->GetComponent<GlobalNamespace::VRController*>();
        _rightController = pauseMenuManager->get_transform()->Find("MenuControllers/ControllerRight")->GetComponent<GlobalNamespace::VRController*>();
    }
} // namespace ScoreSaber::ReplaySystem::UI