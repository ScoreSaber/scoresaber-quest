#pragma once

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

using namespace UnityEngine;
namespace UIUtils
{
    UnityEngine::UI::HorizontalLayoutGroup* CreateTeamMemberLayout(UnityEngine::Transform* parent, TeamUtils::TeamMember& member);
    UnityEngine::GameObject* CreateLoadingIndicator(UnityEngine::Transform* parent);
    UnityEngine::UI::HorizontalLayoutGroup* CreateHeader(UnityEngine::Transform* parent, UnityEngine::Color color);
    UnityEngine::UI::HorizontalLayoutGroup* CreateHeader(
        UnityEngine::Transform* parent, UnityEngine::Color color);

    ScoreSaber::CustomTypes::Components::ImageButton* CreateImageButton(
        GameObject* parent, Sprite* sprite, Vector2 anchoredPosition,
        Vector2 sizeDelta, std::function<void()> onClick);
} // namespace UIUtils
