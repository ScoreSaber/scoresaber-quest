#include "UI/ViewControllers/FAQViewController.hpp"

#include "UnityEngine/Application.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/QuestUI.hpp"

#include "Sprites.hpp"

DEFINE_TYPE(ScoreSaber::UI::ViewControllers, FAQViewController);

using namespace ScoreSaber;
using namespace ScoreSaber::UI::ViewControllers;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace TMPro;

// clickable text
#define SCORESABER_LINK "https://scoresaber.com/"
// button
#define SCORESABER_DISCORD "https://discord.gg/scoresaber"
// button
#define SCORESABER_TWITTER "https://twitter.com/scoresaber"
// button
#define SCORESABER_PATREON "https://www.patreon.com/scoresaber"

// clickable text
#define BSMG_LINK "https://bsmg.wiki/"
// button
#define BSMG_DISCORD "https://discord.gg/beatsabermods"
// button
#define BSMG_TWITTER "https://twitter.com/beatsabermods"
// button
#define BSMG_PATREON "https://www.patreon.com/beatsabermods"

struct LinkBoxData
{
    std::string mainText;
    std::string mainURL;
    std::string discord;
    std::string twitter;
    std::string patreon;
    UnityEngine::Sprite* icon;
    UnityEngine::Color colora;
    UnityEngine::Color colorb;
};

void CreateLinkBoxes(Transform* parent, LinkBoxData data)
{
    VerticalLayoutGroup* layoutGroup =
        BeatSaberUI::CreateVerticalLayoutGroup(parent);
    layoutGroup->get_padding()->set_bottom(4);
    layoutGroup->get_padding()->set_left(4);
    layoutGroup->get_padding()->set_right(4);
    layoutGroup->get_padding()->set_top(4);

    ContentSizeFitter* layoutGroupFitter = layoutGroup->GetComponent<ContentSizeFitter*>();
    layoutGroupFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
    layoutGroupFitter->set_horizontalFit(
        ContentSizeFitter::FitMode::PreferredSize);

    QuestUI::Backgroundable* bg = layoutGroup->get_gameObject()->AddComponent<QuestUI::Backgroundable*>();
    bg->ApplyBackgroundWithAlpha(il2cpp_utils::newcsstr("title-gradient"), 1.0f);

    HMUI::ImageView* imageView = bg->get_gameObject()->GetComponentInChildren<HMUI::ImageView*>();
    imageView->gradient = true;
    imageView->gradientDirection = 1;
    imageView->set_color(Color::get_white());
    imageView->set_color0(data.colora);
    imageView->set_color1(data.colorb);

    LayoutElement* layoutElement = layoutGroup->GetComponent<LayoutElement*>();
    layoutElement->set_preferredWidth(40.0f);
    layoutElement->set_preferredHeight(60.0f);

    ScoreSaber::CustomTypes::Components::ClickableText* text =
        UIUtils::CreateClickableText(layoutGroup->get_transform(), to_utf16(data.mainText), [url = data.mainURL]() { Application::OpenURL(il2cpp_utils::newcsstr(url)); });
    text->set_fontSize(6.5f);
    text->set_alignment(TextAlignmentOptions::Center);

    BeatSaberUI::CreateUIButton(layoutGroup->get_transform(), "Discord", [url = data.discord]() { Application::OpenURL(il2cpp_utils::newcsstr(url)); });
    BeatSaberUI::CreateUIButton(layoutGroup->get_transform(), "Twitter", [url = data.twitter]() { Application::OpenURL(il2cpp_utils::newcsstr(url)); });
    BeatSaberUI::CreateUIButton(layoutGroup->get_transform(), "Patreon", [url = data.patreon]() { Application::OpenURL(il2cpp_utils::newcsstr(url)); });

    VerticalLayoutGroup* imageLayout = BeatSaberUI::CreateVerticalLayoutGroup(layoutGroup->get_transform());

    ContentSizeFitter* imageFitter = imageLayout->GetComponent<ContentSizeFitter*>();
    imageFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
    imageFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);

    VerticalLayoutGroup* imageParent = BeatSaberUI::CreateVerticalLayoutGroup(imageLayout->get_transform());

    LayoutElement* imageElement = imageParent->GetComponent<LayoutElement*>();
    imageElement->set_preferredWidth(20.0f);
    imageElement->set_preferredHeight(20.0f);

    HMUI::ImageView* image = BeatSaberUI::CreateImage(imageElement->get_transform(), data.icon, {0.0f, 0.0f}, {0.0f, 0.0f});
    image->set_preserveAspect(true);
}

namespace ScoreSaber::UI::ViewControllers
{
    void FAQViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            VerticalLayoutGroup* vertical = BeatSaberUI::CreateVerticalLayoutGroup(get_transform());
            ContentSizeFitter* sizeFitter = vertical->GetComponent<ContentSizeFitter*>();
            sizeFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
            sizeFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
            vertical->set_spacing(2.0f);

            auto headerHorizontal = CreateHorizontalLayoutGroup(vertical->get_transform());
            headerHorizontal->set_childAlignment(TextAnchor::MiddleCenter);
            auto headerText = CreateText(headerHorizontal->get_transform(), "Links");
            headerText->set_alignment(TMPro::TextAlignmentOptions::Center);
            headerText->set_fontSize(7.0f);
            auto headerBG = headerHorizontal->get_gameObject()->AddComponent<Backgroundable*>();
            headerBG->ApplyBackgroundWithAlpha(il2cpp_utils::newcsstr("round-rect-panel"), 0.5f);

            HorizontalLayoutGroup* horizontal = BeatSaberUI::CreateHorizontalLayoutGroup(vertical->get_transform());

            ContentSizeFitter* horizFitter = horizontal->GetComponent<ContentSizeFitter*>();
            horizFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
            horizFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);

            LayoutElement* horizontalElement = horizontal->GetComponent<LayoutElement*>();
            horizontalElement->set_preferredWidth(90.0f);

            LinkBoxData scoreSaber = {
                "ScoreSaber",
                SCORESABER_LINK,
                SCORESABER_DISCORD,
                SCORESABER_TWITTER,
                SCORESABER_PATREON,
                Base64ToSprite(ScoreSaber_Active),
                Color(0.0f, 0.2f, 0.25f, 1.0f),
                Color(0.0f, 0.35f, 0.4f, 1.0f)};
            LinkBoxData bsmg = {
                "BSMG",
                BSMG_LINK,
                BSMG_DISCORD,
                BSMG_TWITTER,
                BSMG_PATREON,
                Base64ToSprite(bsmg_base64),
                Color(0.05f, 0.0f, 0.05f, 1.0f),
                Color(0.1f, 0.0f, 0.1f, 1.0f)};

            CreateLinkBoxes(horizontal->get_transform(), scoreSaber);
            CreateLinkBoxes(horizontal->get_transform(), bsmg);
        }
    }
} // namespace ScoreSaber::UI::ViewControllers