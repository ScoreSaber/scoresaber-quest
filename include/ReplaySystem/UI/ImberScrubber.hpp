#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/MainCamera.hpp>
#include <HMUI/ImageView.hpp>
#include "ReplaySystem/UI/Components/AmeBar.hpp"
#include "ReplaySystem/UI/Components/AmeNode.hpp"

#include <Zenject/DiContainer.hpp>

#include <System/IDisposable.hpp>
#include <UnityEngine/Color.hpp>
#include <UnityEngine/RectTransform.hpp>
#include <UnityEngine/Transform.hpp>
#include <Zenject/IInitializable.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

#define INTERFACES                                                                                       \
    {                                                                                                    \
        classof(Zenject::ITickable*), classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::UI, ImberScrubber, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::UI", INTERFACES, 0, nullptr,
                                    // DidCalculateNewTime is at the eof

                                    DECLARE_INSTANCE_FIELD(UnityEngine::Color, _scoreSaberBlue);
                                    DECLARE_INSTANCE_METHOD(UnityEngine::Transform*, get_transform);

                                    // loopMode
                                    DECLARE_INSTANCE_FIELD(bool, _loopMode);
                                    DECLARE_INSTANCE_METHOD(void, set_loopMode, bool value);

                                    DECLARE_INSTANCE_METHOD(void, set_visibility, bool value);

                                    // mainNodeValue
                                    DECLARE_INSTANCE_METHOD(float, get_mainNodeValue);
                                    DECLARE_INSTANCE_METHOD(void, set_mainNodeValue, float value);

                                    // "private" fields

                                    DECLARE_INSTANCE_FIELD(UnityW<ScoreSaber::ReplaySystem::UI::Components::AmeBar>, _bar);
                                    DECLARE_INSTANCE_FIELD(UnityW<ScoreSaber::ReplaySystem::UI::Components::AmeNode>, _mainNode);
                                    DECLARE_INSTANCE_FIELD(UnityW<ScoreSaber::ReplaySystem::UI::Components::AmeNode>, _loopNode);
                                    DECLARE_INSTANCE_FIELD(UnityW<ScoreSaber::ReplaySystem::UI::Components::AmeNode>, _failNode);
                                    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Transform>, _parent);
                                    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::MainCamera>, _mainCamera);
                                    DECLARE_INSTANCE_FIELD(Zenject::DiContainer*, _container);
                                    DECLARE_INSTANCE_FIELD_DEFAULT(float, _minNodeDistance, 0.01f);
                                    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD_DEFAULT(float, _levelFailTime, 0.0f);
                                    DECLARE_INSTANCE_FIELD_DEFAULT(float, _maxPercent, 1.0f);
                                    DECLARE_INSTANCE_FIELD(bool, _allowPast);

                                    DECLARE_CTOR(ctor, GlobalNamespace::MainCamera* mainCamera, Zenject::DiContainer* container, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController);

                                    DECLARE_INSTANCE_METHOD(void, Setup, float levelFailTime, bool allowPast);

                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_INSTANCE_METHOD(void, MainNode_PositionDidChange, float value);
                                    DECLARE_INSTANCE_METHOD(void, LoopNode_PositionDidChange, float value);

                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);

                                    DECLARE_INSTANCE_METHOD(ScoreSaber::ReplaySystem::UI::Components::AmeBar*, Create, UnityEngine::Camera* camera, UnityEngine::Vector2 size);
                                    DECLARE_INSTANCE_METHOD(void, ClickedBackground, float value);
                                    DECLARE_INSTANCE_METHOD(HMUI::ImageView*, CreateImage, UnityEngine::RectTransform* transform);
                                    DECLARE_INSTANCE_METHOD(ScoreSaber::ReplaySystem::UI::Components::AmeNode*, CreateSlideNode, UnityEngine::RectTransform* transform);
                                    DECLARE_INSTANCE_METHOD(ScoreSaber::ReplaySystem::UI::Components::AmeNode*, CreateTextNode, UnityEngine::RectTransform* transform, StringW initialText, UnityEngine::Color color);

                                    public:
                                    std::function<void(float)> DidCalculateNewTime;

)

#undef INTERFACES
