#include "Data/PlayerScoreCollection.hpp"
#include <beatsaber-hook/shared/utils/utils-functions.h>

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
    PlayerScoreCollection::PlayerScoreCollection(const rapidjson::Document&& doc)
    {
        for (auto& playerScore : doc["playerScores"].GetArray())
        {
            emplace_back(playerScore.GetObject());
        }
        auto metadataItr = doc.FindMember("metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    PlayerScoreCollection::PlayerScoreCollection(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc)
    {
        for (auto& playerScore : doc[u"playerScores"].GetArray())
        {
            emplace_back(playerScore.GetObject());
        }
        auto metadataItr = doc.FindMember(u"metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    PlayerScoreCollection::PlayerScoreCollection(rapidjson::GenericObject<true, rapidjson::Value> doc)
    {
        for (auto& playerScore : doc["playerScores"].GetArray())
        {
            emplace_back(playerScore.GetObject());
        }
        auto metadataItr = doc.FindMember("metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    PlayerScoreCollection::PlayerScoreCollection(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc)
    {
        for (auto& playerScore : doc[u"playerScores"].GetArray())
        {
            emplace_back(playerScore.GetObject());
        }
        auto metadataItr = doc.FindMember(u"metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    PlayerScoreCollection::PlayerScoreCollection(StringW string)
        : PlayerScoreCollection(getdoc(string)){};

    PlayerScoreCollection::PlayerScoreCollection(std::string_view string)
        : PlayerScoreCollection(getdoc(string)){};

    PlayerScoreCollection::PlayerScoreCollection(std::u16string_view string)
        : PlayerScoreCollection(getdoc(string)){};

    PlayerScoreCollection::PlayerScoreCollection(){};
} // namespace ScoreSaber::Data