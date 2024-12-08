#pragma once

#include "CustomTypes/Components/ClickableImage.hpp"
#include "CustomTypes/Components/ClickableText.hpp"
#include "CustomTypes/Components/ImageButton.hpp"
#include <UnityEngine/Color.hpp>
#include <UnityEngine/EventSystems/PointerEventData.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/Transform.hpp>
#include <UnityEngine/UI/Button.hpp>
#include <UnityEngine/UI/HorizontalLayoutGroup.hpp>
#include "Utils/TeamUtils.hpp"
#include <functional>
#include <string_view>

using namespace UnityEngine;
namespace UIUtils
{
    ::ScoreSaber::CustomTypes::Components::ClickableText* CreateClickableText(UnityEngine::Transform* parent, std::u16string_view text, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, std::function<void()> onClick);
    ::ScoreSaber::CustomTypes::Components::ClickableText* CreateClickableText(UnityEngine::Transform* parent, std::u16string_view text, std::function<void()> onClick);
    ::ScoreSaber::CustomTypes::Components::ClickableText* CreateClickableText(UnityEngine::Transform* parent, std::u16string_view text, UnityEngine::Vector2 anchoredPosition = {0, 0}, UnityEngine::Vector2 sizeDelta = {60.0f, 10.0f});
    ::ScoreSaber::CustomTypes::Components::ClickableText* CreateClickableText(UnityEngine::Transform* parent, std::string_view text, UnityEngine::Vector2 anchoredPosition = {0, 0}, UnityEngine::Vector2 sizeDelta = {60.0f, 10.0f});
    UnityEngine::UI::HorizontalLayoutGroup* CreateTeamMemberLayout(UnityEngine::Transform* parent, TeamUtils::TeamMember& member);
    UnityEngine::GameObject* CreateLoadingIndicator(UnityEngine::Transform* parent);
    ::ScoreSaber::CustomTypes::Components::ClickableImage* CreateClickableImage(UnityEngine::Transform* parent, UnityEngine::Sprite* sprite, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, std::function<void()> onClick);
    ::ScoreSaber::CustomTypes::Components::ClickableImage* CreateClickableImage(UnityEngine::Transform* parent, UnityEngine::Sprite* sprite, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta);
    UnityEngine::UI::HorizontalLayoutGroup* CreateHeader(UnityEngine::Transform* parent, UnityEngine::Color color);
    UnityEngine::UI::HorizontalLayoutGroup* CreateHeader(
        UnityEngine::Transform* parent, UnityEngine::Color color);

    ScoreSaber::CustomTypes::Components::ImageButton* CreateImageButton(
        GameObject* parent, Sprite* sprite, Vector2 anchoredPosition,
        Vector2 sizeDelta, std::function<void()> onClick);
    UnityEngine::GameObject* CreateStackLayoutGroup(UnityEngine::Transform* parent);
} // namespace UIUtils
