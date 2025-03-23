#pragma once
#include <beatsaber-hook/shared/config/rapidjson-utils.hpp>

#include "Data/Leaderboard.hpp"
#include <GlobalNamespace/LeaderboardTableView.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel.hpp>
#include <vector>

using namespace GlobalNamespace;
namespace ScoreSaber::Data
{
    struct InternalLeaderboard
    {
        InternalLeaderboard();
        InternalLeaderboard(ListW<LeaderboardTableView::ScoreData*> _leaderboardItems, std::vector<std::string> _profilePictures, std::optional<Leaderboard> _leaderboard = std::nullopt);

        ListW<LeaderboardTableView::ScoreData*> leaderboardItems;
        std::vector<std::string> profilePictures;
        std::optional<Leaderboard> leaderboard;
    };
}