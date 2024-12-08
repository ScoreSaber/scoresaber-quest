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
        didActivateDelegate = custom_types::MakeDelegate<MultiplayerResultsViewController::DidActivateDelegate*>((std::function<void(bool, bool, bool)>)[&](bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) { MultiplayerResultsViewController_didActivateEvent(firstActivation, addedToHierarchy, screenSystemEnabling); });
        didDeactivateDelegate = custom_types::MakeDelegate<MultiplayerResultsViewController::DidDeactivateDelegate*>((std::function<void(bool, bool)>)[&](bool removedFromHierarchy, bool screenSystemDisabling) { MultiplayerResultsViewController_didDeactivateEvent(removedFromHierarchy, screenSystemDisabling); });

        _multiplayerResultsViewController->add_didActivateEvent(didActivateDelegate);
        _multiplayerResultsViewController->add_didDeactivateEvent(didDeactivateDelegate);
        HandleMultiplayerLevelDidFinish = (std::function<void(MultiplayerLevelScenesTransitionSetupDataSO*, MultiplayerResultsData*)>)[&](MultiplayerLevelScenesTransitionSetupDataSO * transitionSetupData, MultiplayerResultsData * results)
        {
            MultiplayerLevelDidFinish(transitionSetupData, results);
        };
    }

    void ScoreSaberMultiplayerResultsLeaderboardFlowManager::Dispose()
    {
        _multiplayerResultsViewController->remove_didActivateEvent(didActivateDelegate);
        _multiplayerResultsViewController->remove_didDeactivateEvent(didDeactivateDelegate);
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