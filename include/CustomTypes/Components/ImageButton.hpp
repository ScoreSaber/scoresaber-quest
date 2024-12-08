#pragma once

#include <UnityEngine/Collider.hpp>
#include <UnityEngine/MeshRenderer.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/UI/Button.hpp>
#include <VRUIControls/VRPointer.hpp>
#include <custom-types/shared/macros.hpp>
#include <custom-types/shared/register.hpp>
#include <custom-types/shared/util.hpp>

DECLARE_CLASS_CODEGEN(
    ScoreSaber::CustomTypes::Components, ImageButton,
    UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(ArrayW<VRUIControls::VRPointer*>, pointerArray);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, sprite);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, button);
    DECLARE_INSTANCE_FIELD(float, r); DECLARE_INSTANCE_FIELD(float, g);
    DECLARE_INSTANCE_FIELD(float, b); DECLARE_INSTANCE_FIELD(float, a); public
    : void Init(UnityEngine::Transform* t, UnityEngine::Vector2 v,
                UnityEngine::Vector2 v1, std::function<void()> f);)