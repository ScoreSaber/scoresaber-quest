#pragma once

#include "Data/LeaderboardInfo.hpp"
#include "Data/Score.hpp"
#include "GlobalNamespace/PlatformLeaderboardViewController.hpp"
#include "HMUI/ModalView.hpp"
#include "UI/Other/Banner.hpp"
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

    extern ScoreSaber::UI::Other::Banner* ScoreSaberBanner;

    void ResetPage();
    void DidActivate(PlatformLeaderboardViewController* self,
                     bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling);
    void ChangeScope(bool filterAroundCountry);
    void RefreshLeaderboard(IDifficultyBeatmap* difficultyBeatmap, LeaderboardTableView* tableView, PlatformLeaderboardsModel::ScoresScope scope, LoadingControl* loadingControl, std::string refreshId);
    void SetRankedStatus(Data::LeaderboardInfo leaderboardInfo);
    int GetPlayerScoreIndex(std::vector<Data::Score> scores);
    void SetErrorState(LoadingControl* loadingControl, std::string errorText = "Failed to load leaderboard, score won't upload", bool showRefreshButton = true);

    void DirectionalButtonClicked(PageDirection direction);
    void SetPlayButtonState(bool state);
    void SetUploadState(bool state, bool success, std::string errorMessage = "<color=#89fc81>Upload failed</color>");
    void CheckPage();
} // namespace ScoreSaber::UI::Other::ScoreSaberLeaderboardView
