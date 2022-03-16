#include "Data/Private/AuthResponse.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

namespace ScoreSaber::Data::Private
{
    AuthResponse::AuthResponse(const rapidjson::Value&& value)
    {
        a = value["a"].GetString();
        e = value["e"].GetString();
    }

    AuthResponse::AuthResponse(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value)
    {
        a = to_utf8(value[u"a"].GetString());
        e = to_utf8(value[u"e"].GetString());
    }

    AuthResponse::AuthResponse(rapidjson::GenericObject<true, rapidjson::Value> value)
    {
        a = value["a"].GetString();
        e = value["e"].GetString();
    }

    AuthResponse::AuthResponse(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value)
    {
        a = to_utf8(value[u"a"].GetString());
        e = to_utf8(value[u"e"].GetString());
    }
}