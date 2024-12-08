#include "Utils/UIUtils.hpp"
#include "Utils/WebUtils.hpp"
#include "main.hpp"

#include "CustomTypes/Components/ImageButton.hpp"
#include <GlobalNamespace/HapticFeedbackController.hpp>
#include <GlobalNamespace/MenuShockwave.hpp>
#include <GlobalNamespace/SharedCoroutineStarter.hpp>
#include <HMUI/ButtonSpriteSwap.hpp>
#include <HMUI/CurvedCanvasSettingsHelper.hpp>
#include <HMUI/ImageView.hpp>
#include <HMUI/StackLayoutGroup.hpp>
#include "Libraries/HM/HMLib/VR/HapticPresetSO.hpp"
#include <UnityEngine/Application.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Networking/DownloadHandlerTexture.hpp>
#include <UnityEngine/Networking/UnityWebRequest.hpp>
#include <UnityEngine/Networking/UnityWebRequestTexture.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/ScriptableObject.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/SpriteRenderer.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/UI/LayoutElement.hpp>
#include <UnityEngine/Vector3.hpp>
#include "questui/shared/ArrayUtil.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "Sprites.hpp"
#include "logging.hpp"

using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->gameObject->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->gameObject->AddComponent<LayoutElement*>();  \
    layout##identifier->preferredWidth = width;                                          \
    layout##identifier->preferredHeight = height

#define SocialButton(identifier)                                                                                                                                                             \
    std::string str##identifier = member.get_##identifier();                                                                                                                                 \
    if (str##identifier != "")                                                                                                                                                               \
    {                                                                                                                                                                                        \
        auto identifier##_sprite = Base64ToSprite(identifier##_base64);                                                                                                                      \
        auto btn##identifier = CreateClickableImage(socialHorizontal->transform, identifier##_sprite, {0, 0}, {0, 0}, [str##identifier]() { Application::OpenURL(str##identifier); }); \
        SetPreferredSize(btn##identifier, 3, 3);                                                                                                                                             \
        AddHoverHint(btn##identifier->gameObject, "Opens in Browser");                                                                                                                 \
    }

#define BeginCoroutine(method) \
    BSML::Helpers::GetDiContainer()->Resolve<GlobalNamespace::ICoroutineStarter*>()->StartCoroutine( \
        custom_types::Helpers::CoroutineHelper::New(method));

using HapticPresetSO = Libraries::HM::HMLib::VR::HapticPresetSO;
static SafePtrUnity<HapticPresetSO> hapticFeedbackPresetSO;

namespace UIUtils
{
    ScoreSaber::CustomTypes::Components::ClickableText* CreateClickableText(UnityEngine::Transform* parent, std::u16string_view text, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, std::function<void()> onClick)
    {
        auto clickableText = CreateClickableText(parent, text, anchoredPosition, sizeDelta);
        if (onClick)
            clickableText->onPointerClickEvent += [onClick](auto _) { onClick(); };
        return clickableText;
    }

    ScoreSaber::CustomTypes::Components::ClickableText* CreateClickableText(UnityEngine::Transform* parent, std::u16string_view text, std::function<void()> onClick)
    {
        auto clickableText = CreateClickableText(parent, text);
        if (onClick)
            clickableText->onPointerClickEvent += [onClick](auto _) { onClick(); };
        return clickableText;
    }

    ScoreSaber::CustomTypes::Components::ClickableText* CreateClickableText(Transform* parent, std::u16string_view text, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta)
    {
        GameObject* gameObj = GameObject::New_ctor("QuestUIText");
        gameObj->SetActive(false);

        ScoreSaber::CustomTypes::Components::ClickableText* textMesh = gameObj->AddComponent<ScoreSaber::CustomTypes::Components::ClickableText*>();
        RectTransform* rectTransform = textMesh->rectTransform;
        rectTransform->SetParent(parent, false);
        textMesh->font = GetMainTextFont();
        textMesh->fontSharedMaterial = GetMainUIFontMaterial();

        textMesh->text = text;
        textMesh->fontSize = 4.0f;
        textMesh->color = UnityEngine::Color::white;
        textMesh->richText = true;
        rectTransform->anchorMin = UnityEngine::Vector2(0.5f, 0.5f);
        rectTransform->anchorMax = UnityEngine::Vector2(0.5f, 0.5f);
        rectTransform->anchoredPosition = anchoredPosition;
        rectTransform->sizeDelta = sizeDelta;

        gameObj->AddComponent<LayoutElement*>();

        auto menuShockWave = ArrayUtil::First(Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuShockwave*>());
        auto buttonClickedSignal = ArrayUtil::Last(menuShockWave->buttonClickEvents);
        textMesh->buttonClickedSignal = buttonClickedSignal;

        if (!hapticFeedbackPresetSO)
        {
            hapticFeedbackPresetSO.emplace(UnityEngine::ScriptableObject::CreateInstance<HapticPresetSO*>());
            hapticFeedbackPresetSO->duration = 0.02f;
            hapticFeedbackPresetSO->strength = 1.0f;
            hapticFeedbackPresetSO->frequency = 0.2f;
        }

        auto hapticFeedbackController = UnityEngine::Object::FindObjectOfType<GlobalNamespace::HapticFeedbackController*>();
        textMesh->hapticFeedbackController = hapticFeedbackController;
        textMesh->hapticFeedbackPresetSO = (HapticPresetSO*)hapticFeedbackPresetSO;

        gameObj->SetActive(true);
        return textMesh;
    }

    ScoreSaber::CustomTypes::Components::ClickableText* CreateClickableText(UnityEngine::Transform* parent, std::string_view text, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta)
    {
        return CreateClickableText(parent, Paper::StringConvert::from_utf8(text), anchoredPosition, sizeDelta);
    };

    UnityEngine::GameObject* CreateLoadingIndicator(UnityEngine::Transform* parent)
    {
        auto original = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<GameObject*>(), [](auto el) { return Paper::StringConvert::from_utf16(el->name) == "LoadingIndicator"; });
        auto loadingIndicator = Object::Instantiate(original, parent, false);
        loadingIndicator->name = "ScoreSaberLoadingIndicator";

        loadingIndicator->AddComponent<LayoutElement*>();
        return loadingIndicator;
    }

    UnityEngine::UI::HorizontalLayoutGroup* CreateTeamMemberLayout(UnityEngine::Transform* parent, TeamUtils::TeamMember& member)
    {
        auto horizontal = CreateHorizontalLayoutGroup(parent);
        SetPreferredSize(horizontal, 30, -1);
        horizontal->spacing = 2.0f;
        auto url = member.profilePicture;
        auto image = CreateImage(horizontal->transform, Base64ToSprite(oculus_base64), Vector2(0, 0), Vector2(0, 0));
        if (url.ends_with(".gif"))
        {
            BeginCoroutine(WebUtils::WaitForGifDownload(url, image));
        }
        else
        {
            BeginCoroutine(WebUtils::WaitForImageDownload(url, image));
        }
        SetPreferredSize(image, 12, 12);
        image->preserveAspect = true;

        auto infoVertical = CreateVerticalLayoutGroup(horizontal->transform);
        SetPreferredSize(infoVertical, -1, 15);
        auto nameVertical = CreateVerticalLayoutGroup(infoVertical->transform);
        SetPreferredSize(nameVertical, 15, -1);
        auto socialHorizontal = CreateHorizontalLayoutGroup(infoVertical->transform);
        socialHorizontal->childAlignment = TextAnchor::LowerLeft;
        socialHorizontal->childForceExpandWidth = false;
        socialHorizontal->childForceExpandHeight = false;
        SetPreferredSize(socialHorizontal, 15, 5);

        auto nameText = CreateText(nameVertical->transform, Paper::StringConvert::from_utf8(member.name));
        SocialButton(discord);
        SocialButton(github);
        SocialButton(twitch);
        SocialButton(twitter);
        SocialButton(youtube);
        return horizontal;
    }

    ScoreSaber::CustomTypes::Components::ClickableImage* CreateClickableImage(UnityEngine::Transform* parent, UnityEngine::Sprite* sprite, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, std::function<void()> onClick)
    {
        auto image = CreateClickableImage(parent, sprite, anchoredPosition, sizeDelta);
        if (onClick)
        {
            image->onPointerClickEvent += [onClick](auto _) { onClick(); };
        }
        return image;
    }

    ScoreSaber::CustomTypes::Components::ClickableImage* CreateClickableImage(UnityEngine::Transform* parent, UnityEngine::Sprite* sprite, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta)
    {
        auto go = GameObject::New_ctor("ScoreSaberClickableImage");

        auto image = go->AddComponent<ScoreSaber::CustomTypes::Components::ClickableImage*>();
        auto mat_UINoGlow = ArrayUtil::First(Resources::FindObjectsOfTypeAll<Material*>(), [](Material* x) { return Paper::StringConvert::from_utf16(x->name) == "UINoGlow"; });
        image->material = mat_UINoGlow;

        go->transform->SetParent(parent, false);
        image->rectTransform->sizeDelta = sizeDelta;
        image->rectTransform->anchoredPosition = anchoredPosition;
        image->sprite = sprite;

        go->AddComponent<LayoutElement*>();

        auto menuShockWave = ArrayUtil::First(Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuShockwave*>());
        auto buttonClickedSignal = ArrayUtil::Last(menuShockWave->buttonClickEvents);
        image->buttonClickedSignal = buttonClickedSignal;

        if (!hapticFeedbackPresetSO)
        {
            hapticFeedbackPresetSO.emplace(UnityEngine::ScriptableObject::CreateInstance<HapticPresetSO*>());
            hapticFeedbackPresetSO->duration = 0.01f;
            hapticFeedbackPresetSO->strength = 0.75f;
            hapticFeedbackPresetSO->frequency = 0.5f;
        }

        auto hapticFeedbackController = UnityEngine::Object::FindObjectOfType<GlobalNamespace::HapticFeedbackController*>();
        image->hapticFeedbackController = hapticFeedbackController;
        image->hapticFeedbackPresetSO = (HapticPresetSO*)hapticFeedbackPresetSO;

        return image;
    }

    HorizontalLayoutGroup* CreateHeader(UnityEngine::Transform* parent,
                                        UnityEngine::Color color)
    {
        VerticalLayoutGroup* vertical = CreateVerticalLayoutGroup(parent);
        vertical->rectTransform->anchoredPosition = {0.0f, 45.0f};
        HorizontalLayoutGroup* horizontal = CreateHorizontalLayoutGroup(vertical->transform);

        Backgroundable* background = horizontal->gameObject->AddComponent<Backgroundable*>();
        background->ApplyBackgroundWithAlpha("panel-top", 1.0f);

        ImageView* imageView = background->gameObject->GetComponentInChildren<ImageView*>();
        imageView->gradient = true;
        imageView->gradientDirection = 0;
        imageView->color = Color::white;
        imageView->color0 = color;
        imageView->color1 = color;
        imageView->curvedCanvasSettingsHelper->Reset();

        return horizontal;
    }

    ScoreSaber::CustomTypes::Components::ImageButton* CreateImageButton(GameObject* parent, Sprite* sprite,
                                                                        Vector2 anchoredPosition,
                                                                        Vector2 sizeDelta,
                                                                        std::function<void()> onClick)
    {
        ScoreSaber::CustomTypes::Components::ImageButton* button = parent->AddComponent<ScoreSaber::CustomTypes::Components::ImageButton*>();
        button->sprite = sprite;
        button->Init(parent->transform, anchoredPosition, sizeDelta, onClick);
        return button;
    }

    UnityEngine::GameObject* CreateStackLayoutGroup(UnityEngine::Transform* parent)
    {
        auto stack = GameObject::New_ctor("ScoreSaberStackLayoutGroup");
        stack->transform->SetParent(parent, false);
        stack->AddComponent<HMUI::StackLayoutGroup*>();
        stack->AddComponent<ContentSizeFitter*>();
        stack->AddComponent<Backgroundable*>();
        auto rowStackRect = reinterpret_cast<RectTransform*>(stack->transform);
        rowStackRect->anchorMin = {0, 0};
        rowStackRect->anchorMax = {1, 1};
        rowStackRect->sizeDelta = {0, 0};
        stack->AddComponent<LayoutElement*>();

        return stack;
    }
} // namespace UIUtils
