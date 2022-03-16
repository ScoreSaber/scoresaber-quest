#include "Data/Badge.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

namespace ScoreSaber::Data
{
    Badge::Badge(const rapidjson::Value&& value)
    {
        description = value["description"].GetString();
        image = value["image"].GetString();
    }

    Badge::Badge(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value)
    {
        description = to_utf8(value[u"description"].GetString());
        image = to_utf8(value[u"image"].GetString());
    }

    Badge::Badge(rapidjson::GenericObject<true, rapidjson::Value> value)
    {
        description = value["description"].GetString();
        image = value["image"].GetString();
    }

    Badge::Badge(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value)
    {
        description = to_utf8(value[u"description"].GetString());
        image = to_utf8(value[u"image"].GetString());
    }
}