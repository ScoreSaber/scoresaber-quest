#include "CustomTypes/Components/ClickableImage.hpp"
#include <VRUIControls/VRPointer.hpp>
#include "logging.hpp"
DEFINE_TYPE(ScoreSaber::CustomTypes::Components, ClickableImage);

using namespace UnityEngine;

#define LOG_COLOR(color) INFO("%s: %.2f, %.2f, %.2f, %.2f", #color, color.r, color.g, color.b, color.a)
namespace ScoreSaber::CustomTypes::Components
{
    void ClickableImage::ctor()
    {
        INVOKE_CTOR();
        onClickEvent = OnPointerClickEvent();
        pointerEnterEvent = OnPointerEnterEvent();
        pointerExitEvent = OnPointerExitEvent();
        isHighlighted = false;
        highlightColor = UnityEngine::Color(0.60f, 0.80f, 1.0f, 1.0f);
        defaultColor = UnityEngine::Color(1.0f, 1.0f, 1.0f, 1.0f);

        static auto base_ctor = il2cpp_utils::FindMethod(classof(HMUI::ImageView*), ".ctor");
        if (base_ctor)
        {
            il2cpp_utils::RunMethod(this, base_ctor);
        }
    }

    void ClickableImage::OnPointerClick(EventSystems::PointerEventData* eventData)
    {
        isHighlighted = false;
        if (buttonClickedSignal) buttonClickedSignal->Raise();
        onClickEvent.invoke(eventData);
    }

    void ClickableImage::OnPointerEnter(EventSystems::PointerEventData* eventData)
    {
        isHighlighted = true;
        pointerEnterEvent.invoke(eventData);
        Vibrate(!VRUIControls::VRPointer::____lastSelectedControllerWasRight);
    }

    void ClickableImage::OnPointerExit(EventSystems::PointerEventData* eventData)
    {
        isHighlighted = false;
        pointerExitEvent.invoke(eventData);
    }

    void ClickableImage::Vibrate(bool left)
    {
        UnityEngine::XR::XRNode node = left ? UnityEngine::XR::XRNode::LeftHand : UnityEngine::XR::XRNode::RightHand;
        hapticFeedbackManager->PlayHapticFeedback(node, hapticFeedbackPresetSO);
    }

    void ClickableImage::UpdateHighlight()
    {
        color = isHighlighted ? highlightColor : defaultColor;
    }

    void ClickableImage::highlightColor = UnityEngine::Color color
    {
        highlightColor = color;
        UpdateHighlight();
    }

    UnityEngine::Color ClickableImage::highlightColor
    {
        return highlightColor;
    }

    void ClickableImage::defaultColor = UnityEngine::Color color
    {
        defaultColor = color;
        UpdateHighlight();
    }

    UnityEngine::Color ClickableImage::defaultColor
    {
        return defaultColor;
    }

    bool ClickableImage::isHighlighted
    {
        return isHighlighted;
    }

    void ClickableImage::isHighlighted = bool value
    {
        isHighlighted = value;
        UpdateHighlight();
    }

    ClickableImage::OnPointerClickEvent& ClickableImage::onPointerClickEvent
    {
        return onClickEvent;
    }

    ClickableImage::OnPointerEnterEvent& ClickableImage::onPointerEnterEvent
    {
        return pointerEnterEvent;
    }

    ClickableImage::OnPointerExitEvent& ClickableImage::onPointerExitEvent
    {
        return pointerExitEvent;
    }
}