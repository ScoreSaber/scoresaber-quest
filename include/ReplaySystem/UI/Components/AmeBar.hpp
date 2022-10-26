#pragma once

#include "HMUI/CurvedTextMeshPro.hpp"
#include "ReplaySystem/UI/Components/AmeNode.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Vector2.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(ScoreSaber::ReplaySystem::UI::Components, AmeBar, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, _rectTransform);
                      DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, _fillBarTransform);
                      DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, _endTimeText);
                      DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, _currentTimeText);
                      DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, _otherTransform);
                      DECLARE_INSTANCE_FIELD(UnityEngine::Material*, _mainUIFontMaterial);
                      DECLARE_INSTANCE_METHOD(void, Setup, UnityEngine::RectTransform* fillBarTransform, UnityEngine::RectTransform* otherTransform);
                      DECLARE_INSTANCE_METHOD(void, RegisterNode, ScoreSaber::ReplaySystem::UI::Components::AmeNode* node);
                      DECLARE_INSTANCE_METHOD(void, UnregisterNode, ScoreSaber::ReplaySystem::UI::Components::AmeNode* node);
                      DECLARE_INSTANCE_METHOD(float, GetNodePercent, ScoreSaber::ReplaySystem::UI::Components::AmeNode* node);
                      DECLARE_INSTANCE_METHOD(void, AssignNodeToPercent, ScoreSaber::ReplaySystem::UI::Components::AmeNode* node, float percent);
                      DECLARE_INSTANCE_METHOD(void, set_currentTime, float value);
                      DECLARE_INSTANCE_METHOD(void, set_endTime, float value);
                      DECLARE_INSTANCE_METHOD(void, set_barFill, float value);
                      DECLARE_INSTANCE_METHOD(float, get_barFill);
                      DECLARE_INSTANCE_METHOD(float, XForPercent, float percent);
                      DECLARE_INSTANCE_METHOD(float, PercentForX, float x);
                      DECLARE_DEFAULT_CTOR();
                      void DragCallback(ScoreSaber::ReplaySystem::UI::Components::AmeNode* node, UnityEngine::Vector2 x, UnityEngine::Camera* camera);
                      HMUI::CurvedTextMeshPro * CreateText();)