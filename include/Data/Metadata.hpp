#pragma once
#include <beatsaber-hook/shared/config/rapidjson-utils.hpp>

namespace ScoreSaber::Data
{
    struct Metadata
    {
        Metadata(const rapidjson::Value&& value);
        Metadata(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value);
        Metadata(rapidjson::GenericObject<true, rapidjson::Value> value);
        Metadata(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value);

        int total;
        int page;
        int itemsPerPage;
    };
}