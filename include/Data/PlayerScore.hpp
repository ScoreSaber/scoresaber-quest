#pragma once

#include "Data/LeaderboardInfo.hpp"
#include "Data/Score.hpp"

namespace ScoreSaber::Data
{
    struct PlayerScore
    {
        PlayerScore(const rapidjson::Value&& value);
        PlayerScore(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value);
        PlayerScore(rapidjson::GenericObject<true, rapidjson::Value> value);
        PlayerScore(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);

        Score score;
        LeaderboardInfo leaderboard;
    };
}