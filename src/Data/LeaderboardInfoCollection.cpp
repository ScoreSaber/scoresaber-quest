#include "Data/LeaderboardInfoCollection.hpp"
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
    LeaderboardInfoCollection::LeaderboardInfoCollection(const rapidjson::Document&& doc)
    {
        for (auto& leaderboard : doc["leaderboards"].GetArray())
        {
            emplace_back(leaderboard.GetObject());
        }
        auto metadataItr = doc.FindMember("metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    LeaderboardInfoCollection::LeaderboardInfoCollection(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc)
    {
        for (auto& leaderboard : doc[u"leaderboards"].GetArray())
        {
            emplace_back(leaderboard.GetObject());
        }
        auto metadataItr = doc.FindMember(u"metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    LeaderboardInfoCollection::LeaderboardInfoCollection(rapidjson::GenericObject<true, rapidjson::Value> doc)
    {
        for (auto& leaderboard : doc["leaderboards"].GetArray())
        {
            emplace_back(leaderboard.GetObject());
        }
        auto metadataItr = doc.FindMember("metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    LeaderboardInfoCollection::LeaderboardInfoCollection(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc)
    {
        for (auto& leaderboard : doc[u"leaderboards"].GetArray())
        {
            emplace_back(leaderboard.GetObject());
        }
        auto metadataItr = doc.FindMember(u"metaData");
        if (metadataItr->value.IsObject())
        {
            metadata = Metadata(metadataItr->value.GetObject());
        }
    }

    LeaderboardInfoCollection::LeaderboardInfoCollection(StringW string)
        : LeaderboardInfoCollection(getdoc(csstrtostr(string))){};

    LeaderboardInfoCollection::LeaderboardInfoCollection(std::string_view string)
        : LeaderboardInfoCollection(getdoc(string)){};

    LeaderboardInfoCollection::LeaderboardInfoCollection(std::u16string_view string)
        : LeaderboardInfoCollection(getdoc(string)){};

    LeaderboardInfoCollection::LeaderboardInfoCollection(){};
} // namespace ScoreSaber::Data