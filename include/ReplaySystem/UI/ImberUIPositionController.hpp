#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "Zenject/DiContainer.hpp"

#include "GlobalNamespace/IGamePause.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/PauseMenuManager.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"
#include "ReplaySystem/UI/ImberScrubber.hpp"
#include "ReplaySystem/UI/MainImberPanelView.hpp"
#include "ReplaySystem/UI/VRControllerAccessor.hpp"
#include "System/DateTime.hpp"
#include "System/IDisposable.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/XR/XRNode.hpp"
#include "VRUIControls/VRGraphicRaycaster.hpp"
#include "Zenject/IInitializable.hpp"
#include "Zenject/ITickable.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#define INTERFACES                                                                                       \
    {                                                                                                    \
        classof(Zenject::ITickable*), classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::UI, ImberUIPositionController, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::UI", INTERFACES, 0, nullptr,

                                    DECLARE_INSTANCE_FIELD(bool, _isActive);
                                    DECLARE_INSTANCE_FIELD(bool, _isClicking);
                                    DECLARE_INSTANCE_FIELD(bool, _didClickOnce);
                                    DECLARE_INSTANCE_FIELD(System::DateTime, _lastTriggerDownTime);
                                    DECLARE_INSTANCE_FIELD_DEFAULT(UnityEngine::XR::XRNode, _handTrack, UnityEngine::XR::XRNode::LeftHand);
                                    DECLARE_INSTANCE_FIELD_DEFAULT(float, _senstivityToClick, 0.5f);
                                    DECLARE_INSTANCE_FIELD_DEFAULT(float, _timeBufferToDoubleClick, 0.75f);
                                    DECLARE_INSTANCE_FIELD(GlobalNamespace::IGamePause*, _gamePause);
                                    DECLARE_INSTANCE_FIELD(ReplaySystem::UI::ImberScrubber*, _imberScrubber);
                                    DECLARE_INSTANCE_FIELD(ReplaySystem::UI::MainImberPanelView*, _mainImberPanelView);
                                    DECLARE_INSTANCE_FIELD(ReplaySystem::UI::VRControllerAccessor*, _vrControllerAccessor);
                                    DECLARE_INSTANCE_FIELD(bool, _isPaused);

                                    DECLARE_INSTANCE_FIELD(VRUIControls::VRGraphicRaycaster*, _vrGraphicRaycaster);
                                    DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, _menuControllerTransform);
                                    DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, _menuWrapperTransform);
                                    DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, _pauseMenuManagerTransform);
                                    DECLARE_INSTANCE_FIELD(HMUI::CurvedCanvasSettings*, _curve);
                                    DECLARE_INSTANCE_FIELD(UnityEngine::Canvas*, _canvas);
                                    DECLARE_INSTANCE_FIELD(GlobalNamespace::MainSettingsModelSO*, _mainSettingsModelSO);
                                    DECLARE_INSTANCE_FIELD(UnityEngine::Vector3, _controllerOffset);

                                    DECLARE_CTOR(ctor, GlobalNamespace::IGamePause* gamePause, ReplaySystem::UI::ImberScrubber* imberScrubber, GlobalNamespace::PauseMenuManager* pauseMenuManager, ReplaySystem::UI::MainImberPanelView* mainImberPanelView, ReplaySystem::UI::VRControllerAccessor* vrControllerAccessor);

                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());

                                    DECLARE_INSTANCE_METHOD(void, GamePause_didResumeEvent);
                                    DECLARE_INSTANCE_METHOD(void, GamePause_didPauseEvent);

                                    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());

                                    DECLARE_INSTANCE_METHOD(custom_types::Helpers::Coroutine, KillMe);
                                    DECLARE_INSTANCE_METHOD(void, SetUIPosition, GlobalNamespace::VRController* controller);
                                    DECLARE_INSTANCE_METHOD(void, OpenedUI);
                                    DECLARE_INSTANCE_METHOD(void, UpdateTrackingHand, UnityEngine::XR::XRNode node);
                                    DECLARE_INSTANCE_METHOD(void, SetActiveState, bool state);
                                    DECLARE_INSTANCE_METHOD(void, SetControllerOffset, UnityEngine::Vector3 offset);

                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());

)

#undef INTERFACES
