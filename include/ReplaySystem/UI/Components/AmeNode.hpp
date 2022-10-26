#pragma once

#include "ReplaySystem/UI/Components/AmeHandle.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Vector2.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(ScoreSaber::ReplaySystem::UI::Components, AmeNode, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, _rectTransform);
                      DECLARE_INSTANCE_FIELD_DEFAULT(bool, moveable, true);
                      DECLARE_INSTANCE_FIELD_DEFAULT(float, min, 0.0f);
                      DECLARE_INSTANCE_FIELD_DEFAULT(float, max, 1.0f);
                      DECLARE_INSTANCE_METHOD(bool, get_isBeingDragged);
                      DECLARE_INSTANCE_METHOD(void, Init, ScoreSaber::ReplaySystem::UI::Components::AmeHandle* handle);
                      DECLARE_INSTANCE_METHOD(void, Callback, ScoreSaber::ReplaySystem::UI::Components::AmeHandle* handle, UnityEngine::Vector2 x, UnityEngine::Camera* camera);
                      DECLARE_INSTANCE_METHOD(void, SendUpdatePositionCall, float percentOnBar);
                      DECLARE_DEFAULT_CTOR();
                      ScoreSaber::ReplaySystem::UI::Components::AmeHandle * _handle;
                      std::function<void(ScoreSaber::ReplaySystem::UI::Components::AmeNode*, UnityEngine::Vector2, UnityEngine::Camera*)> _callback;
                      public
                      : void AddCallback(std::function<void(ScoreSaber::ReplaySystem::UI::Components::AmeNode*, UnityEngine::Vector2, UnityEngine::Camera*)> callback);
                      std::function<void(float)> PositionDidChange;)