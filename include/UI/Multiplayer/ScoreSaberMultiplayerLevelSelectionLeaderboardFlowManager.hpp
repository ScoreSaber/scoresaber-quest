#pragma once

#include <GlobalNamespace/LevelSelectionNavigationController.hpp>
#include <GlobalNamespace/MainFlowCoordinator.hpp>
#include <GlobalNamespace/PlatformLeaderboardViewController.hpp>
#include <GlobalNamespace/ServerPlayerListViewController.hpp>
#include <GlobalNamespace/StandardLevelDetailViewController.hpp>
#include <HMUI/ViewController.hpp>
#include <System/Action_2.hpp>
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

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::UI::Multiplayer, ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::UI::Multiplayer", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD(MainFlowCoordinator*, _mainFlowCoordinator);
                                    DECLARE_INSTANCE_FIELD(ServerPlayerListViewController*, _serverPlayerListViewController);
                                    DECLARE_INSTANCE_FIELD(PlatformLeaderboardViewController*, _platformLeaderboardViewController);
                                    DECLARE_INSTANCE_FIELD(LevelSelectionNavigationController*, _levelSelectionNavigationController);
                                    DECLARE_CTOR(ctor, MainFlowCoordinator* mainFlowCoordinator, ServerPlayerListViewController* serverPlayerListViewController, PlatformLeaderboardViewController* platformLeaderboardViewController, LevelSelectionNavigationController* levelSelectionNavigationController);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
                                    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didActivateEvent, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                                    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didDeactivateEvent, bool removedFromHierarchy, bool screenSystemDisabling);
                                    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didChangeDifficultyBeatmapEvent, UnityW<LevelSelectionNavigationController> controller);
                                    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didChangeLevelDetailContentEvent, UnityW<LevelSelectionNavigationController> controller, StandardLevelDetailViewController::ContentType contentType);
                                    DelegateUtils::DelegateW<LevelSelectionNavigationController::DidActivateDelegate> didActivateDelegate;
                                    DelegateUtils::DelegateW<LevelSelectionNavigationController::DidDeactivateDelegate> didDeactivateDelegate;
                                    DelegateUtils::DelegateW<System::Action_1<UnityW<LevelSelectionNavigationController>>> didChangeDifficultyBeatmapDelegate;
                                    DelegateUtils::DelegateW<System::Action_2<UnityW<LevelSelectionNavigationController>, StandardLevelDetailViewController::ContentType>> didChangeLevelDetailContentDelegate;
                                    bool InMulti();
                                    void HideLeaderboard();
                                    void ShowLeaderboard();
                                    bool _currentlyInMulti;
                                    bool _performingFirstActivation;)

#undef INTERFACES
