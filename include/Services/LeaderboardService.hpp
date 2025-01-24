#pragma once
#include <GlobalNamespace/PlatformLeaderboardsModel.hpp>
#include <GlobalNamespace/BeatmapKey.hpp>
#include "Data/InternalLeaderboard.hpp"

namespace ScoreSaber::Services::LeaderboardService
{
    std::string GetLeaderboardUrl(GlobalNamespace::BeatmapKey beatmapKey, GlobalNamespace::PlatformLeaderboardsModel::ScoresScope scope, int page, bool filterAroundCountry);
    void GetLeaderboardData(int maxMultipliedScore, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, GlobalNamespace::PlatformLeaderboardsModel::ScoresScope scope, int page,
                            std::function<void(ScoreSaber::Data::InternalLeaderboard)> finished,
                            bool filterAroundCountry);

    ScoreSaber::Data::InternalLeaderboard GetLeaderboardError(std::string error);
    ScoreSaber::Data::InternalLeaderboard ParseLeaderboardData(std::string rawData, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, PlatformLeaderboardsModel::ScoresScope scope,
                                                   int page, bool filterAroundCountry, int maxScore);
} // namespace ScoreSaber::Services::LeaderboardService
