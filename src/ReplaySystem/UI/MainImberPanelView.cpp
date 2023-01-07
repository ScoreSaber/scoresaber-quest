#include "ReplaySystem/UI/MainImberPanelView.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/RectTransformUtility.hpp"
#include "UnityEngine/Vector2.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
// #include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"
#include <sstream>
#include <iomanip>

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, MainImberPanelView);

namespace ScoreSaber::ReplaySystem::UI
{

    // Props

    Transform* MainImberPanelView::get_Transform()
    {
        return _floatingScreen->get_transform();
    }

    void MainImberPanelView::set_visibility(bool value)
    {
        if (_floatingScreen != nullptr)
        {
            _floatingScreen->SetRootViewController(value ? this : nullptr, value ? HMUI::ViewController::AnimationType::In : HMUI::ViewController::AnimationType::Out);
        }
    }

    void MainImberPanelView::set_fps(int value)
    {
        if (fpsText != nullptr)
        {
            fpsText->set_text(std::to_string(value));
            if (value > 0.85f * _targetFPS)
            {
                fpsText->set_color(_goodColor);
            }
            else if (value > 0.6f * _targetFPS)
            {
                fpsText->set_color(_ehColor);
            }
            else
            {
                fpsText->set_color(_noColor);
            }
        }
    }

    void MainImberPanelView::set_leftSaberSpeed(float value)
    {
        if (leftSpeedText != nullptr) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << value << " m/s";
            leftSpeedText->set_text(ss.str());
            leftSpeedText->set_color(value >= 2.0f ? _goodColor : _noColor);
        }
    }

    void MainImberPanelView::set_rightSaberSpeed(float value)
    {
        if (rightSpeedText != nullptr) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << value << " m/s";
            rightSpeedText->set_text(ss.str());
            rightSpeedText->set_color(value >= 2.0f ? _goodColor : _noColor);
        }
    }

    void MainImberPanelView::set_didParse(bool value)
    {
        _didParse = value;
    }

    bool MainImberPanelView::get_didParse()
    {
        return _didParse;
    }

    void MainImberPanelView::set_timeSync(float value)
    {
        _timeSync = Mathf::Approximately(_initialTime, value) ? _initialTime : value;
        if (DidTimeSyncChange != nullptr)
        {
            DidTimeSyncChange(_timeSync);
        }
    }

    float MainImberPanelView::get_timeSync()
    {
        return _timeSync;
    }

    void MainImberPanelView::set_loopText(StringW value)
    {
        _loopText = value;
    }

    StringW MainImberPanelView::get_loopText()
    {
        return _loopText;
    }

    void MainImberPanelView::set_playPauseText(StringW value)
    {
        _playPauseText = value;
    }

    StringW MainImberPanelView::get_playPauseText()
    {
        return _playPauseText;
    }

    void MainImberPanelView::set_location(StringW value)
    {
        _location = value;
        if (DidPositionPreviewChange != nullptr)
        {
            DidPositionPreviewChange(_location);
        }
    }

    StringW MainImberPanelView::get_location()
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

    System::Action_2<HMUI::SegmentedControl*, int> tabSelectAction;

    void MainImberPanelView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (!firstActivation)
        {
            return;
        }
        BSML::parse_and_construct(IncludedAssets::imber_panel_bsml, get_transform(), this);
        // tabSelectAction = il2cpp_utils::MakeDelegate<System::Action_2<HMUI::SegmentedControl*, int>*>(classof(System::Action_2<HMUI::SegmentedControl*, int>*), this, [](HMUI::SegmentedControl* segmentedControl, int idx) {
        //     DidSelect(idx);
        // });
        // tabSelector->textSegmentedControl->didSelectCellEvent += tabSelectAction;
        set_didParse(true);
        if (firstActivation)
        {
            Vector3 localScale = tabSelector->get_transform()->get_localScale();
            tabSelector->get_transform()->set_localScale({localScale.x * 0.9f, localScale.y * 0.9f, localScale.z * 0.9f});
        }
    }

    void MainImberPanelView::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        // tabSelector->textSegmentedControl->didSelectCellEvent -= tabSelectAction;
        // getBase()->DidDeactivate(removedFromHierarchy, screenSystemDisabling);
    }

    void MainImberPanelView::Construct()
    {
        _floatingScreen = BSML::FloatingScreen::CreateFloatingScreen({60.0f, 45.0f}, false, defaultPosition.position, defaultPosition.rotation, 0.0f, false);

        auto localScale = _floatingScreen->get_transform()->get_localScale();
        _floatingScreen->GetComponent<Canvas*>()->set_sortingOrder(31);
        _floatingScreen->get_transform()->set_localScale({localScale.x / 2.0f, localScale.y / 2.0f, localScale.z / 2.0f});
        _floatingScreen->set_name("Imber Replay Panel (Screen)");
        set_name("Imber Replay Panel (View)");
    }

    void MainImberPanelView::Setup(float initialSongTime, int targetFramerate, std::string defaultLocation, std::vector<std::string> _locations)
    {
        _initialTime = initialSongTime;
        _targetFPS = targetFramerate;
        _timeSync = initialSongTime;
        _location = defaultLocation;
        // _locations = locations;
        //_locations = locations;
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