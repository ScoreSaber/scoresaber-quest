#include "CustomTypes/Components/CellClicker.hpp"

#include <GlobalNamespace/MenuShockwave.hpp>
#include <GlobalNamespace/Signal.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Time.hpp>
#include <UnityEngine/Transform.hpp>
#include <UnityEngine/Vector3.hpp>

#include <custom-types/shared/coroutine.hpp>
#include <beatsaber-hook/shared/utils/typedefs-wrappers.hpp>
#include "Utils/OperatorOverloads.hpp"

using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::EventSystems;

DEFINE_TYPE(ScoreSaber::CustomTypes::Components, CellClicker);


namespace ScoreSaber::CustomTypes::Components {
    // copied from BSML to get the button click sound
    GlobalNamespace::Signal* getImageClickedSignal() {
        static SafePtrUnity<GlobalNamespace::Signal> imageClickedSignal;
        if (!imageClickedSignal) {
            auto menuShockWave = Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuShockwave*>().front_or_default();
            imageClickedSignal = menuShockWave ? menuShockWave->_buttonClickEvents.back_or_default() : nullptr;
        }
        return imageClickedSignal.ptr();
    }

    void CellClicker::Start() {
        originalScale = seperator->transform->localScale;
    }

    void CellClicker::OnPointerClick(PointerEventData* data) {
        // simulate BeatSaberUI.BasicUIAudioManager.HandleButtonClickEvent();
        auto signal = getImageClickedSignal();
        if(signal) signal->Raise();

        if (onClick != nullptr) {
            onClick(index);
        }
    }

    custom_types::Helpers::Coroutine LerpColors(ImageView* target, Color startColor, Color endColor, Color startColor0, Color endColor0, Color startColor1, Color endColor1, float duration) {
        float elapsedTime = 0.0f;
        while (elapsedTime < duration) {
            float t = elapsedTime / duration;
            target->color = Color::Lerp(startColor, endColor, t);
            target->color0 = Color::Lerp(startColor0, endColor0, t);
            target->color1 = Color::Lerp(startColor1, endColor1, t);
            elapsedTime += Time::get_deltaTime();
            co_yield nullptr;
        }
        target->color = endColor;
        target->color0 = endColor0;
        target->color1 = endColor1;
        co_return;
    }

    void CellClicker::OnPointerEnter(PointerEventData* eventData) {
        if (!isScaled) {
            //seperator->transform->localScale = Vector3::op_Multiply(originalScale, 1.8f);
            seperator->transform->localScale = originalScale * 1.8f;
            isScaled = true;
        }

        Color targetColor = Color::get_white();
        Color targetColor0 = Color::get_white();
        Color targetColor1 = Color(1, 1, 1, 0);

        float lerpDuration = 0.15f;

        StopAllCoroutines();
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LerpColors(seperator, seperator->color, targetColor, seperator->color0, targetColor0, seperator->color1, targetColor1, lerpDuration)));
    }

    void CellClicker::OnPointerExit(PointerEventData* eventData) {
        if (isScaled) {
            seperator->transform->localScale = originalScale;
            isScaled = false;
        }

        float lerpDuration = 0.05f;

        StopAllCoroutines();
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LerpColors(seperator, seperator->color, origColour, seperator->color0, origColour0, seperator->color1, origColour1, lerpDuration)));
    }

    void CellClicker::OnDestroy() {
        StopAllCoroutines();
        onClick = nullptr;
        seperator->color = origColour;
        seperator->color0 = origColour0;
        seperator->color1 = origColour1;
    }
}