#pragma once
#include <beatsaber-hook/shared/config/rapidjson-utils.hpp>

#include "Data/Metadata.hpp"
#include "Data/Score.hpp"
#include <optional>
#include <vector>
#include <beatsaber-hook/shared/utils/typedefs.h>

namespace ScoreSaber::Data
{
    struct ScoreCollection : std::vector<Score>
    {
        ScoreCollection();
        ScoreCollection(StringW string);
        ScoreCollection(std::string_view string);
        ScoreCollection(std::u16string_view string);
        ScoreCollection(const rapidjson::Document&& doc);
        ScoreCollection(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc);
        ScoreCollection(rapidjson::GenericObject<true, rapidjson::Value> doc);
        ScoreCollection(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc);
        ScoreCollection(rapidjson::GenericObject<false, rapidjson::Value> doc);
        ScoreCollection(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc);

        std::optional<Metadata> metadata;
    };
}