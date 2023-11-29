#pragma once

#include "HMUI/ViewController.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(ScoreSaber::UI::ViewControllers, MainSettingsViewController, HMUI::ViewController,
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
                      
                      DECLARE_PRIVATE_FIELD_DEFAULT(List<Il2CppObject*>*, locationFilerOptions, List<Il2CppObject*>::New_ctor());

                      DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

                      DECLARE_CTOR(ctor);
)
