
#include "Sprites.hpp"
#include "hooks.hpp"

#include "UI/Other/ScoreSaberLeaderboardView.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/LeaderboardTableView_ScoreData.hpp"
#include "GlobalNamespace/LoadingControl.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_LeaderboardScore.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_ScoresScope.hpp"
#include "HMUI/SegmentedControl.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "logging.hpp"

using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace GlobalNamespace;
using namespace ScoreSaber;
using namespace ScoreSaber::UI::Other;

int _lastScopeIndex = 0;

MAKE_AUTO_HOOK_MATCH(
    PlatformLeaderboardViewController_DidActivate,
    &GlobalNamespace::PlatformLeaderboardViewController::DidActivate, void,
    GlobalNamespace::PlatformLeaderboardViewController* self,
    bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling)
{

    PlatformLeaderboardViewController_DidActivate(self, firstActivation, addedToHeirarchy, screenSystemEnabling);

    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::DidActivate(self, firstActivation, addedToHeirarchy, screenSystemEnabling);

    auto segmentedControl = reinterpret_cast<SegmentedControl*>(self->scopeSegmentedControl);
    segmentedControl->SelectCellWithNumber(_lastScopeIndex);
}

MAKE_AUTO_HOOK_MATCH(PlatformLeaderboardViewController_Refresh,
                     &GlobalNamespace::PlatformLeaderboardViewController::Refresh,
                     void, GlobalNamespace::PlatformLeaderboardViewController* self,
                     bool showLoadingIndicator, bool clear)
{

    // PlatformLeaderboardViewController_Refresh(self, showLoadingIndicator, clear);
    self->hasScoresData = false;
    self->leaderboardTableView->SetScores(System::Collections::Generic::List_1<LeaderboardTableView::ScoreData*>::New_ctor(), -1);
    LoadingControl* loadingControl = self->loadingControl;
    loadingControl->ShowLoading(System::String::_get_Empty());
    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::RefreshLeaderboard(self->difficultyBeatmap, self->leaderboardTableView, self->_get__scoresScope(), loadingControl, "lol");
}

MAKE_HOOK_MATCH(PlatformLeaderboardViewController_HandleScopeSegmentedControlDidSelectCell,
                &PlatformLeaderboardViewController::HandleScopeSegmentedControlDidSelectCell, void,
                PlatformLeaderboardViewController* self, SegmentedControl* segmentedControl, int cellNumber)
{

    bool filterAroundCountry = false;
    switch (cellNumber)
    {
        case 0: {
            self->_set__scoresScope(PlatformLeaderboardsModel::ScoresScope::Global);
            break;
        }
        case 1: {
            self->_set__scoresScope(PlatformLeaderboardsModel::ScoresScope::AroundPlayer);
            break;
        }
        case 2: {
            self->_set__scoresScope(PlatformLeaderboardsModel::ScoresScope::Friends);
            break;
        }
        case 3: {
            filterAroundCountry = true;
            break;
        }
    }
    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ChangeScope(filterAroundCountry);
}