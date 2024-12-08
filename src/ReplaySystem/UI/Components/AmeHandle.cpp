#include "ReplaySystem/UI/Components/AmeHandle.hpp"
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/RectTransformUtility.hpp>
#include <UnityEngine/Vector2.hpp>
#include "logging.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI::Components, AmeHandle);

namespace ScoreSaber::ReplaySystem::UI::Components
{
    void AmeHandle::AddCallback(std::function<void(AmeHandle*, Vector2, Camera*)> callback)
    {
        _handleMoveCallback = callback;
    }
    void AmeHandle::OnBeginDrag(UnityEngine::EventSystems::PointerEventData* eventData)
    {
        dragged = true;
    }
    void AmeHandle::OnInitializePotentialDrag(UnityEngine::EventSystems::PointerEventData* eventData)
    {
        dragged = true;
        eventData->useDragThreshold = false;
    }
    void AmeHandle::OnDrag(UnityEngine::EventSystems::PointerEventData* eventData)
    {
        if (eventData->position.x == 0.0f)
        {
            return;
        }
        if (_handleMoveCallback != nullptr)
        {
            _handleMoveCallback(this, eventData->position, eventData->pressEventCamera);
        }
    }

    void AmeHandle::OnEndDrag(UnityEngine::EventSystems::PointerEventData* eventData)
    {
        dragged = false;
    }
} // namespace ScoreSaber::ReplaySystem::UI::Components