#pragma once

#include <UnityEngine/Camera.hpp>
#include <UnityEngine/EventSystems/IBeginDragHandler.hpp>
#include <UnityEngine/EventSystems/IDragHandler.hpp>
#include <UnityEngine/EventSystems/IEndDragHandler.hpp>
#include <UnityEngine/EventSystems/IEventSystemHandler.hpp>
#include <UnityEngine/EventSystems/IInitializePotentialDragHandler.hpp>
#include <UnityEngine/EventSystems/PointerEventData.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/RectTransform.hpp>
#include <UnityEngine/Vector2.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::ReplaySystem::UI::Components,
        AmeHandle,
        UnityEngine::MonoBehaviour,
        UnityEngine::EventSystems::IBeginDragHandler*,
        UnityEngine::EventSystems::IDragHandler*,
        UnityEngine::EventSystems::IEventSystemHandler*,
        UnityEngine::EventSystems::IInitializePotentialDragHandler*,
        UnityEngine::EventSystems::IEndDragHandler*) {
    DECLARE_INSTANCE_FIELD(bool, dragged);
    DECLARE_OVERRIDE_METHOD_MATCH(void, OnBeginDrag, &::UnityEngine::EventSystems::IBeginDragHandler::OnBeginDrag, UnityEngine::EventSystems::PointerEventData* eventData);
    DECLARE_OVERRIDE_METHOD_MATCH(void, OnInitializePotentialDrag, &::UnityEngine::EventSystems::IInitializePotentialDragHandler::OnInitializePotentialDrag, UnityEngine::EventSystems::PointerEventData* eventData);
    DECLARE_OVERRIDE_METHOD_MATCH(void, OnDrag, &::UnityEngine::EventSystems::IDragHandler::OnDrag, UnityEngine::EventSystems::PointerEventData* eventData);
    DECLARE_OVERRIDE_METHOD_MATCH(void, OnEndDrag, &::UnityEngine::EventSystems::IEndDragHandler::OnEndDrag, UnityEngine::EventSystems::PointerEventData* eventData);
    DECLARE_DEFAULT_CTOR();
public:
    void AddCallback(std::function<void(ScoreSaber::ReplaySystem::UI::Components::AmeHandle*, UnityEngine::Vector2, UnityEngine::Camera*)> callback);
    std::function<void(ScoreSaber::ReplaySystem::UI::Components::AmeHandle*, UnityEngine::Vector2, UnityEngine::Camera*)> _handleMoveCallback;
};