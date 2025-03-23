#pragma once

#include "Data/Metadata.hpp"
#include "Data/Player.hpp"
#include <beatsaber-hook/shared/utils/typedefs.h>

namespace ScoreSaber::Data
{
    struct PlayerCollection : std::vector<Player>
    {
        PlayerCollection();
        PlayerCollection(StringW string);
        PlayerCollection(std::string_view string);
        PlayerCollection(std::u16string_view string);
        PlayerCollection(const rapidjson::Document&& doc);
        PlayerCollection(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc);
        PlayerCollection(rapidjson::GenericObject<true, rapidjson::Value> doc);
        PlayerCollection(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc);

        std::optional<Metadata> metadata;
    };
}