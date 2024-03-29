#include "ReplaySystem/UI/ImberUIPositionController.hpp"

#include "Data/Private/Settings.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "logging.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, ImberUIPositionController);

namespace ScoreSaber::ReplaySystem::UI
{
    void ImberUIPositionController::ctor(GlobalNamespace::IGamePause* gamePause, ReplaySystem::UI::ImberScrubber* imberScrubber, GlobalNamespace::PauseMenuManager* pauseMenuManager, ReplaySystem::UI::MainImberPanelView* mainImberPanelView, ReplaySystem::UI::VRControllerAccessor* vrControllerAccessor)
    {
        INVOKE_CTOR();
        _gamePause = gamePause;
        _imberScrubber = imberScrubber;
        _mainImberPanelView = mainImberPanelView;
        _vrControllerAccessor = vrControllerAccessor;
        _pauseMenuManagerTransform = pauseMenuManager->get_transform();
        _menuWrapperTransform = _pauseMenuManagerTransform->Find("Wrapper/MenuWrapper");
        _menuControllerTransform = _vrControllerAccessor->_leftController->get_transform()->get_parent();
        _vrGraphicRaycaster = _menuWrapperTransform->GetComponentInChildren<VRUIControls::VRGraphicRaycaster*>();
        _canvas = _vrGraphicRaycaster->GetComponent<Canvas*>();
        _curve = _canvas->GetComponent<HMUI::CurvedCanvasSettings*>();
        _mainSettingsModelSO = Resources::FindObjectsOfTypeAll<MainSettingsModelSO*>()->get(0);
        _controllerOffset = Vector3(0.0f, 0.0f, -2.0f);
    }
    void ImberUIPositionController::Initialize()
    {
        std::function<void()> gameDidResume = [&]() {
            GamePause_didResumeEvent();
        };
        std::function<void()> gameDidPause = [&]() {
            GamePause_didPauseEvent();
        };
        _didResumeDelegate = custom_types::MakeDelegate<System::Action*>(classof(System::Action*), gameDidResume);
        _didPauseDelegate = custom_types::MakeDelegate<System::Action*>(classof(System::Action*), gameDidPause);
        _gamePause->add_didResumeEvent(_didResumeDelegate);
        _gamePause->add_didPauseEvent(_didPauseDelegate);
        _pauseMenuManagerTransform->set_position(Vector3(_controllerOffset.x, _controllerOffset.y, _controllerOffset.z));

        if (Settings::leftHandedReplayUI) {
            _handTrack = XR::XRNode::RightHand;
        }
    }
    void ImberUIPositionController::GamePause_didResumeEvent()
    {
        _isPaused = false;
        _menuWrapperTransform->get_gameObject()->SetActive(_isActive);
        _menuControllerTransform->get_gameObject()->SetActive(_isActive);
        _vrGraphicRaycaster->set_enabled(_isActive);
    }
    void ImberUIPositionController::GamePause_didPauseEvent()
    {
        _isPaused = true;
        _menuWrapperTransform->get_gameObject()->SetActive(false);
        _curve->set_enabled(true);
        _canvas->set_enabled(true);
        _menuControllerTransform->get_gameObject()->SetActive(true);
    }
    void ImberUIPositionController::Tick()
    {
        VRController* controller = _handTrack == XR::XRNode::LeftHand ? _vrControllerAccessor->_leftController : _vrControllerAccessor->_rightController;
        if (_didClickOnce && System::DateTime::get_Now() > _lastTriggerDownTime.AddSeconds(_timeBufferToDoubleClick))
        {
            _didClickOnce = false;
        }
        else
        {
            if (controller->get_triggerValue() >= _senstivityToClick && !_isClicking)
            {
                _isClicking = true;
                if (_didClickOnce)
                {
                    _didClickOnce = false;
                    // DID DOUBLE CLICK HERE!!!
                    _isActive = !_isActive;
                    _imberScrubber->set_visibility(_isActive);
                    _mainImberPanelView->set_visibility(_isActive);
                    OpenedUI();
                    GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(KillMe(controller)));
                    if (!_isPaused)
                    {
                        _curve->set_enabled(!_isActive);
                        _canvas->set_enabled(!_isActive);
                        _menuWrapperTransform->get_gameObject()->SetActive(_isActive);
                        _menuControllerTransform->get_gameObject()->SetActive(_isActive);
                        _vrGraphicRaycaster->set_enabled(_isActive);
                    }
                }
                else
                {
                    _lastTriggerDownTime = System::DateTime::get_Now();
                    _didClickOnce = true;
                }
            }
            else if (controller->get_triggerValue() < _senstivityToClick && _isClicking)
            {
                _isClicking = false;
            }
        }
        if (_isActive && !Settings::lockedReplayUIMode)
        {
            SetUIPosition(controller);
        }
    }
    custom_types::Helpers::Coroutine ImberUIPositionController::KillMe(VRController* controller)
    {
        for (int i = 0; i < 5; i++)
        {
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForEndOfFrame::New_ctor());
        }

        SetUIPosition(controller);
        co_return;
    }
    void ImberUIPositionController::SetUIPosition(VRController* controller)
    {
        Vector3 viewOffset = _handTrack == XR::XRNode::LeftHand ? Vector3(0.25f, 0.25f, 0.25f) : Vector3(-0.25f, 0.25f, 0.25f);
        Vector3 scrubberOffset = _handTrack == XR::XRNode::LeftHand ? Vector3(0.46f, -0.06f, 0.25f) : Vector3(-0.46f, -0.06f, 0.25f);

        _mainImberPanelView->get_Transform()->set_localPosition(controller->get_transform()->TransformPoint(viewOffset));
        _mainImberPanelView->get_Transform()->set_localRotation(controller->get_transform()->get_rotation());

        _imberScrubber->get_transform()->set_localPosition(controller->get_transform()->TransformPoint(scrubberOffset));
        _imberScrubber->get_transform()->set_localRotation(controller->get_transform()->get_rotation());
    }

    void ImberUIPositionController::OpenedUI()
    {
        if (!Settings::hasOpenedReplayUI) {
            GameObject* replayPrompt = GameObject::Find("Replay Prompt");
            if (replayPrompt != nullptr) {
                GameObject::Destroy(replayPrompt);
            }
            Settings::hasOpenedReplayUI = true;
            Settings::SaveSettings();
        }
    }

    void ImberUIPositionController::UpdateTrackingHand(XR::XRNode handTrack)
    {
        _handTrack = handTrack;
    }

    void ImberUIPositionController::SetControllerOffset(Vector3 offset)
    {
        _controllerOffset = offset;
        _pauseMenuManagerTransform->set_position(Vector3(_controllerOffset.x, _controllerOffset.y, _controllerOffset.z));
    }

    void ImberUIPositionController::SetActiveState(bool value)
    {
        _isActive = value;
    }

    void ImberUIPositionController::Dispose()
    {
        _gamePause->remove_didResumeEvent(_didResumeDelegate);
        _gamePause->remove_didPauseEvent(_didPauseDelegate);
    }
} // namespace ScoreSaber::ReplaySystem::UI