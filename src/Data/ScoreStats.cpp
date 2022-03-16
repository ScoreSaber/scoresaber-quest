#include "Data/ScoreStats.hpp"

namespace ScoreSaber::Data
{
    ScoreStats::ScoreStats(const rapidjson::Value&& value)
    {
        totalScore = (long)value["totalScore"].GetInt64();
        totalRankedScore = (long)value["totalRankedScore"].GetInt64();
        averageRankedAccuracy = value["averageRankedAccuracy"].GetDouble();
        totalPlayCount = value["totalPlayCount"].GetInt();
        rankedPlayCount = value["rankedPlayCount"].GetInt();
        replaysWatched = value["replaysWatched"].GetInt();
    }

    ScoreStats::ScoreStats(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value)
    {

        totalScore = (long)value[u"totalScore"].GetInt64();
        totalRankedScore = (long)value[u"totalRankedScore"].GetInt64();
        averageRankedAccuracy = value[u"averageRankedAccuracy"].GetDouble();
        totalPlayCount = value[u"totalPlayCount"].GetInt();
        rankedPlayCount = value[u"rankedPlayCount"].GetInt();
        replaysWatched = value[u"replaysWatched"].GetInt();
    }

    ScoreStats::ScoreStats(rapidjson::GenericObject<true, rapidjson::Value> value)
    {
        totalScore = (long)value["totalScore"].GetInt64();
        totalRankedScore = (long)value["totalRankedScore"].GetInt64();
        averageRankedAccuracy = value["averageRankedAccuracy"].GetDouble();
        totalPlayCount = value["totalPlayCount"].GetInt();
        rankedPlayCount = value["rankedPlayCount"].GetInt();
        replaysWatched = value["replaysWatched"].GetInt();
    }

    ScoreStats::ScoreStats(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value)
    {
        totalScore = (long)value[u"totalScore"].GetInt64();
        totalRankedScore = (long)value[u"totalRankedScore"].GetInt64();
        averageRankedAccuracy = value[u"averageRankedAccuracy"].GetDouble();
        totalPlayCount = value[u"totalPlayCount"].GetInt();
        rankedPlayCount = value[u"rankedPlayCount"].GetInt();
        replaysWatched = value[u"replaysWatched"].GetInt();
    }
}