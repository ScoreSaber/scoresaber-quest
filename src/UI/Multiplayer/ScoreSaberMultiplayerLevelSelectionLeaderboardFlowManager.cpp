#include "UI/Multiplayer/ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager.hpp"

#include <GlobalNamespace/MultiplayerLevelSelectionFlowCoordinator.hpp>
#include <GlobalNamespace/BeatmapKey.hpp>
#include <HMUI/ViewController.hpp>
#include <System/Action.hpp>
#include <UnityEngine/GameObject.hpp>
#include <custom-types/shared/delegate.hpp>
#include <functional>
#include <bsml/shared/BSML/MainThreadScheduler.hpp>

using namespace BSML;

DEFINE_TYPE(ScoreSaber::UI::Multiplayer, ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager);

namespace ScoreSaber::UI::Multiplayer
{
    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::ctor(MainFlowCoordinator* mainFlowCoordinator, ServerPlayerListViewController* serverPlayerListViewController, PlatformLeaderboardViewController* platformLeaderboardViewController, LevelSelectionNavigationController* levelSelectionNavigationController)
    {
        _mainFlowCoordinator = mainFlowCoordinator;
        _serverPlayerListViewController = serverPlayerListViewController;
        _platformLeaderboardViewController = platformLeaderboardViewController;
        _levelSelectionNavigationController = levelSelectionNavigationController;
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::Initialize()
    {
        didActivateDelegate = custom_types::MakeDelegate<LevelSelectionNavigationController::DidActivateDelegate*>((std::function<void(bool, bool, bool)>)[&](bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) { LevelSelectionNavigationController_didActivateEvent(firstActivation, addedToHierarchy, screenSystemEnabling); });
        didDeactivateDelegate = custom_types::MakeDelegate<LevelSelectionNavigationController::DidDeactivateDelegate*>((std::function<void(bool, bool)>)[&](bool removedFromHierarchy, bool screenSystemDisabling) { LevelSelectionNavigationController_didDeactivateEvent(removedFromHierarchy, screenSystemDisabling); });

        _levelSelectionNavigationController->add_didActivateEvent(didActivateDelegate);
        _levelSelectionNavigationController->add_didDeactivateEvent(didDeactivateDelegate);
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::Dispose()
    {
        _levelSelectionNavigationController->remove_didActivateEvent(didActivateDelegate);
        // the next one is missing in the pcvr version, so I'll leave it out here aswell
        // _levelSelectionNavigationController->remove_didDeactivateEvent(didDeactivateDelegate);
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didActivateEvent(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (!InMulti())
            return;

        if (firstActivation)
            _performingFirstActivation = true;

        _currentlyInMulti = true;

        didChangeDifficultyBeatmapDelegate = custom_types::MakeDelegate<System::Action_1<UnityW<LevelSelectionNavigationController>>*>((std::function<void(UnityW<LevelSelectionNavigationController>)>)[&](UnityW<LevelSelectionNavigationController> controller) { LevelSelectionNavigationController_didChangeDifficultyBeatmapEvent(controller); });
        didChangeLevelDetailContentDelegate = custom_types::MakeDelegate<System::Action_2<UnityW<LevelSelectionNavigationController>, StandardLevelDetailViewController::ContentType>*>((std::function<void(UnityW<LevelSelectionNavigationController>, StandardLevelDetailViewController::ContentType)>)[&](UnityW<LevelSelectionNavigationController> controller, StandardLevelDetailViewController::ContentType contentType) { LevelSelectionNavigationController_didChangeLevelDetailContentEvent(controller, contentType); });

        _levelSelectionNavigationController->add_didChangeDifficultyBeatmapEvent(didChangeDifficultyBeatmapDelegate);
        _levelSelectionNavigationController->add_didChangeLevelDetailContentEvent(didChangeLevelDetailContentDelegate);
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didDeactivateEvent(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        if (!InMulti())
            return;

        _currentlyInMulti = false;
        _levelSelectionNavigationController->remove_didChangeDifficultyBeatmapEvent(didChangeDifficultyBeatmapDelegate);
        _levelSelectionNavigationController->remove_didChangeLevelDetailContentEvent(didChangeLevelDetailContentDelegate);
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didChangeLevelDetailContentEvent(UnityW<LevelSelectionNavigationController> controller, StandardLevelDetailViewController::ContentType contentType)
    {
        ShowLeaderboard();
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didChangeDifficultyBeatmapEvent(UnityW<LevelSelectionNavigationController> controller)
    {
        ShowLeaderboard();
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::HideLeaderboard()
    {
        if (_platformLeaderboardViewController->isInViewControllerHierarchy)
        {
            auto currentFlowCoordinator = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
            if (!currentFlowCoordinator.try_cast<MultiplayerLevelSelectionFlowCoordinator>().has_value())
                return;

            currentFlowCoordinator->SetRightScreenViewController(nullptr, HMUI::ViewController::AnimationType::Out);
        }
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::ShowLeaderboard()
    {
        if (!InMulti())
            return;

        if(!_levelSelectionNavigationController->beatmapKey.IsValid()) {
            HideLeaderboard();
            return;
        }

        _platformLeaderboardViewController->SetData(ByRef<BeatmapKey>(_levelSelectionNavigationController->beatmapKey));
        auto currentFlowCoordinator = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
        currentFlowCoordinator->SetRightScreenViewController(_platformLeaderboardViewController, HMUI::ViewController::AnimationType::In);

        _serverPlayerListViewController->gameObject->SetActive(false); // copied from pcvr version: This is a bandaid fix, first time startup it gets stuck while animating kinda like the issue we had before (TODO: Fix in 2024)

        // Copied from the pcvr version, but still a bandaid
        if (_performingFirstActivation)
        {
            _performingFirstActivation = false;
            il2cpp_utils::il2cpp_aware_thread([&] {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                MainThreadScheduler::Schedule([=, this]() {
                    _platformLeaderboardViewController->Refresh(true, true);
                });
            }).detach();
        }
    }

    bool ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::InMulti()
    {
        if (_currentlyInMulti)
            return true;

        auto currentFlowCoordinator = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
        return currentFlowCoordinator.try_cast<MultiplayerLevelSelectionFlowCoordinator>().has_value();
    }
} // namespace ScoreSaber::UI::Multiplayer