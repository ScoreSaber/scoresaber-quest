#include "Data/Badge.hpp"
#include <beatsaber-hook/shared/utils/utils.h>
#include <paper2_scotland2/shared/string_convert.hpp>

namespace ScoreSaber::Data
{
    Badge::Badge(const rapidjson::Value&& value)
    {
        description = value["description"].GetString();
        image = value["image"].GetString();
    }

    Badge::Badge(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value)
    {
        description = Paper::StringConvert::from_utf16(value[u"description"].GetString());
        image = Paper::StringConvert::from_utf16(value[u"image"].GetString());
    }

    Badge::Badge(rapidjson::GenericObject<true, rapidjson::Value> value)
    {
        description = value["description"].GetString();
        image = value["image"].GetString();
    }

    Badge::Badge(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value)
    {
        description = Paper::StringConvert::from_utf16(value[u"description"].GetString());
        image = Paper::StringConvert::from_utf16(value[u"image"].GetString());
    }
}