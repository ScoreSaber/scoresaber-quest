#pragma once

#include "HMUI/ViewController.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(ScoreSaber::UI::ViewControllers, TeamViewController, HMUI::ViewController,
                      DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                      DECLARE_INSTANCE_FIELD(Array<UnityEngine::GameObject*>*, creditTabs);
                      public
                      : void Show(int idx);
                      UnityEngine::GameObject * CreateCreditTab(int idx);)