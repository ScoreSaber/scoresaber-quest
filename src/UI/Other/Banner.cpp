#include "UI/Other/Banner.hpp"

#include "Data/Private/Settings.hpp"
#include <HMUI/CurvedCanvasSettingsHelper.hpp>
#include <HMUI/ImageView.hpp>
#include <HMUI/ViewController.hpp>
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
#include <UnityEngine/UI/ContentSizeFitter.hpp>
#include <UnityEngine/WaitForSeconds.hpp>
#include "Utils/UIUtils.hpp"
#include "logging.hpp"
#include <custom-types/shared/delegate.hpp>
#include <bsml/shared/BSML/MainThreadScheduler.hpp>
#include <bsml/shared/BSML/Components/Backgroundable.hpp>
#include <bsml/shared/Helpers/creation.hpp>
#include <bsml/shared/Helpers/getters.hpp>
#include <chrono>
#include "Utils/StrippedMethods.hpp"

DEFINE_TYPE(ScoreSaber::UI::Other, Banner);

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace BSML;
using namespace BSML::Helpers;
using namespace BSML::Lite;
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
        bg->ApplyBackground("title-gradient");
        bg->ApplyAlpha(1.0f);

        bgImage = bg->gameObject->GetComponentInChildren<ImageView*>();
        bgImage->_skew = 0.18f;
        bgImage->_gradient = true;
        bgImage->_gradientDirection = 0;
        bgImage->_color0 = Color(1, 1, 1, 1);
        bgImage->_color1 = Color(1, 1, 1, 0);
        bgImage->_curvedCanvasSettingsHelper->Reset();

        set_color(defaultColor);

        // main menu button setup
        float buttonSize = 10.0f;
        auto btn = CreateUIButton(buttonVertical->transform, "", "SettingsButton", Vector2(0, 0), Vector2(buttonSize, buttonSize), std::bind(&Banner::OpenMainMenuFlowCoordinator, this));
        btn->transform->GetChild(0).cast<RectTransform>()->sizeDelta = {buttonSize, buttonSize};

        auto scoresaber_active = Base64ToSprite(ScoreSaber_Active);
        auto scoresaber_inactive = Base64ToSprite(ScoreSaber_Inactive);
        SetButtonSprites(btn, scoresaber_inactive, scoresaber_active);
        auto btnImageView = btn->gameObject->GetComponentInChildren<ImageView*>();
        btnImageView->_skew = 0.18f;
        BSML::Lite::AddHoverHint(btn->gameObject, "Opens the ScoreSaber main menu");
        auto btnLayout = buttonVertical->gameObject->AddComponent<LayoutElement*>();
        btnLayout->preferredWidth = buttonSize;

        // seperator setup
        auto texture = Texture2D::get_whiteTexture();
        auto seperatorSprite = Sprite::Create(texture, Rect(0.0f, 0.0f, (float)texture->width, (float)texture->height), Vector2(0.5f, 0.5f), 1024.0f, 1u, SpriteMeshType::FullRect, Vector4(0.0f, 0.0f, 0.0f, 0.0f), false);

        auto image = CreateImage(seperatorVertical->transform, seperatorSprite, Vector2(0, 0), Vector2(0, 0));
        image->_skew = 0.18f;
        auto imageLayout = image->gameObject->AddComponent<LayoutElement*>();
        imageLayout->preferredWidth = 1.0f;

        // info setup
        topText = CreateClickableText(infoVertical->transform, "");
        bottomText = CreateClickableText(infoVertical->transform, "");

        topText->onClick += [&]() { OpenPlayerInfoModal(); };
        bottomText->onClick += [&]() { OpenSongInBrowser(); };

        auto loadingHorizontal = CreateHorizontalLayoutGroup(loadingVertical->transform);
        UIUtils::CreateLoadingIndicator(loadingHorizontal->transform);
        SetPreferredSize(loadingHorizontal, 10, 10);

        // settings button setup
        auto setbtn = CreateClickableImage(settingsVertical->transform, GetGameSprite("SettingsIcon"), std::bind(&Banner::OpenSettingsFlowCoordinator, this), Vector2(0, 0), Vector2(6, 6));
        setbtn->preserveAspect = true;
        BSML::Lite::AddHoverHint(setbtn->gameObject, "Opens the ScoreSaber Settings menu");
        auto setbtnLayout = settingsVertical->gameObject->AddComponent<LayoutElement*>();
        setbtnLayout->preferredWidth = 6;
        auto setbtnRectTransform = settingsVertical->rectTransform;
        setbtnRectTransform->anchoredPosition = {35.0f, 0.0f};


        auto promptRoot = CreateHorizontalLayoutGroup(transform);
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

        HorizontalLayoutGroup* textGroup = CreateHorizontalLayoutGroup(promptRootRect);
        textGroup->rectTransform->anchoredPosition = {0.0f, 10.0f};

        promptText = CreateText(textGroup->transform, "...", false);
        promptText->alignment = TMPro::TextAlignmentOptions::BottomLeft;
    }

    void Banner::OpenMainMenuFlowCoordinator()
    {
        // HACK: Resources call to get these objects to use for changing menu is bad but there's no good other choice afaik
        auto mainfc = GetMainFlowCoordinator();
        auto youngest = mainfc->YoungestChildFlowCoordinatorOrSelf();

        auto fc = Resources::FindObjectsOfTypeAll<ScoreSaber::UI::FlowCoordinators::ScoreSaberFlowCoordinator*>()->First();
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

        auto fc = Resources::FindObjectsOfTypeAll<ScoreSaber::UI::FlowCoordinators::ScoreSaberSettingsFlowCoordinator*>()->First();
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
        auto songURL = fmt::format("https://scoresaber.com/leaderboard/{:d}", scoreboardId);
        StrippedMethods::UnityEngine::Application::OpenURL(songURL);
    }

    void Banner::set_rainbow(bool value)
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
            set_color(defaultColor);
        }
        else if (rainbow)
        {
            wasRainbow = true;
            // make this color dependent on some kind of rainbow / gradient source
            colorAngle += Time::get_deltaTime() * 0.1f;
            colorAngle = std::fmod(colorAngle, 1.0f);
            UnityEngine::Color color = UnityEngine::Color::HSVToRGB(colorAngle, 1.0f, 1.0f);
            color = color;
        }
    }

    void Banner::Prompt(std::string status, bool loadingIndicator, float dismiss,
                        std::function<void()> callback)
    {
        this->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(SetPrompt(status, loadingIndicator, dismiss, callback)));
    }

    custom_types::Helpers::Coroutine Banner::SetPrompt(
        std::string status, bool showIndicator, float dismiss,
        std::function<void()> callback)
    {
        if (!Settings::showStatusText) {
            co_return;
        }

        this->promptText->text = status;

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
                    promptText->text = text;
                }
                else
                {
                    for (int k = 0; k < 3; k++)
                    {
                        text.pop_back();
                    }
                    promptText->text = text;
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

    void Banner::set_prompt(std::string text, int dismissTime)
    {
        if (!Settings::showStatusText) {
            return;
        }

        promptText->text = text;
        if (dismissTime != -1)
        {
            il2cpp_utils::il2cpp_aware_thread([dismissTime, this] {
                std::this_thread::sleep_for(std::chrono::seconds(dismissTime));
                MainThreadScheduler::Schedule([=, this]() {
                    this->promptText->text = std::string();
                });
            }).detach();
        }
    }

    void Banner::set_color(UnityEngine::Color color)
    {
        bgImage->color = color;
    }

    void Banner::set_loading(bool value)
    {
        loadingVertical->gameObject->SetActive(value);
        topText->gameObject->SetActive(!value);
        bottomText->gameObject->SetActive(!value);
    }

    void Banner::set_ranking(int rank, float pp)
    {
        if (Settings::showLocalPlayerRank) {
            set_topText(fmt::format("<b><color=#FFDE1A>Global Ranking: </color></b>#{:d}<size=3> (<color=#6772E5>{:.2f}pp</color></size>)", rank, pp));
        } else {
            set_topText("<b>Hidden</b>");
        }
        set_loading(false);
    }

    void Banner::set_status(std::string_view status, int scoreboardId)
    {
        set_bottomText(fmt::format("<b><color=#FFDE1A>Ranked Status:</color></b> {:s}", status.data()));
        this->scoreboardId = scoreboardId;
        set_loading(false);
    }

    void Banner::set_topText(std::u16string_view newText)
    {
        topText->text = u"<i>" + std::u16string(newText) + u"</i>";
        topText->gameObject->SetActive(true);
    }

    void Banner::set_bottomText(std::u16string_view newText)
    {
        bottomText->text = u"<i>" + std::u16string(newText) + u"</i>";
        bottomText->gameObject->SetActive(true);
    }
} // namespace ScoreSaber::UI::Other