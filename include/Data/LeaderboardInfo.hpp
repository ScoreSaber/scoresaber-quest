#pragma once

#include "Data/Difficulty.hpp"
#include "Data/Score.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include <optional>
#include <vector>

namespace ScoreSaber::Data
{
    struct LeaderboardInfo
    {
        LeaderboardInfo();
        LeaderboardInfo(StringW string);
        LeaderboardInfo(std::string_view string);
        LeaderboardInfo(std::u16string_view string);
        LeaderboardInfo(const rapidjson::Document&& doc);
        LeaderboardInfo(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc);

        LeaderboardInfo(const rapidjson::Value&& value);
        LeaderboardInfo(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value);
        LeaderboardInfo(rapidjson::GenericObject<true, rapidjson::Value> value);
        LeaderboardInfo(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);
        LeaderboardInfo(rapidjson::GenericObject<false, rapidjson::Value> value);
        LeaderboardInfo(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);

        int id;
        std::u16string songHash;
        std::u16string songName;
        std::u16string songSubName;
        std::u16string songAuthorName;
        Difficulty difficulty;
        int maxScore;
        std::string createdDate;
        std::optional<std::string> rankedDate;
        std::optional<std::string> qualifiedDate;
        std::optional<std::string> lovedDate;
        bool ranked;
        bool qualified;
        bool loved;
        double maxPP;
        double stars;
        bool positiveModifiers;
        int plays;
        int dailyPlays;
        std::string coverImage;
        std::optional<Score> playerScore;
        std::vector<Difficulty> difficulties;
    };
}