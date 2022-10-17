#pragma once

#include "Data/Metadata.hpp"
#include "Data/PlayerScore.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include <optional>

namespace ScoreSaber::Data
{
    struct PlayerScoreCollection : std::vector<PlayerScore>
    {
        PlayerScoreCollection();
        PlayerScoreCollection(StringW string);
        PlayerScoreCollection(std::string_view string);
        PlayerScoreCollection(std::u16string_view string);
        PlayerScoreCollection(const rapidjson::Document&& doc);
        PlayerScoreCollection(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc);
        PlayerScoreCollection(rapidjson::GenericObject<true, rapidjson::Value> doc);
        PlayerScoreCollection(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc);

        std::optional<Metadata> metadata;
    };
}