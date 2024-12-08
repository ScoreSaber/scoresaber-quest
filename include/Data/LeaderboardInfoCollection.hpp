#pragma once

#include "Data/LeaderboardInfo.hpp"
#include "Data/Metadata.hpp"
#include <beatsaber-hook/shared/utils/typedefs.h>
#include <optional>

namespace ScoreSaber::Data
{
    struct LeaderboardInfoCollection : std::vector<LeaderboardInfo>
    {
        LeaderboardInfoCollection();
        LeaderboardInfoCollection(StringW string);
        LeaderboardInfoCollection(std::string_view string);
        LeaderboardInfoCollection(std::u16string_view string);
        LeaderboardInfoCollection(const rapidjson::Document&& doc);
        LeaderboardInfoCollection(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc);
        LeaderboardInfoCollection(rapidjson::GenericObject<true, rapidjson::Value> doc);
        LeaderboardInfoCollection(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc);

        std::optional<Metadata> metadata;
    };
} // namespace ScoreSaber::Data