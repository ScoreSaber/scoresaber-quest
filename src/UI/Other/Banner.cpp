#include "UI/Other/Banner.hpp"

#include "Data/Private/Settings.hpp"
#include <GlobalNamespace/HMTask.hpp>
#include <GlobalNamespace/SharedCoroutineStarter.hpp>
#include <HMUI/CurvedCanvasSettingsHelper.hpp>
#include <HMUI/ImageView.hpp>
#include <HMUI/ViewController_AnimationDirection.hpp>
#include "Services/PlayerService.hpp"
#include "Sprites.hpp"
#include <System/Action.hpp>
#include <TMPro/TextAlignmentOptions.hpp>
#include "UI/FlowCoordinators/ScoreSaberFlowCoordinator.hpp"
#include "UI/FlowCoordinators/ScoreSaberSettingsFlowCoordinator.hpp"
#include <UnityEngine/Application.hpp>
#include <UnityEngine/Rect.hpp>
#include <UnityEngine/RectOffset.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/Time.hpp>
#include <UnityEngine/UI/LayoutElement.hpp>
#include <UnityEngine/WaitForSeconds.hpp>
#include "Utils/UIUtils.hpp"
#include "logging.hpp"
#include <custom-types/shared/delegate.hpp>
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include <chrono>

DEFINE_TYPE(ScoreSaber::UI::Other, Banner);

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace ScoreSaber::Data::Private;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->gameObject->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->gameObject->AddComponent<LayoutElement*>();  \
    layout##identifier->preferredWidth = width;                                          \
    layout##identifier->preferredHeight = height

namespace ScoreSaber::UI::Other
{
    Sprite* GetGameSprite(StringW name)
    {
        for (auto x : Resources::FindObjectsOfTypeAll<Sprite*>()) {
            if (x->name == name) {
                return x;
            }
        }
        return nullptr;
    }

    Banner* Banner::Create(Transform* parent)
    {
        auto panel = CreateCanvas();
        auto panelTransform = panel->transform;
        panelTransform->SetParent(parent, false);

        auto panelRectTransform = panel->GetComponent<RectTransform*>();
        panelRectTransform->localScale = Vector3(1, 1, 1);
        panelRectTransform->anchoredPosition = {7.5f, 50.0f};

        auto banner = panel->gameObject->AddComponent<Banner*>();
        banner->Setup();
        return banner;
    }

    void Banner::Setup()
    {
        // tbh with all this layout stuff I fuck around till it works the way I want it to
        auto horizon = CreateHorizontalLayoutGroup(transform);
        horizon->childForceExpandWidth = false;
        horizon->childForceExpandHeight = true;
        horizon->childControlWidth = false;
        horizon->childControlHeight = true;
        horizon->spacing = 2.0f;
        horizon->padding = RectOffset::New_ctor(2, 2, 2, 2);
        SetPreferredSize(horizon, 90.5, 14);

        auto buttonVertical = CreateVerticalLayoutGroup(horizon->transform);
        auto seperatorVertical = CreateVerticalLayoutGroup(horizon->transform);
        auto infoVertical = CreateVerticalLayoutGroup(horizon->transform);
        loadingVertical = CreateVerticalLayoutGroup(transform);
        auto settingsVertical = CreateVerticalLayoutGroup(horizon->transform);

        SetPreferredSize(buttonVertical, 10, 10);
        SetPreferredSize(seperatorVertical, 0.5f, 10);
        SetPreferredSize(infoVertical, 70, 10);
        SetPreferredSize(loadingVertical, 10, 10);
        SetPreferredSize(settingsVertical, 6, 10);

        bg = horizon->gameObject->AddComponent<Backgroundable*>();
        bg->ApplyBackgroundWithAlpha("title-gradient", 1.0f);

        bgImage = bg->gameObject->GetComponentInChildren<ImageView*>();
        bgImage->skew = 0.18f;
        bgImage->gradient = true;
        bgImage->gradientDirection = 0;
        bgImage->color0 = Color(1, 1, 1, 1);
        bgImage->color1 = Color(1, 1, 1, 0);
        bgImage->curvedCanvasSettingsHelper->Reset();

        color = defaultColor;

        // main menu button setup
        float buttonSize = 10.0f;
        auto btn = CreateUIButton(buttonVertical->transform, "", "SettingsButton", Vector2(0, 0), Vector2(buttonSize, buttonSize), std::bind(&Banner::OpenMainMenuFlowCoordinator, this));
        reinterpret_cast<RectTransform*>(btn->transform->GetChild(0))->sizeDelta = {buttonSize, buttonSize};

        auto scoresaber_active = Base64ToSprite(ScoreSaber_Active);
        auto scoresaber_inactive = Base64ToSprite(ScoreSaber_Inactive);
        SetButtonSprites(btn, scoresaber_inactive, scoresaber_active);
        auto btnImageView = btn->gameObject->GetComponentInChildren<ImageView*>();
        btnImageView->skew = 0.18f;
        AddHoverHint(btn->gameObject, "Opens the ScoreSaber main menu");
        auto btnLayout = buttonVertical->gameObject->AddComponent<LayoutElement*>();
        btnLayout->preferredWidth = buttonSize;

        // seperator setup
        auto texture = Texture2D::whiteTexture;
        auto seperatorSprite = Sprite::Create(texture, Rect(0.0f, 0.0f, (float)texture->width, (float)texture->height), Vector2(0.5f, 0.5f), 1024.0f, 1u, SpriteMeshType::FullRect, Vector4(0.0f, 0.0f, 0.0f, 0.0f), false);

        auto image = CreateImage(seperatorVertical->transform, seperatorSprite, Vector2(0, 0), Vector2(0, 0));
        image->skew = 0.18f;
        auto imageLayout = image->gameObject->AddComponent<LayoutElement*>();
        imageLayout->preferredWidth = 1.0f;

        // info setup
        topText = UIUtils::CreateClickableText(infoVertical->transform, "");
        bottomText = UIUtils::CreateClickableText(infoVertical->transform, "");

        topText->onPointerClickEvent += [&](auto _) { OpenPlayerInfoModal(); };
        bottomText->onPointerClickEvent += [&](auto _) { OpenSongInBrowser(); };

        auto loadingHorizontal = CreateHorizontalLayoutGroup(loadingVertical->transform);
        UIUtils::CreateLoadingIndicator(loadingHorizontal->transform);
        SetPreferredSize(loadingHorizontal, 10, 10);

        // settings button setup
        auto setbtn = CreateClickableImage(settingsVertical->transform, GetGameSprite("SettingsIcon"), Vector2(0, 0), Vector2(6, 6), std::bind(&Banner::OpenSettingsFlowCoordinator, this));
        setbtn->preserveAspect = true;
        AddHoverHint(setbtn->gameObject, "Opens the ScoreSaber Settings menu");
        auto setbtnLayout = settingsVertical->gameObject->AddComponent<LayoutElement*>();
        setbtnLayout->preferredWidth = 6;
        auto setbtnRectTransform = settingsVertical->rectTransform;
        setbtnRectTransform->anchoredPosition = {35.0f, 0.0f};


        auto promptRoot = BeatSaberUI::CreateHorizontalLayoutGroup(transform);
        promptRoot->childAlignment = TextAnchor::UpperLeft;
        promptRoot->childForceExpandWidth = false;
        promptRoot->spacing = 1.0f;

        RectTransform* promptRootRect = promptRoot->rectTransform;
        promptRootRect->anchoredPosition = {0.0f, 10.3f};

        LayoutElement* promptElement = promptRoot->GetComponent<LayoutElement*>();
        promptElement->preferredHeight = 7.0f;
        promptElement->preferredWidth = 87.0f;

        ContentSizeFitter* promptFitter = promptRoot->GetComponent<ContentSizeFitter*>();
        promptFitter->horizontalFit = ContentSizeFitter::FitMode::PreferredSize;

        HorizontalLayoutGroup* textGroup =
            BeatSaberUI::CreateHorizontalLayoutGroup(promptRootRect);
        textGroup->rectTransform->anchoredPosition = {0.0f, 10.0f};

        promptText = BeatSaberUI::CreateText(textGroup->transform, "...", false);
        promptText->alignment = TMPro::TextAlignmentOptions::BottomLeft;
    }

    void Banner::OpenMainMenuFlowCoordinator()
    {
        // HACK: Resources call to get these objects to use for changing menu is bad but there's no good other choice afaik
        auto mainfc = GetMainFlowCoordinator();
        auto youngest = mainfc->YoungestChildFlowCoordinatorOrSelf();

        auto fc = ArrayUtil::First(Resources::FindObjectsOfTypeAll<ScoreSaber::UI::FlowCoordinators::ScoreSaberFlowCoordinator*>());
        if (!fc)
        {
            fc = CreateFlowCoordinator<ScoreSaber::UI::FlowCoordinators::ScoreSaberFlowCoordinator*>();
        }

        youngest->PresentFlowCoordinator(fc, nullptr, ViewController::AnimationDirection::Horizontal, false, false);
    }

    void Banner::OpenSettingsFlowCoordinator()
    {
        // HACK: Resources call to get these objects to use for changing menu is bad but there's no good other choice afaik
        auto mainfc = GetMainFlowCoordinator();
        auto youngest = mainfc->YoungestChildFlowCoordinatorOrSelf();

        auto fc = ArrayUtil::First(Resources::FindObjectsOfTypeAll<ScoreSaber::UI::FlowCoordinators::ScoreSaberSettingsFlowCoordinator*>());
        if (!fc)
        {
            fc = CreateFlowCoordinator<ScoreSaber::UI::FlowCoordinators::ScoreSaberSettingsFlowCoordinator*>();
        }

        youngest->PresentFlowCoordinator(fc, nullptr, ViewController::AnimationDirection::Horizontal, false, false);
    }

    void Banner::OpenPlayerInfoModal()
    {
        // just make sure to have this actually assigned
        if (playerProfileModal && Object::IsNativeObjectAlive(playerProfileModal))
        {
            playerProfileModal->Show(ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.id);
        }
    }

    void Banner::OpenSongInBrowser()
    {
        auto songURL = string_format("https://scoresaber.com/leaderboard/%d", scoreboardId);
        Application::OpenURL(songURL);
    }

    void Banner::rainbow = bool value
    {
        rainbow = value;
    }

    void Banner::Update()
    {
        // if we are not going to be rainbow, and we were just rainbow
        if (!rainbow && wasRainbow)
        {
            // set bg color to default
            wasRainbow = false;
            color = defaultColor;
        }
        else if (rainbow)
        {
            wasRainbow = true;
            // make this color dependent on some kind of rainbow / gradient source
            colorAngle += Time::deltaTime * 0.1f;
            colorAngle = std::fmod(colorAngle, 1.0f);
            UnityEngine::Color color = UnityEngine::Color::HSVToRGB(colorAngle, 1.0f, 1.0f);
            color = color;
        }
    }

    void Banner::Prompt(std::string status, bool loadingIndicator, float dismiss,
                        std::function<void()> callback)
    {
        GlobalNamespace::SharedCoroutineStarter::instance->StartCoroutine(

            custom_types::Helpers::CoroutineHelper::New(SetPrompt(status, loadingIndicator, dismiss, callback)));
    }

    custom_types::Helpers::Coroutine Banner::SetPrompt(
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

        if (dismiss > 0)
        {
            promptText->text = std::string();
        }

        if (callback)
        {
            callback();
        }

        co_return;
    }

    void Banner::prompt = std::string text, int dismissTime
    {
        if (!Settings::showStatusText) {
            return;
        }

        promptText->text = text;
        if (dismissTime != -1)
        {
            HMTask::New_ctor(custom_types::MakeDelegate<System::Action*>((std::function<void()>)[dismissTime, this] {
                std::this_thread::sleep_for(std::chrono::seconds(dismissTime));
                QuestUI::MainThreadScheduler::Schedule([=]() {
                    this->promptText->text = std::string();
                });
            }), nullptr)->Run();
        }
    }

    void Banner::color = UnityEngine::Color color
    {
        bgImage->color = color;
    }

    void Banner::loading = bool value
    {
        loadingVertical->gameObject->SetActive(value);
        topText->gameObject->SetActive(!value);
        bottomText->gameObject->SetActive(!value);
    }

    void Banner::ranking = int rank, float pp
    {
        if (Settings::showLocalPlayerRank) {
            topText = string_format("<b><color=#FFDE1A>Global Ranking: </color></b>#%d<size=3> (<color=#6772E5>%.2fpp</color></size>)", rank, pp);
        } else {
            topText = "<b>Hidden</b>";
        }
        loading = false;
    }

    void Banner::status = std::string_view status, int scoreboardId
    {
        bottomText = string_format("<b><color=#FFDE1A>Ranked Status:</color></b> %s", status.data());
        this->scoreboardId = scoreboardId;
        loading = false;
    }

    void Banner::topText = std::u16string_view newText
    {
        topText->text = u"<i>" + std::u16string(newText) + u"</i>";
        topText->gameObject->SetActive(true);
    }

    void Banner::bottomText = std::u16string_view newText
    {
        bottomText->text = u"<i>" + std::u16string(newText) + u"</i>";
        bottomText->gameObject->SetActive(true);
    }
} // namespace ScoreSaber::UI::Other