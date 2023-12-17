#include "CustomTypes/Components/CellClicker.hpp"

#include "GlobalNamespace/MenuShockwave.hpp"
#include "GlobalNamespace/Signal.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Transform.hpp"

#include "bsml/shared/BSML.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "questui/shared/QuestUI.hpp"

#include "logging.hpp"

using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::EventSystems;

DEFINE_TYPE(ScoreSaber::CustomTypes::Components, CellClicker);

namespace ScoreSaber::CustomTypes::Components {
    // copied from BSML to get the button click sound
    GlobalNamespace::Signal* get_imageClickedSignal() {
        static SafePtrUnity<GlobalNamespace::Signal> imageClickedSignal;
        if (!imageClickedSignal) {
            auto menuShockWave = Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuShockwave*>().FirstOrDefault();
            imageClickedSignal = menuShockWave ? menuShockWave->buttonClickEvents.LastOrDefault() : nullptr;
        }
        return imageClickedSignal.ptr();
    }

    void CellClicker::Start() {
        originalScale = seperator->get_transform()->get_localScale();
    }

    void CellClicker::OnPointerClick(PointerEventData* data) {
        // simulate BeatSaberUI.BasicUIAudioManager.HandleButtonClickEvent();
        auto signal = get_imageClickedSignal();
        if(signal) signal->Raise();

        if (onClick != nullptr) {
            onClick(index);
        }
    }

    custom_types::Helpers::Coroutine LerpColors(ImageView* target, Color startColor, Color endColor, Color startColor0, Color endColor0, Color startColor1, Color endColor1, float duration) {
        float elapsedTime = 0.0f;
        while (elapsedTime < duration) {
            float t = elapsedTime / duration;
            target->set_color(Color::Lerp(startColor, endColor, t));
            target->set_color0(Color::Lerp(startColor0, endColor0, t));
            target->set_color1(Color::Lerp(startColor1, endColor1, t));
            elapsedTime += Time::get_deltaTime();
            co_yield nullptr;
        }
        target->set_color(endColor);
        target->set_color0(endColor0);
        target->set_color1(endColor1);
        co_return;
    }

    void CellClicker::OnPointerEnter(PointerEventData* eventData) {
        if (!isScaled) {
            seperator->get_transform()->set_localScale(originalScale * 1.8f);
            isScaled = true;
        }

        Color targetColor = Color::get_white();
        Color targetColor0 = Color::get_white();
        Color targetColor1 = Color(1, 1, 1, 0);

        float lerpDuration = 0.15f;

        StopAllCoroutines();
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LerpColors(seperator, seperator->get_color(), targetColor, seperator->color0, targetColor0, seperator->color1, targetColor1, lerpDuration)));
    }

    void CellClicker::OnPointerExit(PointerEventData* eventData) {
        if (isScaled) {
            seperator->get_transform()->set_localScale(originalScale);
            isScaled = false;
        }

        float lerpDuration = 0.05f;

        StopAllCoroutines();
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LerpColors(seperator, seperator->get_color(), origColour, seperator->color0, origColour0, seperator->color1, origColour1, lerpDuration)));
    }

    void CellClicker::OnDestroy() {
        StopAllCoroutines();
        onClick = nullptr;
        seperator->set_color(origColour);
        seperator->set_color0(origColour0);
        seperator->set_color1(origColour1);
    }
}