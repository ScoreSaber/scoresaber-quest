#include "UI/FlowCoordinators/ScoreSaberSettingsFlowCoordinator.hpp"

#include "Data/Private/Settings.hpp"
#include "Services/PlayerService.hpp"
#include "UI/Other/ScoreSaberLeaderboardView.hpp"

#include <bsml/shared/Helpers/creation.hpp>
#include <HMUI/ViewController.hpp>

DEFINE_TYPE(ScoreSaber::UI::FlowCoordinators, ScoreSaberSettingsFlowCoordinator);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace BSML::Helpers;
using namespace ScoreSaber::Data::Private;
using namespace ScoreSaber::Services;
using namespace ScoreSaber::UI::Other;

namespace ScoreSaber::UI::FlowCoordinators
{
    void ScoreSaberSettingsFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            SetTitle("ScoreSaber Settings", ViewController::AnimationType::In);
            showBackButton = true;
            mainSettingsViewController = CreateViewController<ScoreSaber::UI::ViewControllers::MainSettingsViewController*>();

            ProvideInitialViewControllers(mainSettingsViewController, nullptr, nullptr, nullptr, nullptr);
        }
    }

    void ScoreSaberSettingsFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController)
    {
        SetLeftScreenViewController(nullptr, ViewController::AnimationType::None);
        SetRightScreenViewController(nullptr, ViewController::AnimationType::None);
        Settings::SaveSettings();
        this->_parentFlowCoordinator->DismissFlowCoordinator(this, ViewController::AnimationDirection::Horizontal, nullptr, false);
        PlayerService::UpdatePlayerInfo(true);
        ScoreSaberLeaderboardView::RefreshLeaderboard();
    }
}