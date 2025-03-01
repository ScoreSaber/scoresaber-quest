#include "ReplaySystem/UI/ImberUIPositionController.hpp"

#include "Data/Private/Settings.hpp"

#include <UnityEngine/Resources.hpp>
#include <UnityEngine/WaitForEndOfFrame.hpp>
#include "logging.hpp"
#include "Utils/OperatorOverloads.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, ImberUIPositionController);

namespace ScoreSaber::ReplaySystem::UI
{
    void ImberUIPositionController::ctor(GlobalNamespace::IGamePause* gamePause, ReplaySystem::UI::ImberScrubber* imberScrubber, GlobalNamespace::PauseMenuManager* pauseMenuManager, ReplaySystem::UI::MainImberPanelView* mainImberPanelView, ReplaySystem::UI::VRControllerAccessor* vrControllerAccessor)
    {
        INVOKE_CTOR();
        _gamePause = il2cpp_utils::try_cast<GamePause>(gamePause).value();
        _imberScrubber = imberScrubber;
        _mainImberPanelView = mainImberPanelView;
        _vrControllerAccessor = vrControllerAccessor;
        _pauseMenuManagerTransform = pauseMenuManager->transform;
        _menuWrapperTransform = _pauseMenuManagerTransform->Find("Wrapper/MenuWrapper");
        _menuControllerTransform = _vrControllerAccessor->_leftController->transform->parent;
        _vrGraphicRaycaster = _menuWrapperTransform->GetComponentInChildren<VRUIControls::VRGraphicRaycaster*>();
        _canvas = _vrGraphicRaycaster->GetComponent<Canvas*>();
        _curve = _canvas->GetComponent<HMUI::CurvedCanvasSettings*>();
        _controllerOffset = Vector3(0.0f, 0.0f, -2.0f);
    }
    void ImberUIPositionController::Initialize()
    {
        _didResumeDelegate = { &ImberUIPositionController::GamePause_didResumeEvent, this };
        _didPauseDelegate = { &ImberUIPositionController::GamePause_didPauseEvent, this };
        _gamePause->___didResumeEvent += _didResumeDelegate;
        _gamePause->___didPauseEvent += _didPauseDelegate;
        _pauseMenuManagerTransform->position = Vector3(_controllerOffset.x, _controllerOffset.y, _controllerOffset.z);

        if (Settings::leftHandedReplayUI) {
            _handTrack = XR::XRNode::RightHand;
        }
    }
    void ImberUIPositionController::GamePause_didResumeEvent()
    {
        _isPaused = false;
        _menuWrapperTransform->gameObject->SetActive(_isActive);
        _menuControllerTransform->gameObject->SetActive(_isActive);
        _vrGraphicRaycaster->enabled = _isActive;
    }
    void ImberUIPositionController::GamePause_didPauseEvent()
    {
        _isPaused = true;
        _menuWrapperTransform->gameObject->SetActive(false);
        _curve->enabled = true;
        _canvas->enabled = true;
        _menuControllerTransform->gameObject->SetActive(true);
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
            if (controller->triggerValue >= _senstivityToClick && !_isClicking)
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
                    _mainImberPanelView->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(KillMe(controller)));
                    if (!_isPaused)
                    {
                        _curve->enabled = !_isActive;
                        _canvas->enabled = !_isActive;
                        _menuWrapperTransform->gameObject->SetActive(_isActive);
                        _menuControllerTransform->gameObject->SetActive(_isActive);
                        _vrGraphicRaycaster->enabled = _isActive;
                    }
                }
                else
                {
                    _lastTriggerDownTime = System::DateTime::get_Now();
                    _didClickOnce = true;
                }
            }
            else if (controller->triggerValue < _senstivityToClick && _isClicking)
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

        _mainImberPanelView->get_Transform()->SetLocalPositionAndRotation(controller->transform->TransformPoint(viewOffset), controller->transform->rotation);
        _imberScrubber->get_transform()->SetLocalPositionAndRotation(controller->transform->TransformPoint(scrubberOffset), controller->transform->rotation);
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
        _pauseMenuManagerTransform->position = Vector3(_controllerOffset.x, _controllerOffset.y, _controllerOffset.z);
    }

    void ImberUIPositionController::SetActiveState(bool value)
    {
        _isActive = value;
    }

    void ImberUIPositionController::Dispose()
    {
        _gamePause->___didResumeEvent -= _didResumeDelegate;
        _gamePause->___didPauseEvent -= _didPauseDelegate;
    }
} // namespace ScoreSaber::ReplaySystem::UI