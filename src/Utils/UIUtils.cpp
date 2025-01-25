#include "Utils/UIUtils.hpp"
#include "Utils/WebUtils.hpp"
#include "main.hpp"

#include "CustomTypes/Components/ImageButton.hpp"
#include <GlobalNamespace/MenuShockwave.hpp>
#include <GlobalNamespace/ICoroutineStarter.hpp>
#include <HMUI/ButtonSpriteSwap.hpp>
#include <HMUI/CurvedCanvasSettingsHelper.hpp>
#include <HMUI/ImageView.hpp>
#include <HMUI/StackLayoutGroup.hpp>
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
#include <bsml/shared/BSML-Lite.hpp>
#include <bsml/shared/BSML/Components/Backgroundable.hpp>
#include <bsml/shared/BSML/SharedCoroutineStarter.hpp>
#include <bsml/shared/Helpers/getters.hpp>
#include <paper/shared/string_convert.hpp>
#include "questui/ArrayUtil.hpp"
#include "Utils/StrippedMethods.hpp"

#include "Sprites.hpp"
#include "logging.hpp"

using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace BSML;
using namespace BSML::Lite;

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
        auto btn##identifier = CreateClickableImage(socialHorizontal->transform, identifier##_sprite, [str##identifier]() { StrippedMethods::UnityEngine::Application::OpenURL(str##identifier); }, {0, 0}, {0, 0}); \
        SetPreferredSize(btn##identifier, 3, 3);                                                                                                                                             \
        AddHoverHint(btn##identifier->gameObject, "Opens in Browser");                                                                                                                 \
    }

#define BeginCoroutine(method) BSML::SharedCoroutineStarter::StartCoroutine(custom_types::Helpers::CoroutineHelper::New(method))

using HapticPresetSO = Libraries::HM::HMLib::VR::HapticPresetSO;
static SafePtrUnity<HapticPresetSO> hapticFeedbackPresetSO;

namespace UIUtils
{
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
        auto url = member.get_profilePicture();
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

        auto nameText = CreateText(nameVertical->transform, Paper::StringConvert::from_utf8(member.get_name()));
        SocialButton(discord);
        SocialButton(github);
        SocialButton(twitch);
        SocialButton(twitter);
        SocialButton(youtube);
        return horizontal;
    }

    HorizontalLayoutGroup* CreateHeader(UnityEngine::Transform* parent,
                                        UnityEngine::Color color)
    {
        VerticalLayoutGroup* vertical = CreateVerticalLayoutGroup(parent);
        vertical->rectTransform->anchoredPosition = {0.0f, 45.0f};
        HorizontalLayoutGroup* horizontal = CreateHorizontalLayoutGroup(vertical->transform);

        Backgroundable* background = horizontal->gameObject->AddComponent<Backgroundable*>();
        background->ApplyBackground("panel-top");
        background->ApplyAlpha(1.0f);

        ImageView* imageView = background->gameObject->GetComponentInChildren<ImageView*>();
        imageView->gradient = true;
        imageView->_gradientDirection = 0;
        imageView->color = Color::get_white();
        imageView->color0 = color;
        imageView->color1 = color;
        imageView->_curvedCanvasSettingsHelper->Reset();

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
} // namespace UIUtils
