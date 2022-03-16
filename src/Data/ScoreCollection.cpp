#include "Data/ScoreCollection.hpp"
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
    ScoreCollection::ScoreCollection(const rapidjson::Document&& doc)
    {
        for (auto& score : doc["scores"].GetArray())
        {
            emplace_back(score.GetObject());
        }
        auto metadataItr = doc.FindMember("metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    ScoreCollection::ScoreCollection(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc)
    {
        for (auto& score : doc[u"scores"].GetArray())
        {
            emplace_back(score.GetObject());
        }
        auto metadataItr = doc.FindMember(u"metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    ScoreCollection::ScoreCollection(rapidjson::GenericObject<true, rapidjson::Value> doc)
    {
        for (auto& score : doc["scores"].GetArray())
        {
            emplace_back(score.GetObject());
        }
        auto metadataItr = doc.FindMember("metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    ScoreCollection::ScoreCollection(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc)
    {
        for (auto& score : doc[u"scores"].GetArray())
        {
            emplace_back(score.GetObject());
        }
        auto metadataItr = doc.FindMember(u"metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    ScoreCollection::ScoreCollection(rapidjson::GenericObject<false, rapidjson::Value> doc)
    {
        for (auto& score : doc["scores"].GetArray())
        {
            emplace_back(score.GetObject());
        }
        auto metadataItr = doc.FindMember("metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    ScoreCollection::ScoreCollection(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc)
    {
        for (auto& score : doc[u"scores"].GetArray())
        {
            emplace_back(score.GetObject());
        }
        auto metadataItr = doc.FindMember(u"metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    ScoreCollection::ScoreCollection(Il2CppString* string) : ScoreCollection(getdoc(csstrtostr(string))){};

    ScoreCollection::ScoreCollection(std::string_view string) : ScoreCollection(getdoc(string)){};

    ScoreCollection::ScoreCollection(std::u16string_view string) : ScoreCollection(getdoc(string)){};

    ScoreCollection::ScoreCollection(){};
}