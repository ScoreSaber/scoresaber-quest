#pragma once

#include "HMUI/ImageView.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/EventSystems/IEventSystemHandler.hpp"
#include "UnityEngine/EventSystems/IPointerClickHandler.hpp"
#include "UnityEngine/EventSystems/IPointerEnterHandler.hpp"
#include "UnityEngine/EventSystems/IPointerExitHandler.hpp"
#include "UnityEngine/EventSystems/PointerEventData.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Vector3.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include <functional>

#define INTERFACES                                                 \
    {                                                              \
        classof(UnityEngine::EventSystems::IPointerClickHandler*), \
        classof(UnityEngine::EventSystems::IPointerEnterHandler*), \
        classof(UnityEngine::EventSystems::IPointerExitHandler*),  \
        classof(UnityEngine::EventSystems::IEventSystemHandler*)   \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::CustomTypes::Components, CellClicker, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, UnityEngine::MonoBehaviour, "ScoreSaber::CustomTypes::Components", INTERFACES, 0, nullptr,
                                    DECLARE_DEFAULT_CTOR();
                                    DECLARE_PRIVATE_METHOD(void, Start);
                                    DECLARE_PRIVATE_METHOD(void, OnDestroy);
                                    DECLARE_OVERRIDE_METHOD(void, OnPointerClick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IPointerClickHandler::OnPointerClick>::get(), UnityEngine::EventSystems::PointerEventData* eventData);
                                    DECLARE_OVERRIDE_METHOD(void, OnPointerEnter, il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IPointerEnterHandler::OnPointerEnter>::get(), UnityEngine::EventSystems::PointerEventData* eventData);
                                    DECLARE_OVERRIDE_METHOD(void, OnPointerExit, il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IPointerExitHandler::OnPointerExit>::get(), UnityEngine::EventSystems::PointerEventData* eventData);
                                    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, seperator);
                                    DECLARE_PRIVATE_FIELD(UnityEngine::Vector3, originalScale);
                                    DECLARE_PRIVATE_FIELD_DEFAULT(UnityEngine::Color, origColour, UnityEngine::Color(1, 1, 1, 1));
                                    DECLARE_PRIVATE_FIELD_DEFAULT(UnityEngine::Color, origColour0, UnityEngine::Color(1, 1, 1, 0.2509804f));
                                    DECLARE_PRIVATE_FIELD_DEFAULT(UnityEngine::Color, origColour1, UnityEngine::Color(1, 1, 1, 0));

                                    public:
                                    int index;
                                    std::function<void(int)> onClick;
                                    bool isScaled = false;
)

#undef INTERFACES