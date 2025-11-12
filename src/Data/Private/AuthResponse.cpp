#include "Data/Private/AuthResponse.hpp"
#include <beatsaber-hook/shared/utils/utils.h>
#include <paper2_scotland2/shared/string_convert.hpp>

namespace ScoreSaber::Data::Private
{
    AuthResponse::AuthResponse(const rapidjson::Value&& value)
    {
        a = value["a"].GetString();
        e = value["e"].GetString();
    }

    AuthResponse::AuthResponse(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value)
    {
        a = Paper::StringConvert::from_utf16(value[u"a"].GetString());
        e = Paper::StringConvert::from_utf16(value[u"e"].GetString());
    }

    AuthResponse::AuthResponse(rapidjson::GenericObject<true, rapidjson::Value> value)
    {
        a = value["a"].GetString();
        e = value["e"].GetString();
    }

    AuthResponse::AuthResponse(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value)
    {
        a = Paper::StringConvert::from_utf16(value[u"a"].GetString());
        e = Paper::StringConvert::from_utf16(value[u"e"].GetString());
    }
}