#include "ReplaySystem/UI/Components/AmeClicker.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/RectTransformUtility.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI::Components, AmeClicker);

namespace ScoreSaber::ReplaySystem::UI::Components
{
    void AmeClicker::Setup(std::function<void(float)> callback)
    {
        _rect = il2cpp_utils::try_cast<RectTransform>(get_transform()).value_or(nullptr);
        _clickCallback = callback;
    }
    void AmeClicker::OnPointerClick(UnityEngine::EventSystems::PointerEventData* eventData)
    {
        Vector2 computedVector;
        RectTransformUtility::ScreenPointToLocalPointInRectangle(_rect, eventData->get_position(), eventData->get_pressEventCamera(), &computedVector);
        float maxX = _rect->get_rect().get_width() / 2.0f;
        _clickCallback(Mathf::InverseLerp(--maxX, maxX, computedVector.x));
    }
} // namespace ScoreSaber::ReplaySystem::UI::Components