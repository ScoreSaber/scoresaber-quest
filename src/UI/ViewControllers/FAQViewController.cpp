#include "UI/ViewControllers/FAQViewController.hpp"

#include <UnityEngine/Application.hpp>
#include <UnityEngine/Color.hpp>
#include <UnityEngine/RectOffset.hpp>
#include <UnityEngine/UI/HorizontalLayoutGroup.hpp>
#include <UnityEngine/UI/LayoutElement.hpp>
#include <UnityEngine/UI/VerticalLayoutGroup.hpp>
#include "Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/QuestUI.hpp"

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
    VerticalLayoutGroup* layoutGroup = BeatSaberUI::CreateVerticalLayoutGroup(parent);
    layoutGroup->padding->bottom = 4;
    layoutGroup->padding->left = 4;
    layoutGroup->padding->right = 4;
    layoutGroup->padding->top = 4;

    ContentSizeFitter* layoutGroupFitter = layoutGroup->GetComponent<ContentSizeFitter*>();
    layoutGroupFitter->verticalFit = ContentSizeFitter::FitMode::PreferredSize;
    layoutGroupFitter->horizontalFit = ContentSizeFitter::FitMode::PreferredSize;

    QuestUI::Backgroundable* bg = layoutGroup->gameObject->AddComponent<QuestUI::Backgroundable*>();
    bg->ApplyBackgroundWithAlpha("title-gradient", 1.0f);

    HMUI::ImageView* imageView = bg->gameObject->GetComponentInChildren<HMUI::ImageView*>();
    imageView->gradient = true;
    imageView->gradientDirection = 1;
    imageView->color = Color::white;
    imageView->color0 = data.colora;
    imageView->color1 = data.colorb;

    LayoutElement* layoutElement = layoutGroup->GetComponent<LayoutElement*>();
    layoutElement->preferredWidth = 40.0f;
    layoutElement->preferredHeight = 60.0f;

    ScoreSaber::CustomTypes::Components::ClickableText* text = UIUtils::CreateClickableText(layoutGroup->transform, Paper::StringConvert::from_utf8(data.mainText), [url = data.mainURL]() { Application::OpenURL(url); });
    text->fontSize = 6.5f;
    text->alignment = TextAlignmentOptions::Center;

    BeatSaberUI::CreateUIButton(layoutGroup->transform, "Discord", [url = data.discord]() { Application::OpenURL(url); });
    BeatSaberUI::CreateUIButton(layoutGroup->transform, "Twitter", [url = data.twitter]() { Application::OpenURL(url); });
    BeatSaberUI::CreateUIButton(layoutGroup->transform, "Patreon", [url = data.patreon]() { Application::OpenURL(url); });

    VerticalLayoutGroup* imageLayout = BeatSaberUI::CreateVerticalLayoutGroup(layoutGroup->transform);

    ContentSizeFitter* imageFitter = imageLayout->GetComponent<ContentSizeFitter*>();
    imageFitter->horizontalFit = ContentSizeFitter::FitMode::PreferredSize;
    imageFitter->verticalFit = ContentSizeFitter::FitMode::PreferredSize;

    VerticalLayoutGroup* imageParent = BeatSaberUI::CreateVerticalLayoutGroup(imageLayout->transform);

    LayoutElement* imageElement = imageParent->GetComponent<LayoutElement*>();
    imageElement->preferredWidth = 20.0f;
    imageElement->preferredHeight = 20.0f;

    HMUI::ImageView* image = BeatSaberUI::CreateImage(imageElement->transform, data.icon, {0.0f, 0.0f}, {0.0f, 0.0f});
    image->preserveAspect = true;
}

namespace ScoreSaber::UI::ViewControllers
{
    void FAQViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            VerticalLayoutGroup* vertical = BeatSaberUI::CreateVerticalLayoutGroup(transform);
            ContentSizeFitter* sizeFitter = vertical->GetComponent<ContentSizeFitter*>();
            sizeFitter->horizontalFit = ContentSizeFitter::FitMode::PreferredSize;
            sizeFitter->verticalFit = ContentSizeFitter::FitMode::PreferredSize;
            vertical->spacing = 2.0f;

            auto headerHorizontal = CreateHorizontalLayoutGroup(vertical->transform);
            headerHorizontal->childAlignment = TextAnchor::MiddleCenter;
            auto headerText = CreateText(headerHorizontal->transform, "Links");
            headerText->alignment = TMPro::TextAlignmentOptions::Center;
            headerText->fontSize = 7.0f;
            auto headerBG = headerHorizontal->gameObject->AddComponent<Backgroundable*>();
            headerBG->ApplyBackgroundWithAlpha("round-rect-panel", 0.5f);

            HorizontalLayoutGroup* horizontal = BeatSaberUI::CreateHorizontalLayoutGroup(vertical->transform);

            ContentSizeFitter* horizFitter = horizontal->GetComponent<ContentSizeFitter*>();
            horizFitter->verticalFit = ContentSizeFitter::FitMode::PreferredSize;
            horizFitter->horizontalFit = ContentSizeFitter::FitMode::PreferredSize;

            LayoutElement* horizontalElement = horizontal->GetComponent<LayoutElement*>();
            horizontalElement->preferredWidth = 90.0f;

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

            CreateLinkBoxes(horizontal->transform, scoreSaber);
            CreateLinkBoxes(horizontal->transform, bsmg);
        }
    }
} // namespace ScoreSaber::UI::ViewControllers