#pragma once

#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"
#include "GlobalNamespace/PlatformLeaderboardViewController.hpp"
#include "GlobalNamespace/ServerPlayerListViewController.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "HMUI/ViewController_DidActivateDelegate.hpp"
#include "HMUI/ViewController_DidDeactivateDelegate.hpp"
#include "System/Action_2.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/IInitializable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

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
                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didActivateEvent, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                                    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didDeactivateEvent, bool removedFromHierarchy, bool screenSystemDisabling);
                                    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didChangeDifficultyBeatmapEvent, LevelSelectionNavigationController* controller, IDifficultyBeatmap* beatmap);
                                    DECLARE_INSTANCE_METHOD(void, LevelSelectionNavigationController_didChangeLevelDetailContentEvent, LevelSelectionNavigationController* controller, StandardLevelDetailViewController::ContentType contentType);
                                    LevelSelectionNavigationController::DidActivateDelegate* didActivateDelegate;
                                    LevelSelectionNavigationController::DidDeactivateDelegate* didDeactivateDelegate;
                                    System::Action_2<LevelSelectionNavigationController*, IDifficultyBeatmap*>* didChangeDifficultyBeatmapDelegate;
                                    System::Action_2<LevelSelectionNavigationController*, StandardLevelDetailViewController::ContentType>* didChangeLevelDetailContentDelegate;
                                    bool InMulti();
                                    void HideLeaderboard();
                                    void ShowLeaderboard();
                                    bool _currentlyInMulti;
                                    bool _performingFirstActivation;)

#undef INTERFACES
