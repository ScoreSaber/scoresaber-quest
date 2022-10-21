#pragma once

#include "UnityEngine/Camera.hpp"
#include "UnityEngine/EventSystems/IBeginDragHandler.hpp"
#include "UnityEngine/EventSystems/IDragHandler.hpp"
#include "UnityEngine/EventSystems/IEndDragHandler.hpp"
#include "UnityEngine/EventSystems/IEventSystemHandler.hpp"
#include "UnityEngine/EventSystems/IInitializePotentialDragHandler.hpp"
#include "UnityEngine/EventSystems/PointerEventData.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Vector2.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#define INTERFACES                                                                                                                                                                                                                                                                                      \
    {                                                                                                                                                                                                                                                                                                   \
        classof(UnityEngine::EventSystems::IBeginDragHandler*), classof(UnityEngine::EventSystems::IDragHandler*), classof(UnityEngine::EventSystems::IEventSystemHandler*), classof(UnityEngine::EventSystems::IInitializePotentialDragHandler*), classof(UnityEngine::EventSystems::IEndDragHandler*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::UI::Components, AmeHandle, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, UnityEngine::MonoBehaviour, "ScoreSaber::ReplaySystem::UI", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD(bool, dragged);
                                    DECLARE_OVERRIDE_METHOD(void, OnBeginDrag, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::UnityEngine::EventSystems::IBeginDragHandler::OnBeginDrag>::get(), UnityEngine::EventSystems::PointerEventData* eventData);
                                    DECLARE_OVERRIDE_METHOD(void, OnInitializePotentialDrag, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::UnityEngine::EventSystems::IInitializePotentialDragHandler::OnInitializePotentialDrag>::get(), UnityEngine::EventSystems::PointerEventData* eventData);
                                    DECLARE_OVERRIDE_METHOD(void, OnDrag, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::UnityEngine::EventSystems::IDragHandler::OnDrag>::get(), UnityEngine::EventSystems::PointerEventData* eventData);
                                    DECLARE_OVERRIDE_METHOD(void, OnEndDrag, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::UnityEngine::EventSystems::IEndDragHandler::OnEndDrag>::get(), UnityEngine::EventSystems::PointerEventData* eventData);
                                    public
                                    : void AddCallback(std::function<void(ScoreSaber::ReplaySystem::UI::Components::AmeHandle*, UnityEngine::Vector2, UnityEngine::Camera*)> callback);
                                    std::function<void(ScoreSaber::ReplaySystem::UI::Components::AmeHandle*, UnityEngine::Vector2, UnityEngine::Camera*)> _handleMoveCallback;
                                    )
#undef INTERFACES