#include "ReplaySystem/UI/MainImberPanelView.hpp"
#include <HMUI/ViewController_AnimationType.hpp>
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/RectTransformUtility.hpp>
#include <UnityEngine/Vector2.hpp>
#include "assets.hpp"
#include <bsml/shared/BSML.hpp>
#include "logging.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"
#include <sstream>
#include <iomanip>

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, MainImberPanelView);

namespace ScoreSaber::ReplaySystem::UI
{

    // Props

    Transform* MainImberPanelView::transform
    {
        return _floatingScreen->transform;
    }

    void MainImberPanelView::visibility = bool value
    {
        if (_floatingScreen != nullptr)
        {
            _floatingScreen->SetRootViewController(value ? this : nullptr, value ? HMUI::ViewController::AnimationType::In : HMUI::ViewController::AnimationType::Out);
        }
    }

    void MainImberPanelView::fps = int value
    {
        if (fpsText != nullptr)
        {
            fpsText->text = std::to_string(value);
            if (value > 0.85f * _targetFPS)
            {
                fpsText->color = _goodColor;
            }
            else if (value > 0.6f * _targetFPS)
            {
                fpsText->color = _ehColor;
            }
            else
            {
                fpsText->color = _noColor;
            }
        }
    }

    void MainImberPanelView::leftSaberSpeed = float value
    {
        if (leftSpeedText != nullptr) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << value << " m/s";
            leftSpeedText->text = ss.str();
            leftSpeedText->color = value >= 2.0f ? _goodColor : _noColor;
        }
    }

    void MainImberPanelView::rightSaberSpeed = float value
    {
        if (rightSpeedText != nullptr) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << value << " m/s";
            rightSpeedText->text = ss.str();
            rightSpeedText->color = value >= 2.0f ? _goodColor : _noColor;
        }
    }

    void MainImberPanelView::didParse = bool value
    {
        _didParse = value;
    }

    bool MainImberPanelView::didParse
    {
        return _didParse;
    }

    void MainImberPanelView::timeSync = float value
    {
        _timeSync = Mathf::Approximately(_initialTime, value) ? _initialTime : value;
        if (DidTimeSyncChange != nullptr)
        {
            DidTimeSyncChange(_timeSync);
        }
    }

    float MainImberPanelView::timeSync
    {
        return _timeSync;
    }

    void MainImberPanelView::loopText = StringW value
    {
        _loopText = value;
    }

    StringW MainImberPanelView::loopText
    {
        return _loopText;
    }

    void MainImberPanelView::playPauseText = StringW value
    {
        _playPauseText = value;
    }

    StringW MainImberPanelView::playPauseText
    {
        return _playPauseText;
    }

    void MainImberPanelView::location = StringW value
    {
        _location = value;
        if (DidPositionPreviewChange != nullptr)
        {
            DidPositionPreviewChange(_location);
        }
    }

    StringW MainImberPanelView::location
    {
        return _location;
    }

    // Functions

    void MainImberPanelView::ctor()
    {
        INVOKE_CTOR();
        _playPauseText = "PAUSE";
        _loopText = "LOOP";
        _location = "";
    }


    void MainImberPanelView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        //getBase()->DidActivate(firstActivation, addedToHierarchy, screenSystemEnabling);
        if (firstActivation) {
            BSML::parse_and_construct(IncludedAssets::imber_panel_bsml, transform, this);
        }
        didSelectDelegate = custom_types::MakeDelegate<System::Action_2<HMUI::SegmentedControl*, int>*>((std::function<void(HMUI::SegmentedControl*, int)>)[&](HMUI::SegmentedControl* segmentedControl, int idx) {
            DidSelect(segmentedControl, idx);
        });
        tabSelector->textSegmentedControl->add_didSelectCellEvent(didSelectDelegate);
        didParse = true;
        if (firstActivation)
        {
            Vector3 localScale = tabSelector->transform->localScale;
            tabSelector->transform->localScale = {localScale.x * 0.9f, localScale.y * 0.9f, localScale.z * 0.9f};
        }
    }

    void MainImberPanelView::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        tabSelector->textSegmentedControl->remove_didSelectCellEvent(didSelectDelegate);
        //getBase()->DidDeactivate(removedFromHierarchy, screenSystemDisabling);
    }

    void MainImberPanelView::Construct()
    {
        _floatingScreen = BSML::FloatingScreen::CreateFloatingScreen({60.0f, 45.0f}, false, defaultPosition.position, defaultPosition.rotation, 0.0f, false);

        auto localScale = _floatingScreen->transform->localScale;
        _floatingScreen->GetComponent<Canvas*>()->sortingOrder = 31;
        _floatingScreen->transform->localScale = {localScale.x / 2.0f, localScale.y / 2.0f, localScale.z / 2.0f};
        _floatingScreen->name = "Imber Replay Panel (Screen)";
        name = "Imber Replay Panel (View)";
    }

    void MainImberPanelView::Setup(float initialSongTime, int targetFramerate, std::string defaultLocation, std::vector<std::string> _locations)
    {
        _initialTime = initialSongTime;
        _targetFPS = targetFramerate;
        _timeSync = initialSongTime;
        _location = defaultLocation;
        for (auto &loc : _locations) {
            this->locations->Add(StringW(loc));
        }
    }

    void MainImberPanelView::DidSelect(HMUI::SegmentedControl* segmentedControl, int selected)
    {
        int positionTabIndex = 2;
        if (_lastTab == 2 || selected == 2)
        {
            if (DidPositionTabVisibilityChange != nullptr)
            {
                DidPositionTabVisibilityChange(selected == positionTabIndex);
            }
        }
        _lastTab = selected;
    }

    void MainImberPanelView::SwitchHand(XR::XRNode xrNode)
    {
        if (HandDidSwitchEvent != nullptr)
        {
            HandDidSwitchEvent(xrNode);
        }
    }

    void MainImberPanelView::PausePlay()
    {
        if (DidClickPausePlay != nullptr)
        {
            DidClickPausePlay();
        }
    }

    void MainImberPanelView::Restart()
    {
        if (DidClickRestart != nullptr)
        {
            DidClickRestart();
        }
    }

    void MainImberPanelView::Loop()
    {
        if (DidClickLoop != nullptr)
        {
            DidClickLoop();
        }
    }

    void MainImberPanelView::SwitchHandLeft()
    {
        SwitchHand(XR::XRNode::LeftHand);
    }

    void MainImberPanelView::SwitchHandRight()
    {
        SwitchHand(XR::XRNode::RightHand);
    }

    void MainImberPanelView::RequestDismiss()
    {
        if (DidChangeVisiblity != nullptr)
        {
            DidChangeVisiblity(false);
        }
    }

    StringW MainImberPanelView::FormatTimePercent(float value)
    {
        return std::to_string(value);
    }

    void MainImberPanelView::Jump()
    {
        if (DidPositionJump != nullptr)
        {
            DidPositionJump();
        }
    }

} // namespace ScoreSaber::ReplaySystem::UI