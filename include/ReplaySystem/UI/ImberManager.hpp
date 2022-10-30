#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "Zenject/DiContainer.hpp"

#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/AudioTimeSyncController_InitData.hpp"
#include "GlobalNamespace/IGamePause.hpp"
#include "ReplaySystem/Playback/PosePlayer.hpp"
#include "ReplaySystem/Playback/ReplayTimeSyncController.hpp"
#include "ReplaySystem/UI/ImberScrubber.hpp"
#include "ReplaySystem/UI/ImberSpecsReporter.hpp"
#include "ReplaySystem/UI/ImberUIPositionController.hpp"
#include "ReplaySystem/UI/MainImberPanelView.hpp"
#include "ReplaySystem/UI/SpectateAreaController.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/XR/XRNode.hpp"

#include "Zenject/IInitializable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#define INTERFACES                                                         \
    {                                                                      \
        classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::UI, ImberManager, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::UI", INTERFACES, 0, nullptr,

                                    DECLARE_INSTANCE_FIELD(float, _initialTimeScale);
                                    DECLARE_INSTANCE_FIELD(GlobalNamespace::IGamePause*, _gamePause);
                                    DECLARE_INSTANCE_FIELD(ScoreSaber::ReplaySystem::UI::ImberScrubber*, _imberScrubber);
                                    DECLARE_INSTANCE_FIELD(ScoreSaber::ReplaySystem::UI::ImberSpecsReporter*, _imberSpecsReporter);
                                    DECLARE_INSTANCE_FIELD(ScoreSaber::ReplaySystem::UI::MainImberPanelView*, _mainImberPanelView);
                                    DECLARE_INSTANCE_FIELD(ScoreSaber::ReplaySystem::UI::SpectateAreaController*, _spectateAreaController);
                                    DECLARE_INSTANCE_FIELD(GlobalNamespace::AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD(ScoreSaber::ReplaySystem::Playback::ReplayTimeSyncController*, _replayTimeSyncController);
                                    DECLARE_INSTANCE_FIELD(ScoreSaber::ReplaySystem::UI::ImberUIPositionController*, _imberUIPositionController);
                                    DECLARE_INSTANCE_FIELD(GlobalNamespace::AudioTimeSyncController::InitData*, _initData);
                                    DECLARE_INSTANCE_FIELD(ScoreSaber::ReplaySystem::Playback::PosePlayer*, _posePlayer);

                                    DECLARE_CTOR(ctor, GlobalNamespace::IGamePause* gamePause,
                                                 ScoreSaber::ReplaySystem::UI::ImberScrubber* imberScrubber,
                                                 ScoreSaber::ReplaySystem::UI::ImberSpecsReporter* imberSpecsReporter,
                                                 ScoreSaber::ReplaySystem::UI::MainImberPanelView* mainImberPanelView,
                                                 ScoreSaber::ReplaySystem::UI::SpectateAreaController* spectateAreaController,
                                                 GlobalNamespace::AudioTimeSyncController* audioTimeSyncController,
                                                 ScoreSaber::ReplaySystem::Playback::ReplayTimeSyncController* replayTimeSyncController,
                                                 ScoreSaber::ReplaySystem::UI::ImberUIPositionController* imberUIPositionController,
                                                 GlobalNamespace::AudioTimeSyncController::InitData* initData,
                                                 ScoreSaber::ReplaySystem::Playback::PosePlayer* posePlayer);

                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());

                                    DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidHandSwitchEvent, UnityEngine::XR::XRNode hand);
                                    DECLARE_INSTANCE_METHOD(void, GamePause_didResumeEvent);
                                    DECLARE_INSTANCE_METHOD(void, ImberSpecsReporter_DidReport, int fps, float leftSaberSpeed, float rightSaberSpeed);
                                    DECLARE_INSTANCE_METHOD(void, SpectateAreaController_DidUpdatePlayerSpectatorPose, UnityEngine::Vector3 position, UnityEngine::Quaternion rotation);
                                    DECLARE_INSTANCE_METHOD(void, CreateWatermark);

                                    // UI Callbacks
                                    DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidPositionTabVisibilityChange, bool value);
                                    DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidPositionPreviewChange, StringW value);
                                    DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidPositionJump);
                                    DECLARE_INSTANCE_METHOD(void, ImberScrubber_DidCalculateNewTime, float time);
                                    DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidClickLoop);
                                    // DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidClickLogo);
                                    DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidClickRestart);
                                    DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidClickPausePlay);
                                    DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidTimeSyncChange, float value);
                                    DECLARE_INSTANCE_METHOD(void, MainImberPanelView_DidChangeVisibility, bool value);

                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    System::Action * _didResumeDelegate;

)

#undef INTERFACES
