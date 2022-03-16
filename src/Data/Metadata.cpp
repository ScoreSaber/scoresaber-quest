#include "Data/Metadata.hpp"

namespace ScoreSaber::Data
{
    Metadata::Metadata(const rapidjson::Value&& value)
    {
        total = value["total"].GetInt();
        page = value["page"].GetInt();
        itemsPerPage = value["itemsPerPage"].GetInt();
    }

    Metadata::Metadata(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value)
    {
        total = value[u"total"].GetInt();
        page = value[u"page"].GetInt();
        itemsPerPage = value[u"itemsPerPage"].GetInt();
    }

    Metadata::Metadata(rapidjson::GenericObject<true, rapidjson::Value> value)
    {
        total = value["total"].GetInt();
        page = value["page"].GetInt();
        itemsPerPage = value["itemsPerPage"].GetInt();
    }

    Metadata::Metadata(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value)
    {
        total = value[u"total"].GetInt();
        page = value[u"page"].GetInt();
        itemsPerPage = value[u"itemsPerPage"].GetInt();
    }
}