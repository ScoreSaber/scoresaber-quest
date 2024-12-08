#include "Sprites.hpp"
#include "hooks.hpp"

#include "UI/Other/ScoreSaberLeaderboardView.hpp"

// LeaderboardScoreUploader

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/GameplayModifiers.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel.hpp>
#include "ReplaySystem/ReplayLoader.hpp"

// StandardLevelScenesTransitionSetupDataSO

#include <GlobalNamespace/LevelCompletionResults.hpp>
#include <GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp>
#include <GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp>

#include <GlobalNamespace/LeaderboardTableView.hpp>
#include <GlobalNamespace/LoadingControl.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel.hpp>
#include <HMUI/SegmentedControl.hpp>
#include "Services/UploadService.hpp"
#include <System/Guid.hpp>
#include "Utils/BeatmapUtils.hpp"
#include "Utils/StringUtils.hpp"

#include "CustomTypes/Components/CellClicker.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "ReplaySystem/Recorders/MainRecorder.hpp"
#include <beatsaber-hook/shared/utils/hooking.hpp>

#include <GlobalNamespace/MenuTransitionsHelper.hpp>
#include <UnityEngine/GameObject.hpp>

#include <bsml/shared/Helpers/getters.hpp>

#include "logging.hpp"

using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace GlobalNamespace;
using namespace ScoreSaber;
using namespace ScoreSaber::CustomTypes::Components;
using namespace ScoreSaber::Data::Private;
using namespace ScoreSaber::ReplaySystem;
using namespace ScoreSaber::UI::Other;

int _lastScopeIndex = 0;

MAKE_AUTO_HOOK_MATCH(
    PlatformLeaderboardViewController_DidActivate,
    &GlobalNamespace::PlatformLeaderboardViewController::DidActivate, void,
    GlobalNamespace::PlatformLeaderboardViewController* self,
    bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling)
{
    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::EarlyDidActivate(self, firstActivation, addedToHeirarchy, screenSystemEnabling);
    PlatformLeaderboardViewController_DidActivate(self, firstActivation, addedToHeirarchy, screenSystemEnabling);
    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::DidActivate(self, firstActivation, addedToHeirarchy, screenSystemEnabling);
    auto segmentedControl = self->_scopeSegmentedControl.cast<SegmentedControl>();
    segmentedControl->SelectCellWithNumber(_lastScopeIndex);
}

MAKE_AUTO_HOOK_MATCH(
    PlatformLeaderboardViewController_DidDeactivate,
    &GlobalNamespace::PlatformLeaderboardViewController::DidDeactivate, void,
    GlobalNamespace::PlatformLeaderboardViewController* self,
    bool removedFromHierarchy, bool screenSystemEnabling)
{
    PlatformLeaderboardViewController_DidDeactivate(self, removedFromHierarchy, screenSystemEnabling);
    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::DidDeactivate();
}

MAKE_AUTO_HOOK_MATCH(PlatformLeaderboardViewController_Refresh,
                     &GlobalNamespace::PlatformLeaderboardViewController::Refresh,
                     void, GlobalNamespace::PlatformLeaderboardViewController* self,
                     bool showLoadingIndicator, bool clear)
{
    for (auto holder : ScoreSaberLeaderboardView::_cellClickingImages) {
        CellClicker* existingCellClicker = holder->gameObject->GetComponent<CellClicker*>();
        if (existingCellClicker != nullptr) {
            GameObject::Destroy(existingCellClicker);
        }
    }

    self->_hasScoresData = false;
    self->_scores = System::Collections::Generic::List_1<LeaderboardTableView::ScoreData*>::New_ctor();
    self->_leaderboardTableView->SetScores(self->_scores, self->_playerScorePos[(int)self->_scoresScope]);
    self->_loadingControl->ShowLoading(System::String::Empty);
    BeatmapLevelsModel* _beatmapLevelsModel = BSML::Helpers::GetDiContainer()->Resolve<BeatmapLevelsModel*>();
    BeatmapLevel* beatmapLevel = _beatmapLevelsModel->GetBeatmapLevel(self->_beatmapKey.levelId);
    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::RefreshLeaderboard(beatmapLevel, self->_beatmapKey, self->_leaderboardTableView, self->_scoresScope, self->_loadingControl,
                                                                         System::Guid::NewGuid().ToString());
}

MAKE_AUTO_HOOK_MATCH(PlatformLeaderboardViewController_HandleScopeSegmentedControlDidSelectCell, &GlobalNamespace::PlatformLeaderboardViewController::HandleScopeSegmentedControlDidSelectCell, void,
                     PlatformLeaderboardViewController* self, SegmentedControl* segmentedControl, int cellNumber)
{

    bool filterAroundCountry = false;
    switch (cellNumber)
    {
        case 0: {
            self->__scoresScope = PlatformLeaderboardsModel::ScoresScope::Global;
            break;
        }
        case 1: {
            self->__scoresScope = PlatformLeaderboardsModel::ScoresScope::AroundPlayer;
            break;
        }
        case 2: {
            self->__scoresScope = PlatformLeaderboardsModel::ScoresScope::Friends;
            break;
        }
        case 3: {
            self->__scoresScope = PlatformLeaderboardsModel::ScoresScope::Global;
            filterAroundCountry = true;
            break;
        }
    }
    _lastScopeIndex = cellNumber;
    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ChangeScope(filterAroundCountry);
}

MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Finish, &GlobalNamespace::StandardLevelScenesTransitionSetupDataSO::Finish, void,
                     GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* self,
                     GlobalNamespace::LevelCompletionResults* levelCompletionResults)
{
    ScoreSaber::Services::UploadService::Three(self, levelCompletionResults);
    StandardLevelScenesTransitionSetupDataSO_Finish(self, levelCompletionResults);
}

MAKE_AUTO_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Finish, &GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO::Finish, void,
                     GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO* self,
                     GlobalNamespace::MultiplayerResultsData* multiplayerResultsData)
{
    ScoreSaber::Services::UploadService::Four(self, multiplayerResultsData);
    MultiplayerLevelScenesTransitionSetupDataSO_Finish(self, multiplayerResultsData);
}