#include "Data/PlayerCollection.hpp"
#include "beatsaber-hook/shared/utils/utils-functions.h"

inline rapidjson::GenericDocument<rapidjson::UTF16<char16_t>> getdoc(std::u16string_view string)
{
    rapidjson::GenericDocument<rapidjson::UTF16<char16_t>> doc;
    doc.Parse(string.data());
    return doc;
}

inline rapidjson::Document getdoc(std::string_view string)
{
    rapidjson::Document doc;
    doc.Parse(string.data());
    return doc;
}

namespace ScoreSaber::Data
{
    PlayerCollection::PlayerCollection(const rapidjson::Document&& doc)
    {
        for (auto& player : doc["players"].GetArray())
        {
            emplace_back(player.GetObject());
        }
        auto metadataItr = doc.FindMember("metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    PlayerCollection::PlayerCollection(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc)
    {
        for (auto& player : doc[u"players"].GetArray())
        {
            emplace_back(player.GetObject());
        }
        auto metadataItr = doc.FindMember(u"metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    PlayerCollection::PlayerCollection(rapidjson::GenericObject<true, rapidjson::Value> doc)
    {
        for (auto& player : doc["players"].GetArray())
        {
            emplace_back(player.GetObject());
        }
        auto metadataItr = doc.FindMember("metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    PlayerCollection::PlayerCollection(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc)
    {
        for (auto& player : doc[u"players"].GetArray())
        {
            emplace_back(player.GetObject());
        }
        auto metadataItr = doc.FindMember(u"metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    PlayerCollection::PlayerCollection(Il2CppString* string) : PlayerCollection(getdoc(csstrtostr(string))){};

    PlayerCollection::PlayerCollection(std::string_view string) : PlayerCollection(getdoc(string)){};

    PlayerCollection::PlayerCollection(std::u16string_view string) : PlayerCollection(getdoc(string)){};

    PlayerCollection::PlayerCollection(){};
}
