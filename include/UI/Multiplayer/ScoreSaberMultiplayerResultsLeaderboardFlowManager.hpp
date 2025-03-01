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
#include "Utils/DelegateUtils.hpp"

using namespace GlobalNamespace;

#define INTERFACES                                                         \
    {                                                                      \
        classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::UI::Multiplayer, ScoreSaberMultiplayerResultsLeaderboardFlowManager, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::UI::Multiplayer", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD(MainFlowCoordinator*, _mainFlowCoordinator);
                                    DECLARE_INSTANCE_FIELD(MultiplayerResultsViewController*, _multiplayerResultsViewController);
                                    DECLARE_INSTANCE_FIELD(PlatformLeaderboardViewController*, _platformLeaderboardViewController);
                                    DECLARE_INSTANCE_FIELD(BeatmapKey, _lastCompletedBeatmapKey);
                                    DECLARE_CTOR(ctor, MainFlowCoordinator* mainFlowCoordinator, MultiplayerResultsViewController* multiplayerResultsViewController, PlatformLeaderboardViewController* platformLeaderboardViewController);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
                                    DECLARE_INSTANCE_METHOD(void, MultiplayerResultsViewController_didActivateEvent, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                                    DECLARE_INSTANCE_METHOD(void, MultiplayerResultsViewController_didDeactivateEvent, bool removedFromHierarchy, bool screenSystemDisabling);
                                    DECLARE_INSTANCE_METHOD(void, MultiplayerLevelDidFinish, MultiplayerLevelScenesTransitionSetupDataSO* transitionSetupData, MultiplayerResultsData* results);
                                    DelegateUtils::DelegateW<MultiplayerResultsViewController::DidActivateDelegate> didActivateDelegate;
                                    DelegateUtils::DelegateW<MultiplayerResultsViewController::DidDeactivateDelegate> didDeactivateDelegate;)

#undef INTERFACES
