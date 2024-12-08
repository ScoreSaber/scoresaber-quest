#pragma once

#include "CustomTypes/Components/LeaderboardScoreInfoButtonHandler.hpp"
#include "Data/LeaderboardInfo.hpp"
#include "Data/Score.hpp"
#include <GlobalNamespace/PlatformLeaderboardViewController.hpp>
#include <HMUI/ImageView.hpp>
#include <HMUI/ModalView.hpp>
#include "UI/Other/Banner.hpp"
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/UI/Button.hpp>

using namespace GlobalNamespace;

namespace ScoreSaber::UI::Other::ScoreSaberLeaderboardView
{
    enum PageDirection
    {
        Up,
        Down,
    };

    extern ScoreSaber::UI::Other::Banner* ScoreSaberBanner;
    extern ScoreSaber::CustomTypes::Components::LeaderboardScoreInfoButtonHandler* leaderboardScoreInfoButtonHandler;
    extern std::vector<HMUI::ImageView*> _cellClickingImages;

    void OnSoftRestart();

    void EarlyDidActivate(PlatformLeaderboardViewController* self,
                     bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling);
    void DidActivate(PlatformLeaderboardViewController* self,
                     bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling);
    void DidDeactivate();
    void ChangeScope(bool filterAroundCountry);
    void RefreshLeaderboard();
    void RefreshLeaderboard(BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, LeaderboardTableView* tableView, PlatformLeaderboardsModel::ScoresScope scope, LoadingControl* loadingControl, std::string refreshId);
    void SetRankedStatus(Data::LeaderboardInfo leaderboardInfo);
    int GetPlayerScoreIndex(std::vector<Data::Score> scores);
    void SetErrorState(LoadingControl* loadingControl, std::string errorText = "Failed to load leaderboard, score won't upload", bool showRefreshButton = true);

    void DirectionalButtonClicked(PageDirection direction);
    void SetPlayButtonState(bool state);
    void SetUploadState(bool state, bool success, std::string errorMessage = "<color=#fc8181>Upload failed</color>");
    void CheckPage();

    void AllowReplayWatching(bool value);
    bool IsReplayWatchingAllowed();

} // namespace ScoreSaber::UI::Other::ScoreSaberLeaderboardView
