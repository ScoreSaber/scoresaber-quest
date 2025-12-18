#pragma once


#include <custom-types/shared/macros.hpp>
#include <bsml/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

#include <HMUI/CurvedTextMeshPro.hpp>
#include <HMUI/ViewController.hpp>
#include <UnityEngine/Color.hpp>
#include <UnityEngine/Pose.hpp>
#include <UnityEngine/Transform.hpp>
#include <UnityEngine/XR/XRNode.hpp>
#include <beatsaber-hook/shared/utils/typedefs.h>
#include <System/Collections/Generic/List_1.hpp>

#include <bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp>
#include <bsml/shared/BSML/Components/Backgroundable.hpp>
#include <bsml/shared/BSML/Components/TabSelector.hpp>
#include "Utils/DelegateUtils.hpp"

DECLARE_CLASS_CODEGEN(ScoreSaber::ReplaySystem::UI, MainImberPanelView, HMUI::ViewController) {
    DECLARE_INSTANCE_FIELD(UnityEngine::Pose, defaultPosition);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<BSML::FloatingScreen>, _floatingScreen);

    /* Default field values are only set if you INVOKE_CTOR() in your own ctor */
    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(int, _lastTab, 0);
    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(int, _targetFPS, 90);
    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(int, _initialTime, 1.0f);
    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(UnityEngine::Color, _goodColor, UnityEngine::Color(0, 1, 0, 1.0f));
    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(UnityEngine::Color, _ehColor, UnityEngine::Color(1, 1, 0, 1.0f));
    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(UnityEngine::Color, _noColor, UnityEngine::Color(1, 0, 0, 1.0f));

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

    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(ListW<StringW>, locations, ListW<StringW>::New());
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<BSML::TabSelector>, tabSelector);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<HMUI::CurvedTextMeshPro>, fpsText);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<HMUI::CurvedTextMeshPro>, leftSpeedText);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<HMUI::CurvedTextMeshPro>, rightSpeedText);

    /* In DidActivate is when you want to run BSML::parse_and_construct() usually*/
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidDeactivate, &HMUI::ViewController::DidDeactivate, bool removedFromHierarchy, bool screenSystemDisabling);
    DECLARE_INSTANCE_METHOD(void, DidSelect, UnityW<HMUI::SegmentedControl>, int selected);
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
private:
    DelegateUtils::DelegateW<System::Action_2<UnityW<HMUI::SegmentedControl>, int>> didSelectDelegate;
public: 
    std::function<void(bool)> DidPositionTabVisibilityChange;
    std::function<void(std::string)> DidPositionPreviewChange;
    std::function<void(UnityEngine::XR::XRNode)> HandDidSwitchEvent;
    std::function<void(float)> DidTimeSyncChange;
    std::function<void(bool)> DidChangeVisiblity;
    std::function<void(void)> DidClickPausePlay;
    std::function<void(void)> DidClickRestart;
    std::function<void(void)> DidPositionJump;
    std::function<void(void)> DidClickLoop;
    void Setup(float initialSongTime, int targetFramerate, std::string defaultLocation, std::vector<std::string> _locations);
};