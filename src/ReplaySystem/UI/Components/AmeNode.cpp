#include "ReplaySystem/UI/Components/AmeNode.hpp"
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/RectTransformUtility.hpp>
#include <UnityEngine/Vector2.hpp>

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI::Components, AmeNode);

namespace ScoreSaber::ReplaySystem::UI::Components
{
    bool AmeNode::isBeingDragged
    {
        return _handle->dragged;
    }
    void AmeNode::Init(ScoreSaber::ReplaySystem::UI::Components::AmeHandle* handle)
    {
        _handle = handle;
    }
    void AmeNode::AddCallback(std::function<void(AmeNode*, Vector2, Camera*)> callback)
    {
        _callback = callback;
        _handle->AddCallback([=](ScoreSaber::ReplaySystem::UI::Components::AmeHandle* handle, UnityEngine::Vector2 x, UnityEngine::Camera* camera) {
            Callback(handle, x, camera);
        });
    }
    void AmeNode::Callback(ScoreSaber::ReplaySystem::UI::Components::AmeHandle* handle, UnityEngine::Vector2 x, UnityEngine::Camera* camera)
    {
        if (moveable)
        {
            if (_callback != nullptr)
            {
                _callback(this, x, camera);
            }
        }
    }
    void AmeNode::SendUpdatePositionCall(float percentOnBar)
    {
        if (PositionDidChange != nullptr)
        {
            PositionDidChange(percentOnBar);
        }
    }
} // namespace ScoreSaber::ReplaySystem::UI::Components