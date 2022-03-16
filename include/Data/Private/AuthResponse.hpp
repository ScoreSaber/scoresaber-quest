#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include <string>

namespace ScoreSaber::Data::Private
{
    struct AuthResponse
    {
        AuthResponse(const rapidjson::Value&& value);
        AuthResponse(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value);
        AuthResponse(rapidjson::GenericObject<true, rapidjson::Value> value);
        AuthResponse(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);

        std::string a;
        std::string e;
    };
}