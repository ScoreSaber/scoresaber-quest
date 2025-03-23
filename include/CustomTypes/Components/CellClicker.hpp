#pragma once

#include <HMUI/ImageView.hpp>
#include <UnityEngine/Color.hpp>
#include <UnityEngine/EventSystems/IEventSystemHandler.hpp>
#include <UnityEngine/EventSystems/IPointerClickHandler.hpp>
#include <UnityEngine/EventSystems/IPointerEnterHandler.hpp>
#include <UnityEngine/EventSystems/IPointerExitHandler.hpp>
#include <UnityEngine/EventSystems/PointerEventData.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/Vector3.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

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
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, OnPointerClick, &UnityEngine::EventSystems::IPointerClickHandler::OnPointerClick, UnityEngine::EventSystems::PointerEventData* eventData);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, OnPointerEnter, &UnityEngine::EventSystems::IPointerEnterHandler::OnPointerEnter, UnityEngine::EventSystems::PointerEventData* eventData);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, OnPointerExit, &UnityEngine::EventSystems::IPointerExitHandler::OnPointerExit, UnityEngine::EventSystems::PointerEventData* eventData);
                                    DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, seperator);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Vector3, originalScale);
                                    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(UnityEngine::Color, origColour, UnityEngine::Color(1, 1, 1, 1));
                                    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(UnityEngine::Color, origColour0, UnityEngine::Color(1, 1, 1, 0.2509804f));
                                    DECLARE_INSTANCE_FIELD_PRIVATE_DEFAULT(UnityEngine::Color, origColour1, UnityEngine::Color(1, 1, 1, 0));

                                    public:
                                    int index;
                                    std::function<void(int)> onClick;
                                    bool isScaled = false;
)

#undef INTERFACES