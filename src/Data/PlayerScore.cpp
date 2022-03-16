#include "Data/PlayerScore.hpp"

namespace ScoreSaber::Data
{
    PlayerScore::PlayerScore(const rapidjson::Value&& value) : score(value["score"].GetObject()), leaderboard(value["leaderboard"].GetObject()){};

    PlayerScore::PlayerScore(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value) : score(value[u"score"].GetObject()), leaderboard(value[u"leaderboard"].GetObject()){};

    PlayerScore::PlayerScore(rapidjson::GenericObject<true, rapidjson::Value> value) : score(value["score"].GetObject()), leaderboard(value["leaderboard"].GetObject()){};

    PlayerScore::PlayerScore(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value) : score(value[u"score"].GetObject()), leaderboard(value[u"leaderboard"].GetObject()){};

}