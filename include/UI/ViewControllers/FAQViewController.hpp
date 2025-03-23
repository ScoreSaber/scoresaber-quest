#pragma once
#include <HMUI/ViewController.hpp>
#include <custom-types/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(
    ScoreSaber::UI::ViewControllers, FAQViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate,
                            &HMUI::ViewController::DidActivate,
                            bool firstActivation, bool addedToHierarchy,
                            bool screenSystemEnabling););