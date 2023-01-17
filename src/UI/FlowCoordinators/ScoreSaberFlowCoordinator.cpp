#include "UI/FlowCoordinators/ScoreSaberFlowCoordinator.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "questui/shared/BeatSaberUI.hpp"

DEFINE_TYPE(ScoreSaber::UI::FlowCoordinators, ScoreSaberFlowCoordinator);

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

namespace ScoreSaber::UI::FlowCoordinators
{
    void ScoreSaberFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            globalViewController = CreateViewController<ScoreSaber::UI::ViewControllers::GlobalViewController*>();
            faqViewController = CreateViewController<ScoreSaber::UI::ViewControllers::FAQViewController*>();
            teamViewController = CreateViewController<ScoreSaber::UI::ViewControllers::TeamViewController*>();

            SetTitle("ScoreSaber", ViewController::AnimationType::Out);
            set_showBackButton(true);
            ProvideInitialViewControllers(globalViewController, teamViewController, faqViewController, nullptr, nullptr);
        }
        // HACK: if we don't do this the viewcontroller remains active when returning to the main song menu (don't ask me why)
        faqViewController->get_gameObject()->SetActive(true);
    }

    void ScoreSaberFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController)
    {
        // HACK: if we don't do this the viewcontroller remains active when returning to the main song menu (don't ask me why)
        faqViewController->get_gameObject()->SetActive(false);
        this->parentFlowCoordinator->DismissFlowCoordinator(this, ViewController::AnimationDirection::Horizontal, nullptr, false);
    }
}