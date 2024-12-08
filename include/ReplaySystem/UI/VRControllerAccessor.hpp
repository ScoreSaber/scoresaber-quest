#pragma once

#include <GlobalNamespace/PauseMenuManager.hpp>
#include <GlobalNamespace/VRController.hpp>
#include <custom-types/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(ScoreSaber::ReplaySystem::UI, VRControllerAccessor, Il2CppObject,
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::VRController*, _leftController);
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::VRController*, _rightController);
                      DECLARE_CTOR(ctor, GlobalNamespace::PauseMenuManager*);)