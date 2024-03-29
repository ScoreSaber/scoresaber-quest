#include "ReplaySystem/UI/ImberManager.hpp"

#include "Data/Private/Settings.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/RectTransformUtility.hpp"
#include "UnityEngine/Vector2.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
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
        INVOKE_CTOR();
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

        for (auto &sp : Settings::spectatorPositions) {
            _positions.push_back(sp.name);
        }

        _mainImberPanelView->Setup(initData->timeScale, 90, _positions.front(), _positions);

        bool noFail = false;
        for (int i = 0; i < ReplayLoader::LoadedReplay->metadata->Modifiers.size(); i++)
        {
            if (StringUtils::Contains(ReplayLoader::LoadedReplay->metadata->Modifiers[i], "NF"))
            {
                noFail = true;
            }
        }

        _imberScrubber->Setup(ReplayLoader::LoadedReplay->metadata->FailTime, noFail);
        if (ReplayLoader::LoadedReplay->noteKeyframes.size() > 0)
            _initialTimeScale = ReplayLoader::LoadedReplay->noteKeyframes[0].TimeSyncTimescale;
        else
            _initialTimeScale = 1.0;
    }

    void ImberManager::Initialize()
    {
        _mainImberPanelView->DidClickLoop = [&]() {
            MainImberPanelView_DidClickLoop();
        };
        _mainImberPanelView->DidPositionJump = [&]() {
            MainImberPanelView_DidPositionJump();
        };
        _mainImberPanelView->DidClickRestart = [&]() {
            MainImberPanelView_DidClickRestart();
        };
        _mainImberPanelView->DidClickPausePlay = [&]() {
            MainImberPanelView_DidClickPausePlay();
        };
        _mainImberPanelView->DidTimeSyncChange = [&](float value) {
            MainImberPanelView_DidTimeSyncChange(value);
        };
        _mainImberPanelView->DidChangeVisiblity = [&](bool value) {
            MainImberPanelView_DidChangeVisibility(value);
        };
        _mainImberPanelView->HandDidSwitchEvent = [&](XR::XRNode value) {
            MainImberPanelView_DidHandSwitchEvent(value);
        };
        _mainImberPanelView->DidPositionPreviewChange = [&](std::string value) {
            MainImberPanelView_DidPositionPreviewChange(value);
        };
        _mainImberPanelView->DidPositionTabVisibilityChange = [&](bool value) {
            MainImberPanelView_DidPositionTabVisibilityChange(value);
        };
        _spectateAreaController->DidUpdatePlayerSpectatorPose = [&](Vector3 position, Quaternion rotation) {
            SpectateAreaController_DidUpdatePlayerSpectatorPose(position, rotation);
        };
        _imberScrubber->DidCalculateNewTime = [&](float value) {
            ImberScrubber_DidCalculateNewTime(value);
        };
        _imberSpecsReporter->DidReport = [&](int fps, float leftSaberSpeed, float rightSaberSpeed) {
            ImberSpecsReporter_DidReport(fps, leftSaberSpeed, rightSaberSpeed);
        };

        std::function<void()> gameDidResume = [&]() {
            GamePause_didResumeEvent();
        };
        _didResumeDelegate = custom_types::MakeDelegate<System::Action*>(classof(System::Action*), gameDidResume);
        _gamePause->add_didResumeEvent(_didResumeDelegate);

        if (!Settings::hasOpenedReplayUI) {
            CreateWatermark();
        }
    }

    void ImberManager::MainImberPanelView_DidHandSwitchEvent(XR::XRNode hand)
    {

        if (hand == XR::XRNode::RightHand) {
            Settings::leftHandedReplayUI = true;
        }
        if (hand == XR::XRNode::LeftHand) {
            Settings::leftHandedReplayUI = false;
        }
        Settings::SaveSettings();

        _imberUIPositionController->UpdateTrackingHand(hand);
    }

    void ImberManager::GamePause_didResumeEvent()
    {
        _mainImberPanelView->set_playPauseText("PAUSE");
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
        _posePlayer->SetSpectatorOffset(position);
    }

    void ImberManager::CreateWatermark()
    {
        auto _watermarkObject = GameObject::New_ctor("Replay Prompt");
        auto _watermarkObjectTransform = _watermarkObject->get_transform();
        _watermarkObjectTransform->set_localScale(Vector3(0.05f, 0.05f, 0.05f));
        _watermarkObjectTransform->set_position(Vector3(0.0f, 0.025f, 0.8f));
        _watermarkObjectTransform->set_rotation(Quaternion::Euler(Vector3(90.0f, 0.0f, 0.0f)));
        auto canvas = _watermarkObject->AddComponent<Canvas*>();
        auto canvasRectTransform = canvas->get_transform()->GetComponent<RectTransform*>();
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
        _imberScrubber->set_loopMode(!_imberScrubber->_loopMode);
        _mainImberPanelView->set_loopText(_imberScrubber->_loopMode ? "UNLOOP" : "LOOP");
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
            _mainImberPanelView->set_playPauseText("PLAY");
            _audioTimeSyncController->Pause();
        }
        else if (_audioTimeSyncController->state == GlobalNamespace::AudioTimeSyncController::State::Paused)
        {
            _mainImberPanelView->set_playPauseText("PAUSE");
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
        _mainImberPanelView->DidClickLoop = nullptr;
        _mainImberPanelView->DidPositionJump = nullptr;
        _mainImberPanelView->DidClickRestart = nullptr;
        _mainImberPanelView->DidClickPausePlay = nullptr;
        _mainImberPanelView->DidTimeSyncChange = nullptr;
        _mainImberPanelView->DidChangeVisiblity = nullptr;
        _mainImberPanelView->HandDidSwitchEvent = nullptr;
        _mainImberPanelView->DidPositionPreviewChange = nullptr;
        _mainImberPanelView->DidPositionTabVisibilityChange = nullptr;
        _spectateAreaController->DidUpdatePlayerSpectatorPose = nullptr;
        _imberScrubber->DidCalculateNewTime = nullptr;
        _imberSpecsReporter->DidReport = nullptr;

        _gamePause->remove_didResumeEvent(_didResumeDelegate);
    }

} // namespace ScoreSaber::ReplaySystem::UI