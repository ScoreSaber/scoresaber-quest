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
#include "System/zzzz__Object_def.hpp"
#include "Utils/DelegateUtils.hpp"


DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::UI::Multiplayer,
        ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager,
        System::Object,
        Zenject::IInitializable*,
        System::IDisposable*) {
    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::MainFlowCoordinator>, _mainFlowCoordinator);
    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::ServerPlayerListViewController>, _serverPlayerListViewController);
    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::PlatformLeaderboardViewController>, _platformLeaderboardViewController);
    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::LevelSelectionNavigationController>, _levelSelectionNavigationController);
    DECLARE_CTOR(ctor,
        GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator,
        GlobalNamespace::ServerPlayerListViewController* serverPlayerListViewController,
        GlobalNamespace::PlatformLeaderboardViewController* platformLeaderboardViewController,
        GlobalNamespace::LevelSelectionNavigationController* levelSelectionNavigationController);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didActivateEvent, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didDeactivateEvent, bool removedFromHierarchy, bool screenSystemDisabling);
    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didChangeDifficultyBeatmapEvent, UnityW<GlobalNamespace::LevelSelectionNavigationController> controller);
    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didChangeLevelDetailContentEvent, UnityW<GlobalNamespace::LevelSelectionNavigationController> controller, GlobalNamespace::StandardLevelDetailViewController::ContentType contentType);
    DelegateUtils::DelegateW<GlobalNamespace::LevelSelectionNavigationController::DidActivateDelegate> didActivateDelegate;
    DelegateUtils::DelegateW<GlobalNamespace::LevelSelectionNavigationController::DidDeactivateDelegate> didDeactivateDelegate;
    DelegateUtils::DelegateW<System::Action_1<UnityW<GlobalNamespace::LevelSelectionNavigationController>>> didChangeDifficultyBeatmapDelegate;
    DelegateUtils::DelegateW<System::Action_2<UnityW<GlobalNamespace::LevelSelectionNavigationController>, GlobalNamespace::StandardLevelDetailViewController::ContentType>> didChangeLevelDetailContentDelegate;
    bool InMulti();
    void HideLeaderboard();
    void ShowLeaderboard();
    bool _currentlyInMulti;
    bool _performingFirstActivation;
};