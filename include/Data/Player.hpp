#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "Data/Badge.hpp"
#include "Data/ScoreStats.hpp"

#include <optional>
#include <string>
#include <vector>

namespace ScoreSaber::Data
{
    struct Player
    {
        Player();
        Player(std::string _id);
        Player(const rapidjson::Value&& value);
        Player(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value);
        Player(rapidjson::GenericObject<true, rapidjson::Value> value);
        Player(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);

        std::string id;
        std::u16string name;
        std::string profilePicture;
        std::string country;
        double pp;
        int rank;
        int countryRank;
        std::string role;
        std::vector<Badge> badges;
        std::vector<int> histories;
        std::optional<::ScoreSaber::Data::ScoreStats> scoreStats;
        int permissions;
        bool banned;
        bool inactive;
    };
}