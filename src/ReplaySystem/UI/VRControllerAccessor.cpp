#include "ReplaySystem/UI/VRControllerAccessor.hpp"
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/RectTransformUtility.hpp>
#include <UnityEngine/Vector2.hpp>
#include <UnityEngine/Transform.hpp>
#include "logging.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, VRControllerAccessor);

namespace ScoreSaber::ReplaySystem::UI
{
    void VRControllerAccessor::ctor(GlobalNamespace::PauseMenuManager* pauseMenuManager)
    {
        INVOKE_CTOR();
        _leftController = pauseMenuManager->transform->Find("MenuControllers/ControllerLeft")->GetComponent<GlobalNamespace::VRController*>();
        _rightController = pauseMenuManager->transform->Find("MenuControllers/ControllerRight")->GetComponent<GlobalNamespace::VRController*>();
    }
} // namespace ScoreSaber::ReplaySystem::UI