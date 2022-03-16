#include "UI/ViewControllers/TeamViewController.hpp"
#include "Utils/TeamUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"

#include "HMUI/TextPageScrollView.hpp"
#include "HMUI/Touchable.hpp"

#include "UnityEngine/Application.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"

#include "logging.hpp"

DEFINE_TYPE(ScoreSaber::UI::ViewControllers, TeamViewController);

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \
    layout##identifier->set_preferredHeight(height)

static std::vector<std::u16string> teamAndContributorsNames = {
    u"BACKEND",
    u"FRONTEND",
    u"MOD",
    u"PPV3",
    u"ADMIN",
    u"NAT",
    u"RANKING TEAM",
    u"QAT",
    u"CAT"};

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
            get_gameObject()->AddComponent<HMUI::Touchable*>();
            auto headerHorizontal = CreateHorizontalLayoutGroup(get_transform());
            auto headerRectTransform = headerHorizontal->get_rectTransform();
            headerRectTransform->set_anchoredPosition(Vector2(0, 45.0f));
            headerRectTransform->set_sizeDelta(Vector2(-40.0f, 0.0f));
            SetPreferredSize(headerHorizontal, 40.0f, 10.0f);
            headerHorizontal->set_childAlignment(TextAnchor::MiddleCenter);
            auto headerText = CreateText(headerHorizontal->get_transform(), "Team and Contributors");
            SetPreferredSize(headerText, 40.0f, 10.0f);
            headerText->set_alignment(TMPro::TextAlignmentOptions::Center);
            headerText->set_fontSize(7.0f);
            auto headerBG = headerHorizontal->get_gameObject()->AddComponent<Backgroundable*>();
            headerBG->ApplyBackgroundWithAlpha(il2cpp_utils::newcsstr("round-rect-panel"), 0.5f);

            auto segmentedHorizontal = CreateHorizontalLayoutGroup(get_transform());
            SetPreferredSize(segmentedHorizontal, 80.0f, 6.0f);
            segmentedHorizontal->get_rectTransform()->set_anchoredPosition(Vector2(0, 35.0f));
            auto segmentedController = BeatSaberUI::CreateTextSegmentedControl(segmentedHorizontal->get_transform(), {0, 0}, {0, 0}, {}, std::bind(&TeamViewController::Show, this, std::placeholders::_1));

            segmentedController->overrideCellSize = true;
            segmentedController->fontSize *= 0.75f;
            segmentedController->set_texts(teamAndContributorsNames);

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
                theTab->SetActive(i == idx);
        }
    }

    // this method is a fucking mess, but then again, that's just ui
    UnityEngine::GameObject* TeamViewController::CreateCreditTab(int idx)
    {
        auto rootTab = CreateVerticalLayoutGroup(get_transform());
        rootTab->set_padding(RectOffset::New_ctor(4, 4, 4, 4));
        rootTab->get_rectTransform()->set_anchoredPosition(Vector2(0, -10.0f));
        rootTab->get_rectTransform()->set_sizeDelta(Vector2(0, -5.0f));
        rootTab->set_childForceExpandWidth(true);
        rootTab->set_childControlWidth(true);
        auto rootBG = rootTab->get_gameObject()->AddComponent<Backgroundable*>();
        rootBG->ApplyBackgroundWithAlpha(il2cpp_utils::newcsstr("round-rect-panel"), 1.0f);

        UnityEngine::GameObject* scrollViewGO = BeatSaberUI::CreateScrollView(rootTab->get_transform());
        auto externalComponents = scrollViewGO->GetComponent<ExternalComponents*>();
        auto scrollView = externalComponents->Get<HMUI::ScrollView*>();
        SetPreferredSize(scrollView, 115, -1);
        auto viewport = scrollView->dyn__viewport();
        viewport->set_sizeDelta({0, 0});
        auto scrollRectT = scrollViewGO->GetComponent<RectTransform*>();
        scrollRectT->set_sizeDelta({0.0f, 0.0f});
        auto gridParentHorizon = CreateHorizontalLayoutGroup(scrollViewGO->get_transform());
        gridParentHorizon->set_childForceExpandWidth(true);
        gridParentHorizon->set_childControlWidth(true);
        SetPreferredSize(gridParentHorizon, 115, -1);
        auto gridParent = CreateVerticalLayoutGroup(gridParentHorizon->get_transform());
        SetPreferredSize(gridParent, 115, -1);

        auto grid = CreateGridLayoutGroup(gridParent->get_transform());
        grid->set_cellSize(Vector2(30, 15));
        grid->set_spacing(Vector2(3, 3));
        grid->set_startAxis(GridLayoutGroup::Axis::Horizontal);
        grid->set_startCorner(GridLayoutGroup::Corner::UpperLeft);
        auto& members = TeamUtils::get_members(teamAndContributorsNamesIdentifiers[idx]);

        for (auto& mem : members)
        {
            UIUtils::CreateTeamMemberLayout(grid->get_transform(), mem);
        }
        return rootTab->get_gameObject();
    }
}