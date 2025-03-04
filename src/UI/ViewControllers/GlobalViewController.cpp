#include "UI/ViewControllers/GlobalViewController.hpp"

#include "CustomTypes/Components/GlobalLeaderboardTableCell.hpp"
#include "CustomTypes/Components/GlobalLeaderboardTableData.hpp"
#include <HMUI/TableView.hpp>

#include "Sprites.hpp"
#include <UnityEngine/Application.hpp>
#include <UnityEngine/RectOffset.hpp>
#include <UnityEngine/RectTransform.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/UI/LayoutElement.hpp>
#include <UnityEngine/UI/ContentSizeFitter.hpp>
#include <bsml/shared/BSML-Lite.hpp>
#include "Utils/UIUtils.hpp"
#include "Utils/StrippedMethods.hpp"

DEFINE_TYPE(ScoreSaber::UI::ViewControllers, GlobalViewController);

using namespace ScoreSaber::CustomTypes::Components;
using namespace TMPro;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;
using namespace BSML::Lite;

custom_types::Helpers::Coroutine WaitForInit(
    SafePtrUnity<ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData>
        leaderboard,
    std::function<void()> callback)
{
    while (!leaderboard->initialized)
    {
        co_yield nullptr;
    }
    callback();
    co_return;
}

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->gameObject->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->gameObject->AddComponent<LayoutElement*>();  \
    layout##identifier->preferredWidth = width;                                          \
    layout##identifier->preferredHeight = height

namespace ScoreSaber::UI::ViewControllers
{
    void GlobalViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            VerticalLayoutGroup* vertical = CreateVerticalLayoutGroup(transform);
            vertical->spacing = 2.0f;
            vertical->childControlHeight = false;
            vertical->childAlignment = TextAnchor::MiddleCenter;

            ContentSizeFitter* verticalFitter = vertical->GetComponent<ContentSizeFitter*>();
            verticalFitter->horizontalFit = ContentSizeFitter::FitMode::PreferredSize;
            verticalFitter->verticalFit = ContentSizeFitter::FitMode::PreferredSize;

            LayoutElement* verticalElement = vertical->GetComponent<LayoutElement*>();
            verticalElement->preferredWidth = 120.0f;

            auto headerHorizontal = CreateHorizontalLayoutGroup(vertical->transform);
            SetPreferredSize(headerHorizontal, 120.0f, -1);
            headerHorizontal->childControlHeight = true;
            auto textHorizontal = CreateHorizontalLayoutGroup(headerHorizontal->transform);
            textHorizontal->childForceExpandWidth = false;
            textHorizontal->childAlignment = TextAnchor::MiddleCenter;
            SetPreferredSize(textHorizontal, 80.0f, -1);
            textHorizontal->rectTransform->sizeDelta = Vector2(-40.0f, 0.0f);
            auto headerText = CreateClickableText(textHorizontal->transform, u"Global Leaderboards", Vector2(0.0f, 0.0f), Vector2(0.0f, 0.0f), []() { StrippedMethods::UnityEngine::Application::OpenURL("https://scoresaber.com/rankings"); });

            headerText->alignment = TMPro::TextAlignmentOptions::Center;
            headerText->fontSize = 7.0f;

            auto infoButton = CreateUIButton(headerHorizontal->transform, " ?", Vector2(114.0f, -5.0f), Vector2(10.0f, 8.5f), std::bind(&GlobalViewController::OpenMoreInfoModal, this));

            auto layoutinfoButton = infoButton->GetComponent<LayoutElement*>();
            layoutinfoButton->ignoreLayout = true;
            auto textObject = infoButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
            textObject->alignment = TMPro::TextAlignmentOptions::Left;

            auto headerBG = headerHorizontal->gameObject->AddComponent<Backgroundable*>();
            headerBG->ApplyBackground("round-rect-panel");
            headerBG->ApplyAlpha(0.5f);
            auto headerImageView = headerBG->gameObject->GetComponentInChildren<HMUI::ImageView*>();
            headerImageView->_skew = 0.18f;

            HorizontalLayoutGroup* globalHost = CreateHorizontalLayoutGroup(vertical->transform);
            globalHost->spacing = 1.0f;

            ContentSizeFitter* globalHostFitter = globalHost->GetComponent<ContentSizeFitter*>();
            globalHostFitter->horizontalFit = ContentSizeFitter::FitMode::PreferredSize;
            globalHostFitter->verticalFit = ContentSizeFitter::FitMode::PreferredSize;

            LayoutElement* globalHostElement = globalHost->GetComponent<LayoutElement*>();
            globalHostElement->preferredWidth = 120.0f;

            VerticalLayoutGroup* scoreScopesHost = CreateVerticalLayoutGroup(globalHost->transform);

            LayoutElement* scoreScopesHostElement = scoreScopesHost->GetComponent<LayoutElement*>();
            scoreScopesHostElement->preferredWidth = 9.0f;
            
            auto _pageUpButton = CreateUIButton(scoreScopesHost->transform, "", "SettingsButton", Vector2(0.0f, 25.0f), Vector2(9.0f, 9.0f), std::bind(&GlobalViewController::UpButtonWasPressed, this));
            SetButtonSprites(_pageUpButton, Base64ToSprite(carat_up_inactive_base64), Base64ToSprite(carat_up_base64));
            auto upRectTransform = _pageUpButton->transform->GetChild(0).cast<RectTransform>();
            upRectTransform->sizeDelta = {9.0f, 9.0f};

            VerticalLayoutGroup* scoreScopes = CreateVerticalLayoutGroup(scoreScopesHost->transform);

            RectOffset* scoreScopesPad = scoreScopes->padding;
            scoreScopesPad->bottom = 1;
            scoreScopesPad->left = 1;
            scoreScopesPad->right = 1;
            scoreScopesPad->top = 1;

            LayoutElement* scoreScopesElement = scoreScopes->GetComponent<LayoutElement*>();
            scoreScopesElement->preferredWidth = 9.0f;
            scoreScopesElement->preferredHeight = 40.0f;

            Backgroundable* background = scoreScopes->gameObject->AddComponent<Backgroundable*>();
            background->ApplyBackground("round-rect-panel");
            background->ApplyAlpha(1.0f);

            VerticalLayoutGroup* imagesGroup = CreateVerticalLayoutGroup(scoreScopes->transform);

            LayoutElement* imagesGroupElement = imagesGroup->GetComponent<LayoutElement*>();
            imagesGroupElement->preferredWidth = 4.0f;
            imagesGroupElement->preferredHeight = 20.0f;
            imagesGroup->spacing = 2;
            auto countrySprite = Base64ToSprite(country_base64);
            auto globalSprite = Base64ToSprite(global_base64);
            auto playerSprite = Base64ToSprite(player_base64);
            auto friendsSprite = Base64ToSprite(friends_base64);

            auto globalButton = CreateClickableImage(imagesGroup->transform,
                                                     globalSprite,
                                                     std::bind(&GlobalViewController::FilterWasClicked, this, ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType::Global),
                                                     {0.0f, 0.0f},
                                                     {4.0f, 4.0f});
            globalButton->preserveAspect = true;
            auto aroundYouButton = CreateClickableImage(imagesGroup->transform,
                                                        playerSprite, std::bind(&GlobalViewController::FilterWasClicked, this, ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType::AroundYou),
                                                        {0.0f, 0.0f},
                                                        {4.0f, 4.0f});
            aroundYouButton->preserveAspect = true;
            auto friendsButton = CreateClickableImage(imagesGroup->transform,
                                                      friendsSprite,
                                                      std::bind(&GlobalViewController::FilterWasClicked, this, ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType::Friends),
                                                      {0.0f, 0.0f},
                                                      {4.0f, 4.0f});
            friendsButton->preserveAspect = true;
            auto countryButton = CreateClickableImage(imagesGroup->transform,
                                                      countrySprite,
                                                      std::bind(&GlobalViewController::FilterWasClicked, this, ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType::Country),
                                                      {0.0f, 0.0f},
                                                      {4.0f, 4.0f});
            countryButton->preserveAspect = true;

            auto _pageDownButton = CreateUIButton(scoreScopesHost->transform, "", "SettingsButton", Vector2(0.0f, -25.0f), Vector2(9.0f, 9.0f), std::bind(&GlobalViewController::DownButtonWasPressed, this));
            SetButtonSprites(_pageDownButton, Base64ToSprite(carat_down_inactive_base64), Base64ToSprite(carat_down_base64));
            auto downRectTransform = _pageDownButton->transform->GetChild(0).cast<RectTransform>();
            downRectTransform->sizeDelta = {9.0f, 9.0f};

            VerticalLayoutGroup* globalVerticalHost = CreateVerticalLayoutGroup(globalHost->transform);

            VerticalLayoutGroup* playersHost = CreateVerticalLayoutGroup(globalVerticalHost->transform);

            auto loadingVertical = CreateVerticalLayoutGroup(transform);
            SetPreferredSize(loadingVertical, 10, 10);
            layoutloadingVertical->ignoreLayout = true;
            loadingVertical->rectTransform->anchoredPosition = playersHost->rectTransform->anchoredPosition;
            auto loadingHorizontal = CreateHorizontalLayoutGroup(loadingVertical->transform);
            SetPreferredSize(loadingHorizontal, 10, 10);

            loadingIndicator = UIUtils::CreateLoadingIndicator(loadingHorizontal->transform);
            auto loadingIndicatorLayout = loadingIndicator->GetComponent<LayoutElement*>();
            auto loadingIndicatorRectTransform = loadingIndicator->transform.cast<RectTransform>();

            playersHost->padding = RectOffset::New_ctor(2, 2, 2, 2);

            Backgroundable* playersHostBg = playersHost->gameObject->AddComponent<Backgroundable*>();
            playersHostBg->ApplyBackground("round-rect-panel");
            playersHostBg->ApplyAlpha(1.0f);

            LayoutElement* playersHostElement = playersHost->GetComponent<LayoutElement*>();
            playersHostElement->preferredWidth = 105.0f;
            playersHostElement->preferredHeight = 60.0f;

            auto sizeDelta = Vector2(105.0f, 60.0f);
            auto anchoredPosition = Vector2(0.0f, 0.0f);
            playersHost->rectTransform->sizeDelta = sizeDelta;
            playersHost->rectTransform->anchoredPosition = anchoredPosition;
            playersHost->childForceExpandHeight = false;
            playersHost->childControlHeight = false;
            playersHost->childScaleHeight = false;

            leaderboardList = CreateCustomSourceList<ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData*>(playersHost->transform, Vector2(sizeDelta.x, sizeDelta.y), nullptr);
            leaderboardList->globalViewController = this;
            leaderboardList->StartRefresh();

            playerProfileModal = ::ScoreSaber::UI::Other::PlayerProfileModal::Create(transform);
            leaderboardList->playerProfileModal = playerProfileModal;
        }
    }

    void GlobalViewController::set_loading(bool value)
    {
        loadingIndicator->SetActive(value);
    }

    void GlobalViewController::UpButtonWasPressed()
    {
        leaderboardList->UpButtonWasPressed();
    }

    void GlobalViewController::DownButtonWasPressed()
    {
        leaderboardList->DownButtonWasPressed();
    }

    void GlobalViewController::OpenMoreInfoModal()
    {
        if (!moreInfoModal)
        {
            // make it
            moreInfoModal = CreateModal(transform, Vector2(84.0f, 64.0f), nullptr, true);
            auto textVertical = CreateVerticalLayoutGroup(moreInfoModal->transform);
            textVertical->spacing = 1.0f;
            textVertical->padding = RectOffset::New_ctor(2, 2, 2, 2);
            auto h1 = CreateText(textVertical->transform, "What is <color=#6772E5>PP</color>?", FontStyles::Normal);
            h1->alignment = TMPro::TextAlignmentOptions::TopLeft;
            h1->fontSize = 4.0f;
            auto t1 = CreateText(textVertical->transform, "Performance Points (<color=#6772E5>PP</color>) are earned through playing ranked maps. Harder maps and higher\n"
                                                                "scores will increase the amount of <color=#6772E5>PP</color> you receive from beating a map. Performance Points\n"
                                                                "determine your rank on the ScoreSaber global leaderboards, the more <color=#6772E5>PP</color> you have the higher\n"
                                                                "your rank will be.",
                                                                FontStyles::Normal);
            t1->alignment = TMPro::TextAlignmentOptions::TopLeft;
            t1->fontSize = 2.5f;
            t1->enableWordWrapping = true;
            t1->rectTransform->sizeDelta = {0, 0};
            Object::Destroy(t1->GetComponent<LayoutElement*>());
            auto h2 = CreateText(textVertical->transform, "How Does Ranking Work?", FontStyles::Normal);
            h2->alignment = TMPro::TextAlignmentOptions::TopLeft;
            h2->fontSize = 4.0f;
            auto t2 = CreateText(textVertical->transform, "Ranked maps are maps passed through a ranking system that judges maps to make sure they\n"
                                                                "are objectively fair. The Ranking Team votes based on the Ranking Criteria, a document that\n"
                                                                "goes over the rules a map must follow to be ranked. The Quality Assurance Team votes to\n"
                                                                "either speed up or slow down maps in queue based on whether the map meets their quality\n"
                                                                "standards, however they cannot fully block a map that meets the criteria from being ranked.",
                                                                FontStyles::Normal);
            t2->alignment = TMPro::TextAlignmentOptions::TopLeft;
            t2->fontSize = 2.5f;
            t2->enableWordWrapping = true;
            t2->rectTransform->sizeDelta = {0, 0};
            Object::Destroy(t2->GetComponent<LayoutElement*>());

            auto buttonHorizontal = CreateHorizontalLayoutGroup(textVertical->transform);
            auto dismiss = CreateUIButton(buttonHorizontal->transform, "Dismiss", [&]() { moreInfoModal->Hide(true, nullptr); });

            auto moreInfo = CreateUIButton(buttonHorizontal->transform, "More Info", []() { StrippedMethods::UnityEngine::Application::OpenURL("https://wiki.scoresaber.com/ranking-system.html"); });
        }
        moreInfoModal->Show(true, true, nullptr);
    }

    void GlobalViewController::FilterWasClicked(ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType type)
    {
        leaderboardList->set_LeaderboardType(type);
    }
} // namespace ScoreSaber::UI::ViewControllers
