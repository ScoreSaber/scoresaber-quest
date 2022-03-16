#include "Data/Difficulty.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

namespace ScoreSaber::Data
{
    Difficulty::Difficulty(const rapidjson::Value&& value)
    {
        // leaderboardId = value["leaderboardId"].GetInt();
        // difficulty = value["difficulty"].GetInt();
        // gameMode = value["gameMode"].GetString();
        // difficultyRaw = value["difficultyRaw"].GetString();
    }

    Difficulty::Difficulty(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value)
    {
        // leaderboardId = value[u"leaderboardId"].GetInt();
        // difficulty = value[u"difficulty"].GetInt();
        // gameMode = to_utf8(value[u"gameMode"].GetString());
        // difficultyRaw = to_utf8(value[u"difficultyRaw"].GetString());
    }

    Difficulty::Difficulty(rapidjson::GenericObject<true, rapidjson::Value> value)
    {
        // leaderboardId = value["leaderboardId"].GetInt();
        // difficulty = value["difficulty"].GetInt();
        // gameMode = value["gameMode"].GetString();
        // difficultyRaw = value["difficultyRaw"].GetString();
    }

    Difficulty::Difficulty(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value)
    {
        // leaderboardId = value[u"leaderboardId"].GetInt();
        // difficulty = value[u"difficulty"].GetInt();
        // gameMode = to_utf8(value[u"gameMode"].GetString());
        // difficultyRaw = to_utf8(value[u"difficultyRaw"].GetString());
    }
    Difficulty::Difficulty() {}
}