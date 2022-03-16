#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include <string>

namespace ScoreSaber::Data
{
    struct Badge
    {
        Badge(const rapidjson::Value&& value);
        Badge(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value);
        Badge(rapidjson::GenericObject<true, rapidjson::Value> value);
        Badge(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);

        std::string description;
        std::string image;
    };
}