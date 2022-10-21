#pragma once

#include "bsml/shared/macros.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ViewController.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Pose.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/XR/XRNode.hpp"

#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"

#include "bsml/shared/BSML/Components/TabSelector.hpp"

DECLARE_CLASS_CODEGEN(ScoreSaber::ReplaySystem::UI, MainImberPanelView, HMUI::ViewController,
                      DECLARE_INSTANCE_FIELD(UnityEngine::Pose, defaultPosition);
                      /* Fix this with the actual type & include, the project I made this header in doesn't use qui */
                      DECLARE_PRIVATE_FIELD(QuestUI::FloatingScreen*, _floatingScreen);

                      /* Default field values are only set if you INVOKE_CTOR() in your own ctor */
                      DECLARE_PRIVATE_FIELD_DEFAULT(int, _lastTab, 0);
                      DECLARE_PRIVATE_FIELD_DEFAULT(int, _targetFPS, 90);
                      DECLARE_PRIVATE_FIELD_DEFAULT(int, _initialTime, 1.0f);
                      DECLARE_PRIVATE_FIELD_DEFAULT(UnityEngine::Color, _goodColor, UnityEngine::Color(0, 1, 0, 1.0f));
                      DECLARE_PRIVATE_FIELD_DEFAULT(UnityEngine::Color, _ehColor, UnityEngine::Color(1, 1, 0, 1.0f));
                      DECLARE_PRIVATE_FIELD_DEFAULT(UnityEngine::Color, _noColor, UnityEngine::Color(1, 0, 0, 1.0f));

                      /* Declare BSML property already makes a type _propname; field for you! */
                      DECLARE_INSTANCE_METHOD(UnityEngine::Transform*, get_Transform);
                      DECLARE_INSTANCE_METHOD(void, set_visibility, bool value);
                      DECLARE_INSTANCE_METHOD(void, set_fps, int value);
                      DECLARE_INSTANCE_METHOD(void, set_leftSaberSpeed, float value);
                      DECLARE_INSTANCE_METHOD(void, set_rightSaberSpeed, float value);
                      DECLARE_BSML_PROPERTY(bool, didParse);
                      DECLARE_BSML_PROPERTY(float, timeSync);
                      DECLARE_BSML_PROPERTY(StringW, loopText);
                      DECLARE_BSML_PROPERTY(StringW, playPauseText);
                      DECLARE_BSML_PROPERTY(StringW, location);

                      DECLARE_PRIVATE_FIELD_DEFAULT(List<Il2CppObject*>*, locations, List<Il2CppObject*>::New_ctor());
                      DECLARE_PRIVATE_FIELD(BSML::TabSelector*, tabSelector);
                      DECLARE_PRIVATE_FIELD(HMUI::CurvedTextMeshPro*, fpsText);
                      DECLARE_PRIVATE_FIELD(HMUI::CurvedTextMeshPro*, leftSpeedText);
                      DECLARE_PRIVATE_FIELD(HMUI::CurvedTextMeshPro*, rightSpeedText);

                      /* In DidActivate is when you want to run BSML::parse_and_construct() usually*/
                      DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                      DECLARE_OVERRIDE_METHOD(void, DidDeactivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidDeactivate>::get(), bool removedFromHierarchy, bool screenSystemDisabling);
                      DECLARE_INSTANCE_METHOD(void, Setup, float initialSongTime, int targetFramerate, StringW defaultLocation, ListWrapper<StringW> locations);
                      DECLARE_INSTANCE_METHOD(void, DidSelect, HMUI::SegmentedControl*, int selected);
                      DECLARE_INSTANCE_METHOD(void, SwitchHand, UnityEngine::XR::XRNode xrNode);
                      DECLARE_INSTANCE_METHOD(void, PausePlay);
                      DECLARE_INSTANCE_METHOD(void, Restart);
                      DECLARE_INSTANCE_METHOD(void, Loop);
                      DECLARE_INSTANCE_METHOD(void, SwitchHandLeft);
                      DECLARE_INSTANCE_METHOD(void, SwitchHandRight);
                      DECLARE_INSTANCE_METHOD(void, RequestDismiss);
                      DECLARE_INSTANCE_METHOD(StringW, FormatTimePercent, float value);
                      DECLARE_INSTANCE_METHOD(void, Jump);

                      DECLARE_INJECT_METHOD(void, Construct);
                      /* Use the ctor to set default values like _playPauseText = "PAUSE"; */
                      DECLARE_CTOR(ctor);
                      public
                      : std::function<void(bool)>
                          DidPositionTabVisibilityChange;
                      std::function<void(std::string)> DidPositionPreviewChange;
                      std::function<void(UnityEngine::XR::XRNode)> HandDidSwitchEvent;
                      std::function<void(float)> DidTimeSyncChange;
                      std::function<void(bool)> DidChangeVisiblity;
                      std::function<void(void)> DidClickPausePlay;
                      std::function<void(void)> DidClickRestart;
                      std::function<void(void)> DidPositionJump;
                      std::function<void(void)> DidClickLoop;)