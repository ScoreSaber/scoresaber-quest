#include "ReplaySystem/UI/ImberManager.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/RectTransformUtility.hpp"
#include "UnityEngine/Vector2.hpp"
#include "Utils/StringUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, ImberManager);

namespace ScoreSaber::ReplaySystem::UI
{
    void ImberManager::ctor(GlobalNamespace::IGamePause* gamePause,
                            ScoreSaber::ReplaySystem::UI::ImberScrubber* imberScrubber,
                            ScoreSaber::ReplaySystem::UI::ImberSpecsReporter* imberSpecsReporter,
                            ScoreSaber::ReplaySystem::UI::MainImberPanelView* mainImberPanelView,
                            ScoreSaber::ReplaySystem::UI::SpectateAreaController* spectateAreaController,
                            GlobalNamespace::AudioTimeSyncController* audioTimeSyncController,
                            ScoreSaber::ReplaySystem::Playback::ReplayTimeSyncController* replayTimeSyncController,
                            ScoreSaber::ReplaySystem::UI::ImberUIPositionController* imberUIPositionController,
                            GlobalNamespace::AudioTimeSyncController::InitData* initData,
                            ScoreSaber::ReplaySystem::Playback::PosePlayer* posePlayer)
    {
        _gamePause = gamePause;
        _imberScrubber = imberScrubber;
        _imberSpecsReporter = imberSpecsReporter;
        _mainImberPanelView = mainImberPanelView;
        _spectateAreaController = spectateAreaController;
        _audioTimeSyncController = audioTimeSyncController;
        _replayTimeSyncController = replayTimeSyncController;
        _imberUIPositionController = imberUIPositionController;
        _initData = initData;
        _posePlayer = posePlayer;

        // TODO: Load positions

        _mainImberPanelView->Setup(initData->timeScale, 90, "default", std::vector<string>());

        bool noFail = false;
        for (int i = 0; i < ReplayLoader::LoadedReplay->metadata->Modifiers.size(); i++)
        {
            if (StringUtils::Contains(ReplayLoader::LoadedReplay->metadata->Modifiers[i], "NF"))
            {
                noFail = true;
            }
        }

        _imberScrubber->Setup(ReplayLoader::LoadedReplay->metadata->FailTime, noFail);
        _initialTimeScale = ReplayLoader::LoadedReplay->noteKeyframes[0].TimeSyncTimescale;
    }

    void ImberManager::Initialize()
    {
        // TODO: Sub to events
    }

    void ImberManager::MainImberPanelView_DidHandSwitchEvent(XR::XRNode hand)
    {
        // TODO: Update hand settings in config

        _imberUIPositionController->UpdateTrackingHand(hand);
    }

    void ImberManager::GamePause_didResumeEvent()
    {
        // _mainImberPanelView->set_playPauseText("PAUSE");
    }

    void ImberManager::ImberSpecsReporter_DidReport(int fps, float leftSaberSpeed, float rightSaberSpeed)
    {
        if (_mainImberPanelView->get_didParse())
        {
            _mainImberPanelView->set_fps(fps);
            _mainImberPanelView->set_leftSaberSpeed(leftSaberSpeed * (_initialTimeScale / _audioTimeSyncController->get_timeScale()));
            _mainImberPanelView->set_rightSaberSpeed(rightSaberSpeed * (_initialTimeScale / _audioTimeSyncController->get_timeScale()));
        }
    }

    void ImberManager::SpectateAreaController_DidUpdatePlayerSpectatorPose(Vector3 position, Quaternion rotation)
    {
        _imberUIPositionController->SetControllerOffset(position);
        // TODO: _posePlayer.SetSpectatorOffset
    }

    void ImberManager::CreateWatermark()
    {
        auto _watermarkObject = GameObject::New_ctor("Replay Prompt");
        auto _watermarkObjectTransform = _watermarkObject->get_transform();
        _watermarkObjectTransform->set_localScale(Vector3(0.05f, 0.05f, 0.05f));
        _watermarkObjectTransform->set_position(Vector3(0.0f, 0.025f, 0.8f));
        _watermarkObjectTransform->set_rotation(Quaternion::Euler(Vector3(90.0f, 0.0f, 0.0f)));
        auto canvas = _watermarkObject->AddComponent<Canvas*>();
        auto canvasRectTransform = il2cpp_utils::try_cast<RectTransform>(canvas).value_or(nullptr); // NOTE: Might not work
        canvasRectTransform->set_sizeDelta(Vector2(100.0f, 50.0f));

        auto curvedCanvasSettigns = _watermarkObject->AddComponent<HMUI::CurvedCanvasSettings*>();
        curvedCanvasSettigns->SetRadius(0.0f);

        auto curvedTextMeshPro = QuestUI::BeatSaberUI::CreateText(canvasRectTransform, "Double click left trigger to open Replay menu", Vector2(0.0f, 0.0f));
        curvedTextMeshPro->set_alignment(TMPro::TextAlignmentOptions::Center);
        curvedTextMeshPro->set_fontSize(4.0f);
        curvedTextMeshPro->set_color(Color(0.95f, 0.95f, 0.95f, 0.95f));
    }

    // UI Callbacks
    void ImberManager::MainImberPanelView_DidPositionTabVisibilityChange(bool value)
    {
        if (value)
        {
            _spectateAreaController->AnimateTo(_mainImberPanelView->get_location());
        }
        else
        {
            _spectateAreaController->Dismiss();
        }
    }

    void ImberManager::MainImberPanelView_DidPositionPreviewChange(StringW value)
    {
        _spectateAreaController->AnimateTo(value);
    }

    void ImberManager::MainImberPanelView_DidPositionJump()
    {
        _spectateAreaController->JumpToCallback(_mainImberPanelView->get_location());
    }

    void ImberManager::ImberScrubber_DidCalculateNewTime(float newTime)
    {
        _replayTimeSyncController->OverrideTime(newTime);
    }

    void ImberManager::MainImberPanelView_DidClickLoop()
    {
        _imberScrubber->_loopMode = !_imberScrubber->_loopMode;
        _mainImberPanelView->_loopText = _imberScrubber->_loopMode ? "UNLOOP" : "LOOP";
    }

    void ImberManager::MainImberPanelView_DidClickRestart()
    {
        _replayTimeSyncController->OverrideTime(0.0f);
    }

    void ImberManager::MainImberPanelView_DidClickPausePlay()
    {
        if (_audioTimeSyncController->state == GlobalNamespace::AudioTimeSyncController::State::Playing)
        {
            _replayTimeSyncController->CancelAllHitSounds();
            // _mainImberPanelView->set_playPauseText("PLAY");
            _audioTimeSyncController->Pause();
        }
        else if (_audioTimeSyncController->state == GlobalNamespace::AudioTimeSyncController::State::Paused)
        {
            // _mainImberPanelView->set_playPauseText("PAUSE");
            _audioTimeSyncController->Resume();
        }
    }

    void ImberManager::MainImberPanelView_DidTimeSyncChange(float value)
    {
        _replayTimeSyncController->OverrideTimeScale(value);
    }

    void ImberManager::MainImberPanelView_DidChangeVisibility(bool value)
    {
        _imberUIPositionController->SetActiveState(value);
        _mainImberPanelView->set_visibility(value);
        _imberScrubber->set_visibility(value);
        if (!value)
        {
            _spectateAreaController->Dismiss();
        }
    }

    void ImberManager::Dispose()
    {
        // TODO: Unsub from events
    }

} // namespace ScoreSaber::ReplaySystem::UI