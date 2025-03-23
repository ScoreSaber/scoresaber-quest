#pragma once

#include <HMUI/FlowCoordinator.hpp>
#include <HMUI/ViewController.hpp>
#include "UI/ViewControllers/MainSettingsViewController.hpp"
#include <UnityEngine/GameObject.hpp>
#include <custom-types/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(ScoreSaber::UI::FlowCoordinators, ScoreSaberSettingsFlowCoordinator, HMUI::FlowCoordinator,
                      DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::FlowCoordinator::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                      DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed, &HMUI::FlowCoordinator::BackButtonWasPressed, HMUI::ViewController* topViewController);

                      DECLARE_INSTANCE_FIELD_DEFAULT(UnityW<ScoreSaber::UI::ViewControllers::MainSettingsViewController>, mainSettingsViewController, nullptr);)
