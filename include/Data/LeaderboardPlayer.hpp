#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include <string>

namespace ScoreSaber::Data
{
    struct LeaderboardPlayer
    {
        LeaderboardPlayer(const rapidjson::Value&& value);
        LeaderboardPlayer(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value);
        LeaderboardPlayer(rapidjson::GenericObject<true, rapidjson::Value> value);
        LeaderboardPlayer(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);

        std::optional<std::string> id;
        std::optional<std::u16string> name;
        std::string profilePicture;
        std::optional<std::string> country;
        std::optional<int> permissions;
        std::optional<std::string> role;
    };
}