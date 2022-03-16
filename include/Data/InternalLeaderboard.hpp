#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "Data/Leaderboard.hpp"
#include "GlobalNamespace/LeaderboardTableView_ScoreData.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"
#include <vector>

using namespace GlobalNamespace;
namespace ScoreSaber::Data
{
    struct InternalLeaderboard
    {
        InternalLeaderboard();
        InternalLeaderboard(System::Collections::Generic::List_1<LeaderboardTableView::ScoreData*>* _leaderboardItems, std::optional<Leaderboard> _leaderboard = std::nullopt);

        System::Collections::Generic::List_1<LeaderboardTableView::ScoreData*>* leaderboardItems;

        std::optional<Leaderboard> leaderboard;
    };
}