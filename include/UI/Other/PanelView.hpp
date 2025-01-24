#pragma once

#include <GlobalNamespace/LoadingControl.hpp>
#include <GlobalNamespace/PlatformLeaderboardViewController.hpp>
#include <HMUI/CurvedTextMeshPro.hpp>
#include <HMUI/ModalView.hpp>
#include <HMUI/Screen.hpp>
#include <HMUI/ViewController.hpp>
#include <TMPro/TextMeshProUGUI.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/UI/Button.hpp>
#include <UnityEngine/UI/HorizontalLayoutGroup.hpp>
#include <custom-types/shared/coroutine.hpp>
#include <custom-types/shared/macros.hpp>
#include <custom-types/shared/register.hpp>
#include <custom-types/shared/types.hpp>
#include <custom-types/shared/util.hpp>

using namespace UnityEngine;
using namespace GlobalNamespace;

DECLARE_CLASS_CODEGEN(
    ScoreSaber::UI::Other, PanelView, HMUI::ViewController,
    DECLARE_INSTANCE_METHOD(void, Init, PlatformLeaderboardViewController*);
    DECLARE_INSTANCE_METHOD(void, Show);
    DECLARE_INSTANCE_FIELD(PlatformLeaderboardViewController*, leaderboardView);
    DECLARE_INSTANCE_FIELD(HMUI::Screen*, floatingScreen);
    DECLARE_INSTANCE_FIELD(LoadingControl*, loadingControl);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, promptText);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::HorizontalLayoutGroup*, promptRoot);
    public
    : custom_types::Helpers::Coroutine SetPrompt(
        std::string status, bool loadingIndicator, float dismiss,
        std::function<void()> callback);)