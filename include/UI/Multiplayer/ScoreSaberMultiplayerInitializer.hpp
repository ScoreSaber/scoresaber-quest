#pragma once

#include <GlobalNamespace/GameServerLobbyFlowCoordinator.hpp>
#include <System/Action.hpp>
#include <System/IDisposable.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include "Utils/DelegateUtils.hpp"

using namespace GlobalNamespace;

#define INTERFACES                                                         \
    {                                                                      \
        classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::UI::Multiplayer, ScoreSaberMultiplayerInitializer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::UI::Multiplayer", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD(UnityW<GameServerLobbyFlowCoordinator>, _gameServerLobbyFlowCoordinator);
                                    DECLARE_CTOR(ctor, GameServerLobbyFlowCoordinator* gameServerLobbyFlowCoordinator);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
                                    DECLARE_INSTANCE_METHOD(void, GameServerLobbyFlowCoordinator_didSetupEvent);
                                    DECLARE_INSTANCE_METHOD(void, GameServerLobbyFlowCoordinator_didFinishEvent);
                                    DelegateUtils::DelegateW<System::Action> didSetupDelegate;
                                    DelegateUtils::DelegateW<System::Action> didFinishDelegate;)

#undef INTERFACES
