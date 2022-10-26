#pragma once

#include "UnityEngine/EventSystems/IPointerClickHandler.hpp"
#include "UnityEngine/EventSystems/PointerEventData.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#define INTERFACES                                                \
    {                                                             \
        classof(UnityEngine::EventSystems::IPointerClickHandler*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::UI::Components, AmeClicker, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, UnityEngine::MonoBehaviour, "ScoreSaber::ReplaySystem::UI::Components", INTERFACES, 0, nullptr,
                                    DECLARE_DEFAULT_CTOR();
                                    DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, _rect);
                                    DECLARE_OVERRIDE_METHOD(void, OnPointerClick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::UnityEngine::EventSystems::IPointerClickHandler::OnPointerClick>::get(), UnityEngine::EventSystems::PointerEventData* eventData);
                                    std::function<void(float)> _clickCallback;
                                    public
                                    : void Setup(std::function<void(float)> callback);)

#undef INTERFACES