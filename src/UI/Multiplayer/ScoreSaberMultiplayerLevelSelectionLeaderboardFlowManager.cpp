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
using namespace GlobalNamespace;

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
        didActivateDelegate = { &ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didActivateEvent, this };
        didDeactivateDelegate = { &ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didDeactivateEvent, this };

        _levelSelectionNavigationController->___didActivateEvent += didActivateDelegate;
        _levelSelectionNavigationController->___didDeactivateEvent += didDeactivateDelegate;
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::Dispose()
    {
        if(_levelSelectionNavigationController) {
            _levelSelectionNavigationController->___didActivateEvent -= didActivateDelegate;
            // the next one is missing in the pcvr version, so I'll leave it out here aswell
            // _levelSelectionNavigationController->___didDeactivateEvent -= didDeactivateDelegate;
        }
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didActivateEvent(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (!InMulti())
            return;

        if (firstActivation)
            _performingFirstActivation = true;

        _currentlyInMulti = true;

        didChangeDifficultyBeatmapDelegate = { &ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didChangeDifficultyBeatmapEvent, this };
        didChangeLevelDetailContentDelegate = { &ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didChangeLevelDetailContentEvent, this };

        _levelSelectionNavigationController->___didChangeDifficultyBeatmapEvent += didChangeDifficultyBeatmapDelegate;
        _levelSelectionNavigationController->___didChangeLevelDetailContentEvent += didChangeLevelDetailContentDelegate;
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didDeactivateEvent(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        if (!InMulti())
            return;

        _currentlyInMulti = false;
        _levelSelectionNavigationController->___didChangeDifficultyBeatmapEvent -= didChangeDifficultyBeatmapDelegate;
        _levelSelectionNavigationController->___didChangeLevelDetailContentEvent -= didChangeLevelDetailContentDelegate;
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

        auto beatmapKey = _levelSelectionNavigationController->beatmapKey;
        _platformLeaderboardViewController->SetData(byref(beatmapKey));
        auto currentFlowCoordinator = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
        currentFlowCoordinator->SetRightScreenViewController(_platformLeaderboardViewController, HMUI::ViewController::AnimationType::In);

        _serverPlayerListViewController->gameObject->SetActive(false); // copied from pcvr version: This is a bandaid fix, first time startup it gets stuck while animating kinda like the issue we had before (TODO: Fix in 2024)

        // Copied from the pcvr version, but still a bandaid
        if (_performingFirstActivation)
        {
            _performingFirstActivation = false;

            SafePtr<ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager> self(this);

            il2cpp_utils::il2cpp_aware_thread([self] {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                MainThreadScheduler::Schedule([self]() {
                    self->_platformLeaderboardViewController->Refresh(true, true);
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