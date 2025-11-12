#include "UI/ViewControllers/TeamViewController.hpp"
#include "Utils/TeamUtils.hpp"
#include "Utils/UIUtils.hpp"

#include <HMUI/TextPageScrollView.hpp>
#include <HMUI/Touchable.hpp>

#include <System/Collections/Generic/List_1.hpp>
#include <UnityEngine/Application.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Rect.hpp>
#include <UnityEngine/RectOffset.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/UI/LayoutElement.hpp>
#include <bsml/shared/BSML-Lite.hpp>
#include <bsml/shared/BSML/Components/Backgroundable.hpp>
#include <bsml/shared/BSML/Components/ExternalComponents.hpp>

#include "logging.hpp"

DEFINE_TYPE(ScoreSaber::UI::ViewControllers, TeamViewController);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace BSML;
using namespace BSML::Lite;

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
    "CAT",
    "CCT"};

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
            headerBG->ApplyBackground("round-rect-panel");
            headerBG->ApplyAlpha(0.5f);

            auto segmentedHorizontal = CreateHorizontalLayoutGroup(transform);
            SetPreferredSize(segmentedHorizontal, 80.0f, 6.0f);
            segmentedHorizontal->rectTransform->anchoredPosition = Vector2(0, 35.0f);
            auto segmentedController = CreateTextSegmentedControl(segmentedHorizontal->transform, {0, 0}, {0, 0}, {}, std::bind(&TeamViewController::Show, this, std::placeholders::_1));

            segmentedController->_overrideCellSize = true;
            segmentedController->_fontSize *= 0.75f;

            auto teamAndContributorsNames = ListW<StringW>::New();
            teamAndContributorsNames.push_back("BACKEND");
            teamAndContributorsNames.push_back("FRONTEND");
            teamAndContributorsNames.push_back("MOD");
            teamAndContributorsNames.push_back("PPV3");
            teamAndContributorsNames.push_back("ADMIN");
            teamAndContributorsNames.push_back("NAT");
            teamAndContributorsNames.push_back(" RT ");
            teamAndContributorsNames.push_back("QAT");
            teamAndContributorsNames.push_back("CAT");
            teamAndContributorsNames.push_back("CCT");

            segmentedController->SetTexts(teamAndContributorsNames.getPtr()->i___System__Collections__Generic__IReadOnlyList_1_T_(), nullptr);

            creditTabs = Array<UnityW<GameObject>>::NewLength(teamAndContributorsNames.size());

            Show(0);
        }
    }

    void TeamViewController::Show(int idx)
    {
        if (!creditTabs->_values[idx])
        {
            // the GO didn't exist yet, make it!
            creditTabs->_values[idx] = CreateCreditTab(idx);
        }

        // it exists
        int length = creditTabs->get_Length();
        for (int i = 0; i < length; i++)
        {
            auto theTab = creditTabs->_values[i];
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
        rootBG->ApplyBackground("round-rect-panel");
        rootBG->ApplyAlpha(1.0f);

        UnityEngine::GameObject* scrollViewGO = CreateScrollView(rootTab->transform);
        auto externalComponents = scrollViewGO->GetComponent<ExternalComponents*>();
        auto scrollView = externalComponents->Get<HMUI::ScrollView*>();
        SetPreferredSize(scrollView, 115, -1);
        auto viewport = scrollView->_viewport;
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