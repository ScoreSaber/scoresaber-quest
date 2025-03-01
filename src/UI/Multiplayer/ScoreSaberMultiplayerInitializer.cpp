#include "UI/Multiplayer/ScoreSaberMultiplayerInitializer.hpp"

#include "Services/PlayerService.hpp"
#include "UI/Other/ScoreSaberLeaderboardView.hpp"
#include <custom-types/shared/delegate.hpp>
#include <functional>

DEFINE_TYPE(ScoreSaber::UI::Multiplayer, ScoreSaberMultiplayerInitializer);

namespace ScoreSaber::UI::Multiplayer
{
    void ScoreSaberMultiplayerInitializer::ctor(GameServerLobbyFlowCoordinator* gameServerLobbyFlowCoordinator)
    {
        _gameServerLobbyFlowCoordinator = gameServerLobbyFlowCoordinator;
    }

    void ScoreSaberMultiplayerInitializer::Initialize()
    {
        didSetupDelegate = { &ScoreSaberMultiplayerInitializer::GameServerLobbyFlowCoordinator_didSetupEvent, this };
        didFinishDelegate = { &ScoreSaberMultiplayerInitializer::GameServerLobbyFlowCoordinator_didFinishEvent, this };

        _gameServerLobbyFlowCoordinator->___didSetupEvent += didSetupDelegate;
        _gameServerLobbyFlowCoordinator->___didFinishEvent += didFinishDelegate;
    }

    void ScoreSaberMultiplayerInitializer::Dispose()
    {
        if(_gameServerLobbyFlowCoordinator) {
            _gameServerLobbyFlowCoordinator->___didSetupEvent -= didSetupDelegate;
            _gameServerLobbyFlowCoordinator->___didFinishEvent -= didFinishDelegate;
        }
    }

    void ScoreSaberMultiplayerInitializer::GameServerLobbyFlowCoordinator_didSetupEvent()
    {
        Services::PlayerService::AuthenticateUser([&](Services::PlayerService::LoginStatus loginStatus) {});
        Other::ScoreSaberLeaderboardView::AllowReplayWatching(false);
    }

    void ScoreSaberMultiplayerInitializer::GameServerLobbyFlowCoordinator_didFinishEvent()
    {
        Other::ScoreSaberLeaderboardView::AllowReplayWatching(true);
    }
} // namespace ScoreSaber::UI::Multiplayer