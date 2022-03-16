#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include <string>

namespace ScoreSaber::Data
{
    struct ScoreStats
    {
        ScoreStats(const rapidjson::Value&& value);
        ScoreStats(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value);
        ScoreStats(rapidjson::GenericObject<true, rapidjson::Value> value);
        ScoreStats(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);

        long totalScore;
        long totalRankedScore;
        double averageRankedAccuracy;
        int totalPlayCount;
        int rankedPlayCount;
        int replaysWatched;
    };
}