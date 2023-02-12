#pragma once

#include "GlobalNamespace/GameServerLobbyFlowCoordinator.hpp"
#include "System/Action.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/IInitializable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

using namespace GlobalNamespace;

#define INTERFACES                                                         \
    {                                                                      \
        classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::UI::Multiplayer, ScoreSaberMultiplayerInitializer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::UI::Multiplayer", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD(GameServerLobbyFlowCoordinator*, _gameServerLobbyFlowCoordinator);
                                    DECLARE_CTOR(ctor, GameServerLobbyFlowCoordinator* gameServerLobbyFlowCoordinator);
                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    DECLARE_INSTANCE_METHOD(void, GameServerLobbyFlowCoordinator_didSetupEvent);
                                    DECLARE_INSTANCE_METHOD(void, GameServerLobbyFlowCoordinator_didFinishEvent);
                                    System::Action* didSetupDelegate;
                                    System::Action* didFinishDelegate;)

#undef INTERFACES
