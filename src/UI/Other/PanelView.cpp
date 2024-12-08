#include "UI/Other/PanelView.hpp"

#include "Data/Private/Settings.hpp"

#include "CustomTypes/Components/ImageButton.hpp"
#include <GlobalNamespace/SharedCoroutineStarter.hpp>
#include <HMUI/CurvedCanvasSettingsHelper.hpp>
#include <HMUI/ViewController_AnimationDirection.hpp>
#include <HMUI/ViewController_AnimationType.hpp>
#include <System/Collections/IEnumerator.hpp>
#include <TMPro/TextMeshProUGUI.hpp>
#include "UI/FlowCoordinators/ScoreSaberFlowCoordinator.hpp"
#include <UnityEngine/Events/UnityAction.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/Transform.hpp>
#include <UnityEngine/UI/Button.hpp>
#include <UnityEngine/UI/VerticalLayoutGroup.hpp>
#include <UnityEngine/WaitForSeconds.hpp>
#include "Utils/StringUtils.hpp"
#include "Utils/UIUtils.hpp"
#include <beatsaber-hook/shared/utils/il2cpp-functions.hpp>
#include "main.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

DEFINE_TYPE(ScoreSaber::UI::Other, PanelView);

using namespace ScoreSaber::UI::Other;
using namespace ScoreSaber::CustomTypes::Components;
using namespace ScoreSaber::UI::FlowCoordinators;
using namespace ScoreSaber::Data::Private;
using namespace QuestUI;
using namespace UnityEngine::UI;
using namespace UnityEngine::Events;
using namespace GlobalNamespace;

#define BeginCoroutine(method) \
    BSML::Helpers::GetDiContainer()->Resolve<GlobalNamespace::ICoroutineStarter*>()->StartCoroutine( \
        custom_types::Helpers::CoroutineHelper::New(method));

void PanelView::Init(PlatformLeaderboardViewController* viewController)
{
    this->leaderboardView = viewController;

    GameObject* canvas = BeatSaberUI::CreateCanvas();

    HMUI::Screen* screen = canvas->AddComponent<HMUI::Screen*>();

    Transform* transform = screen->transform;
    RectTransform* rectTransform = screen->GetComponent<RectTransform*>();

    rectTransform->sizeDelta = {100.0f, 25.0f};
    transform->SetParent(leaderboardView->transform, false);
    transform->localPosition = {0.0f, 50.0f, 0.0f};
    transform->eulerAngles = Quaternion::identity.eulerAngles;
    transform->localScale = Vector3::one;

    this->floatingScreen = screen;

    promptRoot = BeatSaberUI::CreateHorizontalLayoutGroup(screen->transform);
    promptRoot->childAlignment = TextAnchor::UpperLeft;
    promptRoot->childForceExpandWidth = false;
    promptRoot->spacing = 1.0f;

    RectTransform* promptRootRect = promptRoot->rectTransform;
    promptRootRect->anchoredPosition = {0.0f, 10.3f};

    LayoutElement* promptElement = promptRoot->GetComponent<LayoutElement*>();
    promptElement->preferredHeight = 7.0f;
    promptElement->preferredWidth = 87.0f;

    ContentSizeFitter* promptFitter =
        promptRoot->GetComponent<ContentSizeFitter*>();
    promptFitter->horizontalFit = ContentSizeFitter::FitMode::PreferredSize;

    HorizontalLayoutGroup* textGroup =
        BeatSaberUI::CreateHorizontalLayoutGroup(promptRootRect);
    textGroup->rectTransform->anchoredPosition = {0.0f, 10.0f};

    this->promptText =
        BeatSaberUI::CreateText(textGroup->transform, "...", false);
    this->promptText->alignment = TMPro::TextAlignmentOptions::BottomLeft;

    HorizontalLayoutGroup* indicatorGroup =
        BeatSaberUI::CreateHorizontalLayoutGroup(promptRootRect);

    LayoutElement* indicatorElement =
        indicatorGroup->GetComponent<LayoutElement*>();
    indicatorElement->preferredHeight = 4.0f;
    indicatorElement->preferredWidth = 4.0f;
}

void PanelView::Show()
{
    this->floatingScreen->SetRootViewController(leaderboardView, 1);

    std::string iconPath =
        "/sdcard/ModData/com.beatgames.beatsaber/"
        "Mods/ScoreSaber/Icons/";

    VerticalLayoutGroup* vertical =
        QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(
            floatingScreen->transform);
    vertical->rectTransform->anchoredPosition = {0.0f, 0.0f};
    HorizontalLayoutGroup* horizontal =
        QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(
            vertical->transform);

    TMPro::TextMeshProUGUI* text =
        QuestUI::BeatSaberUI::CreateText(horizontal->transform, "");
    text->fontSize = text->fontSize * 2.0f;
    text->alignment = TMPro::TextAlignmentOptions::Center;

    LayoutElement* layoutelem =
        text->gameObject->AddComponent<LayoutElement*>();
    layoutelem->preferredHeight = 15.0f;
    layoutelem->preferredWidth = 90.0f;

    Backgroundable* background =
        horizontal->gameObject->AddComponent<Backgroundable*>();
    background->ApplyBackgroundWithAlpha("title-gradient",
                                         1.0f);

    HMUI::ImageView* imageView =
        background->gameObject->GetComponentInChildren<HMUI::ImageView*>();
    imageView->gradient = true;
    imageView->gradientDirection = 0;
    imageView->color = Color::white;
    imageView->color0 = Color(0.0f, 0.45f, 0.65f, 1.0f);
    imageView->color1 = Color(0.0f, 0.45f, 0.65f, 0.0f);
    imageView->curvedCanvasSettingsHelper->Reset();

    Sprite* sprite = BeatSaberUI::FileToSprite(iconPath + "scoresaber.png");

    ImageButton* menu = UIUtils::CreateImageButton(
        floatingScreen->gameObject, sprite, {-37.0f, 0.0f}, {12.0f, 12.0f},
        []() {
            HMUI::FlowCoordinator* currentFlowCoordinator =
                QuestUI::BeatSaberUI::GetMainFlowCoordinator()
                    ->YoungestChildFlowCoordinatorOrSelf();
            auto flowCoordinator = ArrayUtil::First(Resources::FindObjectsOfTypeAll<ScoreSaber::UI::FlowCoordinators::ScoreSaberFlowCoordinator*>());
            if (!flowCoordinator)
            {
                flowCoordinator =
                    BeatSaberUI::CreateFlowCoordinator<ScoreSaberFlowCoordinator*>();
            }

            currentFlowCoordinator->PresentFlowCoordinator(
                flowCoordinator, nullptr,
                HMUI::ViewController::AnimationDirection::Horizontal,
                HMUI::ViewController::AnimationType::In, false);
        });
    /*
  for (int i = 0; i < 13; i++) {
    QuestUI::BeatSaberUI::CreateImage(
        leaderboardView->transform,
        "<color=#ffde1c>Global Ranking: "
        "</color>#893 "
        "(<size=80%><color=#6771e5>11,141.92pp</color></size>)",
        true, Vector2(5.0f, 50.0f));
    ScoreSaber::ScoreSaber::leaderboard->ranked =
        QuestUI::BeatSaberUI::CreateText(
            leaderboardView->transform,
            "<color=#ffde1c>Ranked Status: </color>Ranked (modifiers "
            "disabled)",
            true, Vector2(5.0f, 45.0f));
*/
    BeginCoroutine(SetPrompt("Connecting to ScoreSaber", true, 5.0f, [=]() { BeginCoroutine(SetPrompt(
                                                                                 StringUtils::Colorize("Successfully Connected to ScoreSaber", "green"),
                                                                                 false, 2.0f, nullptr)); }));
}

custom_types::Helpers::Coroutine PanelView::SetPrompt(
    std::string status, bool showIndicator, float dismiss,
    std::function<void()> callback)
{
    if (!Settings::showStatusText) {
        co_return;
    }

    this->promptText->SetText(status);

    std::string text = status;

    for (int i = 1; i < (dismiss * 2) + 1; i++)
    {
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(
            CRASH_UNLESS(WaitForSeconds::New_ctor(0.5f)));

        // Couldn't get the loading indicator to work so right now it just displays
        // dots as the loading indicator
        if (showIndicator)
        {
            if (i % 4 != 0)
            {
                text = text + ".";
                promptText->SetText(text);
            }
            else
            {
                for (int k = 0; k < 3; k++)
                {
                    text.pop_back();
                }
                promptText->SetText(text);
            }
        }
    }

    promptText->SetText(std::string());

    if (callback)
    {
        callback();
    }
    co_return;
}