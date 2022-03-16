#pragma once
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"

using namespace GlobalNamespace;
using namespace ScoreSaber;

namespace ScoreSaber::Services::LeaderboardService
{
    std::string GetLeaderboardUrl(IDifficultyBeatmap* difficultyBeatmap, PlatformLeaderboardsModel::ScoresScope scope, int page, bool filterAroundCountry);
    void GetLeaderboardData(IDifficultyBeatmap* difficultyBeatmap, PlatformLeaderboardsModel::ScoresScope scope, int page,
                            std::function<void(Data::InternalLeaderboard)> finished,
                            bool filterAroundCountry);

    Data::InternalLeaderboard GetLeaderboardError(std::string error);
    Data::InternalLeaderboard ParseLeaderboardData(std::string rawData, IDifficultyBeatmap* difficultyBeatmap, PlatformLeaderboardsModel::ScoresScope scope,
                                                   int page, bool filterAroundCountry);
}
