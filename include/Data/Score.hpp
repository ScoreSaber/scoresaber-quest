#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "Data/LeaderboardPlayer.hpp"
namespace ScoreSaber::Data
{
    struct Score
    {
        Score(const rapidjson::Value&& value);
        Score(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value);
        Score(rapidjson::GenericObject<true, rapidjson::Value> value);
        Score(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);

        int id;
        LeaderboardPlayer leaderboardPlayerInfo;
        int rank;
        int baseScore;
        int modifiedScore;
        double pp;
        double weight;
        std::string modifiers;
        double multiplier;
        int badCuts;
        int missedNotes;
        int maxCombo;
        bool fullCombo;
        int hmd;
        bool hasReplay;
        std::string timeSet;
    };
}