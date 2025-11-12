#pragma once

#include <GlobalNamespace/BeatmapKey.hpp>
#include <GlobalNamespace/MainFlowCoordinator.hpp>
#include <GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp>
#include <GlobalNamespace/MultiplayerResultsData.hpp>
#include <GlobalNamespace/MultiplayerResultsViewController.hpp>
#include <GlobalNamespace/PlatformLeaderboardViewController.hpp>
#include <HMUI/ViewController.hpp>
#include <System/IDisposable.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include "System/zzzz__Object_def.hpp"
#include "Utils/DelegateUtils.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::UI::Multiplayer,
        ScoreSaberMultiplayerResultsLeaderboardFlowManager,
        System::Object,
        Zenject::IInitializable*,
        System::IDisposable*) {
    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::MainFlowCoordinator>, _mainFlowCoordinator);
    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::MultiplayerResultsViewController>, _multiplayerResultsViewController);
    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::PlatformLeaderboardViewController>, _platformLeaderboardViewController);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapKey, _lastCompletedBeatmapKey);
    DECLARE_CTOR(ctor,
        GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator,
        GlobalNamespace::MultiplayerResultsViewController* multiplayerResultsViewController,
        GlobalNamespace::PlatformLeaderboardViewController* platformLeaderboardViewController);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_INSTANCE_METHOD(void, MultiplayerResultsViewController_didActivateEvent, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_METHOD(void, MultiplayerResultsViewController_didDeactivateEvent, bool removedFromHierarchy, bool screenSystemDisabling);
    DECLARE_INSTANCE_METHOD(void, MultiplayerLevelDidFinish, GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO* transitionSetupData, GlobalNamespace::MultiplayerResultsData* results);
    DelegateUtils::DelegateW<GlobalNamespace::MultiplayerResultsViewController::DidActivateDelegate> didActivateDelegate;
    DelegateUtils::DelegateW<GlobalNamespace::MultiplayerResultsViewController::DidDeactivateDelegate> didDeactivateDelegate;
};