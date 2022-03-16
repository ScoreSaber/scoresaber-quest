#pragma once

#include "GlobalNamespace/PlatformLeaderboardViewController.hpp"

#include "Data/Score.hpp"
#include "HMUI/ModalView.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/UI/Button.hpp"

using namespace GlobalNamespace;

namespace ScoreSaber::UI::Other::ScoreSaberLeaderboardView
{
    enum PageDirection
    {
        Up,
        Down,
    };

    void ResetPage();
    void DidActivate(PlatformLeaderboardViewController* self,
                     bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling);
    void ChangeScope(bool filterAroundCountry);
    void RefreshLeaderboard(IDifficultyBeatmap* difficultyBeatmap, LeaderboardTableView* tableView, PlatformLeaderboardsModel::ScoresScope scope, LoadingControl* loadingControl, std::string refreshId);
    int GetPlayerScoreIndex(std::vector<Data::Score> scores);
    void SetErrorState(LoadingControl* loadingControl, std::string errorText = "Failed to load leaderboard, score won't upload", bool showRefreshButton = true);

        void DirectionalButtonClicked(PageDirection direction);
    void CheckPage();
} // namespace ScoreSaber::UI::Other::ScoreSaberLeaderboardView
