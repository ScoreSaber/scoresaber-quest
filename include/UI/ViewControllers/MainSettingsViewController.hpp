#pragma once

#include <HMUI/ViewController.hpp>

#include <beatsaber-hook/shared/utils/typedefs.h>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(ScoreSaber::UI::ViewControllers, MainSettingsViewController, HMUI::ViewController) {
                      DECLARE_INSTANCE_METHOD(bool, get_showScorePP);
                      DECLARE_INSTANCE_METHOD(void, set_showScorePP, bool value);
                      DECLARE_INSTANCE_METHOD(bool, get_showLocalPlayerRank);
                      DECLARE_INSTANCE_METHOD(void, set_showLocalPlayerRank, bool value);
                      DECLARE_INSTANCE_METHOD(bool, get_hideNAScores);
                      DECLARE_INSTANCE_METHOD(void, set_hideNAScores, bool value);
                      DECLARE_INSTANCE_METHOD(StringW, get_locationFilterMode);
                      DECLARE_INSTANCE_METHOD(void, set_locationFilterMode, StringW value);
                      DECLARE_INSTANCE_METHOD(bool, get_enableCountryLeaderboards);
                      DECLARE_INSTANCE_METHOD(void, set_enableCountryLeaderboards, bool value);
                      DECLARE_INSTANCE_METHOD(bool, get_saveLocalReplays);
                      DECLARE_INSTANCE_METHOD(void, set_saveLocalReplays, bool value);
                      
                      DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(ListW<StringW>, locationFilerOptions, ListW<StringW>::New());

                      DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

                      DECLARE_CTOR(ctor);
};