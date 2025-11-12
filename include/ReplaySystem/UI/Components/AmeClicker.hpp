#pragma once

#include <UnityEngine/EventSystems/IPointerClickHandler.hpp>
#include <UnityEngine/EventSystems/PointerEventData.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/RectTransform.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::ReplaySystem::UI::Components,
        AmeClicker,
        UnityEngine::MonoBehaviour,
        UnityEngine::EventSystems::IPointerClickHandler*) {
    DECLARE_DEFAULT_CTOR();
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::RectTransform>, _rect);
    DECLARE_OVERRIDE_METHOD_MATCH(void, OnPointerClick, &UnityEngine::EventSystems::IPointerClickHandler::OnPointerClick, UnityEngine::EventSystems::PointerEventData* eventData);
    std::function<void(float)> _clickCallback;
public:
    void Setup(std::function<void(float)> callback);
};