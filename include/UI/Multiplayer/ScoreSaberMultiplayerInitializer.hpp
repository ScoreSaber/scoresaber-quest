#pragma once

#include <GlobalNamespace/GameServerLobbyFlowCoordinator.hpp>
#include <System/Action.hpp>
#include <System/IDisposable.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include "Utils/DelegateUtils.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::UI::Multiplayer,
        ScoreSaberMultiplayerInitializer,
        System::Object,
        Zenject::IInitializable*,
        System::IDisposable*) {
    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::GameServerLobbyFlowCoordinator>, _gameServerLobbyFlowCoordinator);
    DECLARE_CTOR(ctor, GlobalNamespace::GameServerLobbyFlowCoordinator* gameServerLobbyFlowCoordinator);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_INSTANCE_METHOD(void, GameServerLobbyFlowCoordinator_didSetupEvent);
    DECLARE_INSTANCE_METHOD(void, GameServerLobbyFlowCoordinator_didFinishEvent);
    DelegateUtils::DelegateW<System::Action> didSetupDelegate;
    DelegateUtils::DelegateW<System::Action> didFinishDelegate;
};
