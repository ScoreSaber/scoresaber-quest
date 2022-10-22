
#include "ReplaySystem/UI/ImberUIPositionController.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "System/Action.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include <custom-types/shared/delegate.hpp>

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, ImberUIPositionController);

namespace ScoreSaber::ReplaySystem::UI
{
    void ImberUIPositionController::ctor(GlobalNamespace::IGamePause* gamePause, ReplaySystem::UI::ImberScrubber* imberScrubber, GlobalNamespace::PauseMenuManager* pauseMenuManager, ReplaySystem::UI::MainImberPanelView* mainImberPanelView, ReplaySystem::UI::VRControllerAccessor* vrControllerAccessor)
    {
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
        _controllerOffset = Vector3(0.0f, 0.0f, 0.0f);
    }
    void ImberUIPositionController::Initialize()
    {
        std::function<void()> gameDidResume = [&]() {
            GamePause_didResumeEvent();
        };
        std::function<void()> gameDidPause = [&]() {
            GamePause_didPauseEvent();
        };
        _gamePause->add_didResumeEvent(custom_types::MakeDelegate<System::Action*>(classof(System::Action*), gameDidResume));
        _gamePause->add_didPauseEvent(custom_types::MakeDelegate<System::Action*>(classof(System::Action*), gameDidPause));
        _pauseMenuManagerTransform->set_position(Vector3(_controllerOffset.x, _controllerOffset.y, _controllerOffset.z));

        // TODO: Check left hand
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
                    OpenedUI();
                    GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(
                        custom_types::Helpers::CoroutineHelper::New(KillMe(controller)));
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
        // TODO: Check settings for lockedReplayUIMode
        if (_isActive)
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

        _mainImberPanelView->get_transform()->set_localPosition(controller->get_transform()->TransformPoint(viewOffset));
        _mainImberPanelView->get_transform()->set_localRotation(controller->get_transform()->get_rotation());

        _imberScrubber->get_transform()->set_localPosition(controller->get_transform()->TransformPoint(scrubberOffset));
        _imberScrubber->get_transform()->set_localRotation(controller->get_transform()->get_rotation());

        // _mainImberPanelView->get_Transform().SetLocalPosition
    }
} // namespace ScoreSaber::ReplaySystem::UI