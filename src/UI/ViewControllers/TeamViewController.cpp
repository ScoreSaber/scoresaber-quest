#include "UI/ViewControllers/TeamViewController.hpp"
#include "Utils/TeamUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"

#include <HMUI/TextPageScrollView.hpp>
#include <HMUI/Touchable.hpp>

#include <UnityEngine/Application.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Rect.hpp>
#include <UnityEngine/RectOffset.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/UI/LayoutElement.hpp>

#include "logging.hpp"

DEFINE_TYPE(ScoreSaber::UI::ViewControllers, TeamViewController);

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->gameObject->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->gameObject->AddComponent<LayoutElement*>();  \
    layout##identifier->preferredWidth = width;                                          \
    layout##identifier->preferredHeight = height

static std::vector<std::string> teamAndContributorsNamesIdentifiers = {
    "Backend",
    "Frontend",
    "Mod",
    "PPv3",
    "Admin",
    "NAT",
    "RT",
    "QAT",
    "CAT"};

namespace ScoreSaber::UI::ViewControllers
{
    void TeamViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            gameObject->AddComponent<HMUI::Touchable*>();
            auto headerHorizontal = CreateHorizontalLayoutGroup(transform);
            auto headerRectTransform = headerHorizontal->rectTransform;
            headerRectTransform->anchoredPosition = Vector2(0, 45.0f);
            headerRectTransform->sizeDelta = Vector2(-40.0f, 0.0f);
            SetPreferredSize(headerHorizontal, 40.0f, 10.0f);
            headerHorizontal->childAlignment = TextAnchor::MiddleCenter;
            auto headerText = CreateText(headerHorizontal->transform, "Team and Contributors");
            SetPreferredSize(headerText, 40.0f, 10.0f);
            headerText->alignment = TMPro::TextAlignmentOptions::Center;
            headerText->fontSize = 7.0f;
            auto headerBG = headerHorizontal->gameObject->AddComponent<Backgroundable*>();
            headerBG->ApplyBackgroundWithAlpha("round-rect-panel", 0.5f);

            auto segmentedHorizontal = CreateHorizontalLayoutGroup(transform);
            SetPreferredSize(segmentedHorizontal, 80.0f, 6.0f);
            segmentedHorizontal->rectTransform->anchoredPosition = Vector2(0, 35.0f);
            auto segmentedController = BeatSaberUI::CreateTextSegmentedControl(segmentedHorizontal->transform, {0, 0}, {0, 0}, ArrayW<StringW>(il2cpp_array_size_t(0)), std::bind(&TeamViewController::Show, this, std::placeholders::_1));

            segmentedController->overrideCellSize = true;
            segmentedController->fontSize *= 0.75f;

            auto teamAndContributorsNames = ArrayW<StringW>(9);
            teamAndContributorsNames[0] = "BACKEND";
            teamAndContributorsNames[1] = "FRONTEND";
            teamAndContributorsNames[2] = "MOD";
            teamAndContributorsNames[3] = "PPV3";
            teamAndContributorsNames[4] = "ADMIN";
            teamAndContributorsNames[5] = "NAT";
            teamAndContributorsNames[6] = "RANKING TEAM";
            teamAndContributorsNames[7] = "QAT";
            teamAndContributorsNames[8] = "CAT";

            segmentedController->texts = teamAndContributorsNames;

            creditTabs = Array<GameObject*>::NewLength(teamAndContributorsNames.size());

            Show(0);
        }
    }

    void TeamViewController::Show(int idx)
    {
        if (!creditTabs->values[idx])
        {
            // the GO didn't exist yet, make it!
            creditTabs->values[idx] = CreateCreditTab(idx);
        }

        // it exists
        int length = creditTabs->Length();
        for (int i = 0; i < length; i++)
        {
            auto theTab = creditTabs->values[i];
            // disable the ones that are not this one, and enable the one that is this one
            if (theTab)
            {
                theTab->SetActive(i == idx);
            }
        }
    }

    // this method is a fucking mess, but then again, that's just ui
    UnityEngine::GameObject* TeamViewController::CreateCreditTab(int idx)
    {
        auto rootTab = CreateVerticalLayoutGroup(transform);
        rootTab->padding = RectOffset::New_ctor(4, 4, 4, 4);
        rootTab->rectTransform->anchoredPosition = Vector2(0, -10.0f);
        rootTab->rectTransform->sizeDelta = Vector2(0, -5.0f);
        rootTab->childForceExpandWidth = true;
        rootTab->childControlWidth = true;
        auto rootBG = rootTab->gameObject->AddComponent<Backgroundable*>();
        rootBG->ApplyBackgroundWithAlpha("round-rect-panel", 1.0f);

        UnityEngine::GameObject* scrollViewGO = BeatSaberUI::CreateScrollView(rootTab->transform);
        auto externalComponents = scrollViewGO->GetComponent<ExternalComponents*>();
        auto scrollView = externalComponents->Get<HMUI::ScrollView*>();
        SetPreferredSize(scrollView, 115, -1);
        auto viewport = scrollView->viewport;
        viewport->sizeDelta = {0, 0};
        auto scrollRectT = scrollViewGO->GetComponent<RectTransform*>();
        scrollRectT->sizeDelta = {0.0f, 0.0f};
        auto gridParentHorizon = CreateHorizontalLayoutGroup(scrollViewGO->transform);
        gridParentHorizon->childForceExpandWidth = true;
        gridParentHorizon->childControlWidth = true;
        SetPreferredSize(gridParentHorizon, 115, -1);
        auto gridParent = CreateVerticalLayoutGroup(gridParentHorizon->transform);
        SetPreferredSize(gridParent, 115, -1);

        auto grid = CreateGridLayoutGroup(gridParent->transform);
        grid->cellSize = Vector2(30, 15);
        grid->spacing = Vector2(3, 3);
        grid->startAxis = GridLayoutGroup::Axis::Horizontal;
        grid->startCorner = GridLayoutGroup::Corner::UpperLeft;
        auto& members = TeamUtils::get_members(teamAndContributorsNamesIdentifiers[idx]);

        for (auto& mem : members)
        {
            UIUtils::CreateTeamMemberLayout(grid->transform, mem);
        }
        return rootTab->gameObject;
    }
} // namespace ScoreSaber::UI::ViewControllers