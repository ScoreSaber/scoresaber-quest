#include "UI/Multiplayer/ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager.hpp"

#include "GlobalNamespace/HMTask.hpp"
#include "GlobalNamespace/MultiplayerLevelSelectionFlowCoordinator.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "System/Action.hpp"
#include "UnityEngine/GameObject.hpp"
#include "custom-types/shared/delegate.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include <functional>

DEFINE_TYPE(ScoreSaber::UI::Multiplayer, ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager);

namespace ScoreSaber::UI::Multiplayer
{
    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::ctor(MainFlowCoordinator* mainFlowCoordinator, ServerPlayerListViewController* serverPlayerListViewController, PlatformLeaderboardViewController* platformLeaderboardViewController, LevelSelectionNavigationController* levelSelectionNavigationController)
    {
        _mainFlowCoordinator = mainFlowCoordinator;
        _serverPlayerListViewController = serverPlayerListViewController;
        _platformLeaderboardViewController = platformLeaderboardViewController;
        _levelSelectionNavigationController = levelSelectionNavigationController;
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::Initialize()
    {
        didActivateDelegate = custom_types::MakeDelegate<LevelSelectionNavigationController::DidActivateDelegate*>((std::function<void(bool, bool, bool)>)[&](bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) { LevelSelectionNavigationController_didActivateEvent(firstActivation, addedToHierarchy, screenSystemEnabling); });
        didDeactivateDelegate = custom_types::MakeDelegate<LevelSelectionNavigationController::DidDeactivateDelegate*>((std::function<void(bool, bool)>)[&](bool removedFromHierarchy, bool screenSystemDisabling) { LevelSelectionNavigationController_didDeactivateEvent(removedFromHierarchy, screenSystemDisabling); });

        _levelSelectionNavigationController->add_didActivateEvent(didActivateDelegate);
        _levelSelectionNavigationController->add_didDeactivateEvent(didDeactivateDelegate);
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::Dispose()
    {
        _levelSelectionNavigationController->remove_didActivateEvent(didActivateDelegate);
        // the next one is missing in the pcvr version, so I'll leave it out here aswell
        // _levelSelectionNavigationController->remove_didDeactivateEvent(didDeactivateDelegate);
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didActivateEvent(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (!InMulti())
            return;

        if (firstActivation)
            _performingFirstActivation = true;

        _currentlyInMulti = true;

        didChangeDifficultyBeatmapDelegate = custom_types::MakeDelegate<System::Action_2<LevelSelectionNavigationController*, IDifficultyBeatmap*>*>((std::function<void(LevelSelectionNavigationController*, IDifficultyBeatmap*)>)[&](LevelSelectionNavigationController * controller, IDifficultyBeatmap * beatmap) { LevelSelectionNavigationController_didChangeDifficultyBeatmapEvent(controller, beatmap); });
        didChangeLevelDetailContentDelegate = custom_types::MakeDelegate<System::Action_2<LevelSelectionNavigationController*, StandardLevelDetailViewController::ContentType>*>((std::function<void(LevelSelectionNavigationController*, StandardLevelDetailViewController::ContentType)>)[&](LevelSelectionNavigationController * controller, StandardLevelDetailViewController::ContentType contentType) { LevelSelectionNavigationController_didChangeLevelDetailContentEvent(controller, contentType); });

        _levelSelectionNavigationController->add_didChangeDifficultyBeatmapEvent(didChangeDifficultyBeatmapDelegate);
        _levelSelectionNavigationController->add_didChangeLevelDetailContentEvent(didChangeLevelDetailContentDelegate);
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didDeactivateEvent(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        if (!InMulti())
            return;

        _currentlyInMulti = false;
        _levelSelectionNavigationController->remove_didChangeDifficultyBeatmapEvent(didChangeDifficultyBeatmapDelegate);
        _levelSelectionNavigationController->remove_didChangeLevelDetailContentEvent(didChangeLevelDetailContentDelegate);
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didChangeLevelDetailContentEvent(LevelSelectionNavigationController* controller, StandardLevelDetailViewController::ContentType contentType)
    {
        if (controller->get_selectedDifficultyBeatmap() == nullptr)
        {
            HideLeaderboard();
            return;
        }

        ShowLeaderboard();
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::LevelSelectionNavigationController_didChangeDifficultyBeatmapEvent(LevelSelectionNavigationController* controller, IDifficultyBeatmap* beatmap)
    {
        ShowLeaderboard();
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::HideLeaderboard()
    {
        if (_platformLeaderboardViewController->get_isInViewControllerHierarchy())
        {
            auto currentFlowCoordinator = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
            if (!il2cpp_utils::try_cast<MultiplayerLevelSelectionFlowCoordinator>(currentFlowCoordinator).has_value())
                return;

            currentFlowCoordinator->SetRightScreenViewController(nullptr, HMUI::ViewController::AnimationType::Out);
        }
    }

    void ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::ShowLeaderboard()
    {
        if (!InMulti())
            return;

        auto selected = _levelSelectionNavigationController->get_selectedDifficultyBeatmap();
        if (selected == nullptr)
        {
            HideLeaderboard();
            return;
        }

        _platformLeaderboardViewController->SetData(selected);
        auto currentFlowCoordinator = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
        currentFlowCoordinator->SetRightScreenViewController(_platformLeaderboardViewController, HMUI::ViewController::AnimationType::In);

        _serverPlayerListViewController->get_gameObject()->SetActive(false); // copied from pcvr version: This is a bandaid fix, first time startup it gets stuck while animating kinda like the issue we had before (TODO: Fix in 2024)

        // Copied from the pcvr version, but still a bandaid
        if (_performingFirstActivation)
        {
            _performingFirstActivation = false;
            HMTask::New_ctor(custom_types::MakeDelegate<System::Action*>((std::function<void()>)[&] {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                QuestUI::MainThreadScheduler::Schedule([=]() {
                    _platformLeaderboardViewController->Refresh(true, true);
                });
            }), nullptr)->Run();
        }
    }

    bool ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager::InMulti()
    {
        if (_currentlyInMulti)
            return true;

        auto currentFlowCoordinator = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
        return il2cpp_utils::try_cast<MultiplayerLevelSelectionFlowCoordinator>(currentFlowCoordinator).has_value();
    }
} // namespace ScoreSaber::UI::Multiplayer