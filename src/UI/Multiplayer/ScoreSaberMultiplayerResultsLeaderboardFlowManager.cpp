#include "UI/Multiplayer/ScoreSaberMultiplayerResultsLeaderboardFlowManager.hpp"

#include <GlobalNamespace/GameServerLobbyFlowCoordinator.hpp>
#include <GlobalNamespace/MenuTransitionsHelper.hpp>
#include <HMUI/ViewController.hpp>
#include "Services/PlayerService.hpp"
#include "UI/Other/ScoreSaberLeaderboardView.hpp"
#include <custom-types/shared/delegate.hpp>
#include "hooks.hpp"
#include <functional>

DEFINE_TYPE(ScoreSaber::UI::Multiplayer, ScoreSaberMultiplayerResultsLeaderboardFlowManager);

namespace ScoreSaber::UI::Multiplayer
{
    std::optional<std::function<void(MultiplayerLevelScenesTransitionSetupDataSO*, MultiplayerResultsData*)>> HandleMultiplayerLevelDidFinish;

    void ScoreSaberMultiplayerResultsLeaderboardFlowManager::ctor(MainFlowCoordinator* mainFlowCoordinator, MultiplayerResultsViewController* multiplayerResultsViewController, PlatformLeaderboardViewController* platformLeaderboardViewController)
    {
        _mainFlowCoordinator = mainFlowCoordinator;
        _multiplayerResultsViewController = multiplayerResultsViewController;
        _platformLeaderboardViewController = platformLeaderboardViewController;
    }

    void ScoreSaberMultiplayerResultsLeaderboardFlowManager::Initialize()
    {
        didActivateDelegate = { &ScoreSaberMultiplayerResultsLeaderboardFlowManager::MultiplayerResultsViewController_didActivateEvent, this };
        didDeactivateDelegate = { &ScoreSaberMultiplayerResultsLeaderboardFlowManager::MultiplayerResultsViewController_didDeactivateEvent, this };

        _multiplayerResultsViewController->___didActivateEvent += didActivateDelegate;
        _multiplayerResultsViewController->___didDeactivateEvent += didDeactivateDelegate;

        SafePtr<ScoreSaberMultiplayerResultsLeaderboardFlowManager> self(this);
        HandleMultiplayerLevelDidFinish = [self](MultiplayerLevelScenesTransitionSetupDataSO * transitionSetupData, MultiplayerResultsData * results)
        {
            self->MultiplayerLevelDidFinish(transitionSetupData, results);
        };
    }

    void ScoreSaberMultiplayerResultsLeaderboardFlowManager::Dispose()
    {
        if(_multiplayerResultsViewController) {
            _multiplayerResultsViewController->___didActivateEvent -= didActivateDelegate;
            _multiplayerResultsViewController->___didDeactivateEvent -= didDeactivateDelegate;
        }
        HandleMultiplayerLevelDidFinish = std::nullopt;
    }

    void ScoreSaberMultiplayerResultsLeaderboardFlowManager::MultiplayerResultsViewController_didActivateEvent(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        auto currentFlowCoordinator = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
        if (!currentFlowCoordinator.try_cast<GameServerLobbyFlowCoordinator>().has_value())
            return;

        _platformLeaderboardViewController->SetData(_lastCompletedBeatmapKey);
        currentFlowCoordinator->SetRightScreenViewController(_platformLeaderboardViewController, HMUI::ViewController::AnimationType::In);
    }

    void ScoreSaberMultiplayerResultsLeaderboardFlowManager::MultiplayerResultsViewController_didDeactivateEvent(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        // we can't really set this to null anymore
        /*if (removedFromHierarchy || screenSystemDisabling)
            _lastCompletedBeatmap = nullptr;*/
    }

    void ScoreSaberMultiplayerResultsLeaderboardFlowManager::MultiplayerLevelDidFinish(MultiplayerLevelScenesTransitionSetupDataSO* transitionSetupData, MultiplayerResultsData* results)
    {
        _lastCompletedBeatmapKey = transitionSetupData->beatmapKey;
    }
} // namespace ScoreSaber::UI::Multiplayer

MAKE_AUTO_HOOK_MATCH(MenuTransitionsHelper_HandleMultiplayerLevelDidFinish, &MenuTransitionsHelper::HandleMultiplayerLevelDidFinish,
                     void, MenuTransitionsHelper* self, MultiplayerLevelScenesTransitionSetupDataSO* multiplayerLevelScenesTransitionSetupData, MultiplayerResultsData* multiplayerResultsData)
{
    if (auto callback = ScoreSaber::UI::Multiplayer::HandleMultiplayerLevelDidFinish)
        (*callback)(multiplayerLevelScenesTransitionSetupData, multiplayerResultsData);
    MenuTransitionsHelper_HandleMultiplayerLevelDidFinish(self, multiplayerLevelScenesTransitionSetupData, multiplayerResultsData);
}