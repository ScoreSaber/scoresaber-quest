#include "Data/LeaderboardInfo.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

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
    LeaderboardInfo::LeaderboardInfo(const rapidjson::Document&& doc) : difficulty(doc["difficulty"].GetObject())
    {
        id = doc["id"].GetInt();
        songHash = to_utf16(doc["songHash"].GetString());
        songName = to_utf16(doc["songName"].GetString());
        songSubName = to_utf16(doc["songSubName"].GetString());
        songAuthorName = to_utf16(doc["songAuthorName"].GetString());
        maxScore = doc["maxScore"].GetInt();
        createdDate = doc["createdDate"].GetString();
        auto rankedDateItr = doc.FindMember("rankedDate");
        if (!rankedDateItr->value.IsNull())
        {
            rankedDate = std::make_optional(std::string(rankedDateItr->value.GetString()));
        }
        auto qualifiedDateItr = doc.FindMember("qualifiedDate");
        if (!qualifiedDateItr->value.IsNull())
        {
            qualifiedDate = std::make_optional(std::string(qualifiedDateItr->value.GetString()));
        }
        auto lovedDateItr = doc.FindMember("lovedDate");
        if (!lovedDateItr->value.IsNull())
        {
            lovedDate = std::make_optional(std::string(lovedDateItr->value.GetString()));
        }
        ranked = doc["ranked"].GetBool();
        qualified = doc["qualified"].GetBool();
        loved = doc["loved"].GetBool();
        maxPP = doc["maxPP"].GetDouble();
        stars = doc["stars"].GetDouble();
        positiveModifiers = doc["positiveModifiers"].GetBool();
        plays = doc["plays"].GetInt();
        dailyPlays = doc["dailyPlays"].GetInt();
        coverImage = doc["coverImage"].GetString();

        auto playerScoreItr = doc.FindMember("playerScore");
        if (!playerScoreItr->value.IsNull())
        {
            playerScore = std::make_optional(Score(playerScoreItr->value.GetObject()));
        }

        auto difficultiesItr = doc.FindMember("difficulties");

        if (!difficultiesItr->value.IsNull())
        {
            for (auto& diff : doc["difficulties"].GetArray())
            {
                difficulties.emplace_back(diff.GetObject());
            }
        }
    }

    LeaderboardInfo::LeaderboardInfo(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc) : difficulty(doc[u"difficulty"].GetObject())
    {
        id = doc[u"id"].GetInt();
        songHash = std::u16string(doc[u"songHash"].GetString());
        songName = std::u16string(doc[u"songName"].GetString());
        songSubName = std::u16string(doc[u"songSubName"].GetString());
        songAuthorName = std::u16string(doc[u"songAuthorName"].GetString());
        maxScore = doc[u"maxScore"].GetInt();
        createdDate = to_utf8(doc[u"createdDate"].GetString());
        auto rankedDateItr = doc.FindMember(u"rankedDate");
        if (!rankedDateItr->value.IsNull())
        {
            rankedDate = std::make_optional(to_utf8(rankedDateItr->value.GetString()));
        }
        auto qualifiedDateItr = doc.FindMember(u"qualifiedDate");
        if (!qualifiedDateItr->value.IsNull())
        {
            qualifiedDate = std::make_optional(to_utf8(qualifiedDateItr->value.GetString()));
        }
        auto lovedDateItr = doc.FindMember(u"lovedDate");
        if (!lovedDateItr->value.IsNull())
        {
            lovedDate = std::make_optional(to_utf8(lovedDateItr->value.GetString()));
        }
        ranked = doc[u"ranked"].GetBool();
        qualified = doc[u"qualified"].GetBool();
        loved = doc[u"loved"].GetBool();
        maxPP = doc[u"maxPP"].GetDouble();
        stars = doc[u"stars"].GetDouble();
        positiveModifiers = doc[u"positiveModifiers"].GetBool();
        plays = doc[u"plays"].GetInt();
        dailyPlays = doc[u"dailyPlays"].GetInt();
        coverImage = to_utf8(doc[u"coverImage"].GetString());

        auto playerScoreItr = doc.FindMember(u"playerScore");
        if (!playerScoreItr->value.IsNull())
        {
            playerScore = std::make_optional(Score(playerScoreItr->value.GetObject()));
        }

        auto difficultiesItr = doc.FindMember(u"difficulties");

        if (!difficultiesItr->value.IsNull())
        {
            for (auto& diff : doc[u"difficulties"].GetArray())
            {
                difficulties.emplace_back(diff.GetObject());
            }
        }
    }

    LeaderboardInfo::LeaderboardInfo(const rapidjson::Value&& value) : difficulty(value["difficulty"].GetObject())
    {
        id = value["id"].GetInt();
        songHash = to_utf16(value["songHash"].GetString());
        songName = to_utf16(value["songName"].GetString());
        songSubName = to_utf16(value["songSubName"].GetString());
        songAuthorName = to_utf16(value["songAuthorName"].GetString());
        maxScore = value["maxScore"].GetInt();
        createdDate = value["createdDate"].GetString();
        auto rankedDateItr = value.FindMember("rankedDate");

        if (!rankedDateItr->value.IsNull())
        {
            rankedDate = std::make_optional(std::string(rankedDateItr->value.GetString()));
        }

        auto qualifiedDateItr = value.FindMember("qualifiedDate");

        if (!qualifiedDateItr->value.IsNull())
        {
            qualifiedDate = std::make_optional(std::string(qualifiedDateItr->value.GetString()));
        }

        auto lovedDateItr = value.FindMember("lovedDate");

        if (!lovedDateItr->value.IsNull())
        {
            lovedDate = std::make_optional(std::string(lovedDateItr->value.GetString()));
        }

        ranked = value["ranked"].GetBool();
        qualified = value["qualified"].GetBool();
        loved = value["loved"].GetBool();
        maxPP = value["maxPP"].GetDouble();
        stars = value["stars"].GetDouble();
        positiveModifiers = value["positiveModifiers"].GetBool();
        plays = value["plays"].GetInt();
        dailyPlays = value["dailyPlays"].GetInt();
        coverImage = value["coverImage"].GetString();

        auto playerScoreItr = value.FindMember("playerScore");

        if (!playerScoreItr->value.IsNull())
        {
            playerScore = std::make_optional(Score(playerScoreItr->value.GetObject()));
        }

        auto difficultiesItr = value.FindMember("difficulties");
        if (!difficultiesItr->value.IsNull())
        {
            for (auto& diff : value["difficulties"].GetArray())
            {
                difficulties.emplace_back(diff.GetObject());
            }
        }
    }

    LeaderboardInfo::LeaderboardInfo(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value) : difficulty(value[u"difficulty"].GetObject())
    {
        id = value[u"id"].GetInt();
        songHash = std::u16string(value[u"songHash"].GetString());
        songName = std::u16string(value[u"songName"].GetString());
        songSubName = std::u16string(value[u"songSubName"].GetString());
        songAuthorName = std::u16string(value[u"songAuthorName"].GetString());
        maxScore = value[u"maxScore"].GetInt();
        createdDate = to_utf8(value[u"createdDate"].GetString());
        auto rankedDateItr = value.FindMember(u"rankedDate");

        if (!rankedDateItr->value.IsNull())
        {
            rankedDate = std::make_optional(to_utf8(rankedDateItr->value.GetString()));
        }

        auto qualifiedDateItr = value.FindMember(u"qualifiedDate");
        if (!qualifiedDateItr->value.IsNull())

        {
            qualifiedDate = std::make_optional(to_utf8(qualifiedDateItr->value.GetString()));
        }

        auto lovedDateItr = value.FindMember(u"lovedDate");

        if (!lovedDateItr->value.IsNull())
        {
            lovedDate = std::make_optional(to_utf8(lovedDateItr->value.GetString()));
        }

        ranked = value[u"ranked"].GetBool();
        qualified = value[u"qualified"].GetBool();
        loved = value[u"loved"].GetBool();
        maxPP = value[u"maxPP"].GetDouble();
        stars = value[u"stars"].GetDouble();
        positiveModifiers = value[u"positiveModifiers"].GetBool();
        plays = value[u"plays"].GetInt();
        dailyPlays = value[u"dailyPlays"].GetInt();
        coverImage = to_utf8(value[u"coverImage"].GetString());

        auto playerScoreItr = value.FindMember(u"playerScore");

        if (!playerScoreItr->value.IsNull())
        {
            playerScore = std::make_optional(Score(playerScoreItr->value.GetObject()));
        }

        auto difficultiesItr = value.FindMember(u"difficulties");
        if (!difficultiesItr->value.IsNull())
        {
            for (auto& diff : value[u"difficulties"].GetArray())
            {
                difficulties.emplace_back(diff.GetObject());
            }
        }
    }

    LeaderboardInfo::LeaderboardInfo(rapidjson::GenericObject<true, rapidjson::Value> value) : difficulty(value["difficulty"].GetObject())
    {
        id = value["id"].GetInt();
        songHash = to_utf16(value["songHash"].GetString());
        songName = to_utf16(value["songName"].GetString());
        songSubName = to_utf16(value["songSubName"].GetString());
        songAuthorName = to_utf16(value["songAuthorName"].GetString());
        maxScore = value["maxScore"].GetInt();
        createdDate = value["createdDate"].GetString();
        auto rankedDateItr = value.FindMember("rankedDate");

        if (!rankedDateItr->value.IsNull())
        {
            rankedDate = std::make_optional(std::string(rankedDateItr->value.GetString()));
        }

        auto qualifiedDateItr = value.FindMember("qualifiedDate");

        if (!qualifiedDateItr->value.IsNull())
        {
            qualifiedDate = std::make_optional(std::string(qualifiedDateItr->value.GetString()));
        }

        auto lovedDateItr = value.FindMember("lovedDate");

        if (!lovedDateItr->value.IsNull())
        {
            lovedDate = std::make_optional(std::string(lovedDateItr->value.GetString()));
        }

        ranked = value["ranked"].GetBool();
        qualified = value["qualified"].GetBool();
        loved = value["loved"].GetBool();
        maxPP = value["maxPP"].GetDouble();
        stars = value["stars"].GetDouble();
        positiveModifiers = value["positiveModifiers"].GetBool();
        plays = value["plays"].GetInt();
        dailyPlays = value["dailyPlays"].GetInt();
        coverImage = value["coverImage"].GetString();

        auto playerScoreItr = value.FindMember("playerScore");

        if (!playerScoreItr->value.IsNull())
        {
            playerScore = std::make_optional(Score(playerScoreItr->value.GetObject()));
        }

        auto difficultiesItr = value.FindMember("difficulties");
        if (!difficultiesItr->value.IsNull())
        {
            for (auto& diff : value["difficulties"].GetArray())
            {
                difficulties.emplace_back(diff.GetObject());
            }
        }
    }

    LeaderboardInfo::LeaderboardInfo(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value) : difficulty(value[u"difficulty"].GetObject())
    {
        id = value[u"id"].GetInt();
        songHash = std::u16string(value[u"songHash"].GetString());
        songName = std::u16string(value[u"songName"].GetString());
        songSubName = std::u16string(value[u"songSubName"].GetString());
        songAuthorName = std::u16string(value[u"songAuthorName"].GetString());
        maxScore = value[u"maxScore"].GetInt();
        createdDate = to_utf8(value[u"createdDate"].GetString());
        auto rankedDateItr = value.FindMember(u"rankedDate");

        if (!rankedDateItr->value.IsNull())
        {
            rankedDate = std::make_optional(to_utf8(rankedDateItr->value.GetString()));
        }

        auto qualifiedDateItr = value.FindMember(u"qualifiedDate");

        if (!qualifiedDateItr->value.IsNull())
        {
            qualifiedDate = std::make_optional(to_utf8(qualifiedDateItr->value.GetString()));
        }

        auto lovedDateItr = value.FindMember(u"lovedDate");

        if (!lovedDateItr->value.IsNull())
        {
            lovedDate = std::make_optional(to_utf8(lovedDateItr->value.GetString()));
        }

        ranked = value[u"ranked"].GetBool();
        qualified = value[u"qualified"].GetBool();
        loved = value[u"loved"].GetBool();
        maxPP = value[u"maxPP"].GetDouble();
        stars = value[u"stars"].GetDouble();
        positiveModifiers = value[u"positiveModifiers"].GetBool();
        plays = value[u"plays"].GetInt();
        dailyPlays = value[u"dailyPlays"].GetInt();
        coverImage = to_utf8(value[u"coverImage"].GetString());
        auto playerScoreItr = value.FindMember(u"playerScore");

        if (!playerScoreItr->value.IsNull())
        {
            playerScore = std::make_optional(Score(playerScoreItr->value.GetObject()));
        }

        auto difficultiesItr = value.FindMember(u"difficulties");
        if (!difficultiesItr->value.IsNull())
        {
            for (auto& diff : value[u"difficulties"].GetArray())
            {
                difficulties.emplace_back(diff.GetObject());
            }
        }
    }

    LeaderboardInfo::LeaderboardInfo(rapidjson::GenericObject<false, rapidjson::Value> value) : difficulty(value["difficulty"].GetObject())
    {

        // This is the one that gets called

        id = value["id"].GetInt();
        songHash = to_utf16(value["songHash"].GetString());
        songName = to_utf16(value["songName"].GetString());
        songSubName = to_utf16(value["songSubName"].GetString());
        songAuthorName = to_utf16(value["songAuthorName"].GetString());
        maxScore = value["maxScore"].GetInt();
        createdDate = value["createdDate"].GetString();
        auto rankedDateItr = value.FindMember("rankedDate");

        if (!rankedDateItr->value.IsNull())
        {
            rankedDate = std::make_optional(std::string(rankedDateItr->value.GetString()));
        }

        auto qualifiedDateItr = value.FindMember("qualifiedDate");

        if (!qualifiedDateItr->value.IsNull())
        {
            qualifiedDate = std::make_optional(std::string(qualifiedDateItr->value.GetString()));
        }

        auto lovedDateItr = value.FindMember("lovedDate");

        if (!lovedDateItr->value.IsNull())
        {
            lovedDate = std::make_optional(std::string(lovedDateItr->value.GetString()));
        }

        ranked = value["ranked"].GetBool();
        qualified = value["qualified"].GetBool();
        loved = value["loved"].GetBool();
        maxPP = value["maxPP"].GetDouble();
        stars = value["stars"].GetDouble();
        positiveModifiers = value["positiveModifiers"].GetBool();
        plays = value["plays"].GetInt();
        dailyPlays = value["dailyPlays"].GetInt();
        coverImage = value["coverImage"].GetString();

        auto playerScoreItr = value.FindMember("playerScore");
        if (!playerScoreItr->value.IsNull())
        {
            playerScore = std::make_optional(Score(playerScoreItr->value.GetObject()));
        }

        auto difficultiesItr = value.FindMember("difficulties");

        if (!difficultiesItr->value.IsNull())
        {
            for (auto& diff : value["difficulties"].GetArray())
            {
                difficulties.emplace_back(diff.GetObject());
            }
        }
    }

    LeaderboardInfo::LeaderboardInfo(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value) : difficulty(value[u"difficulty"].GetObject())
    {
        id = value[u"id"].GetInt();
        songHash = std::u16string(value[u"songHash"].GetString());
        songName = std::u16string(value[u"songName"].GetString());
        songSubName = std::u16string(value[u"songSubName"].GetString());
        songAuthorName = std::u16string(value[u"songAuthorName"].GetString());
        maxScore = value[u"maxScore"].GetInt();
        createdDate = to_utf8(value[u"createdDate"].GetString());
        auto rankedDateItr = value.FindMember(u"rankedDate");

        if (!rankedDateItr->value.IsNull())
        {
            rankedDate = std::make_optional(to_utf8(rankedDateItr->value.GetString()));
        }

        auto qualifiedDateItr = value.FindMember(u"qualifiedDate");

        if (!qualifiedDateItr->value.IsNull())
        {
            qualifiedDate = std::make_optional(to_utf8(qualifiedDateItr->value.GetString()));
        }

        auto lovedDateItr = value.FindMember(u"lovedDate");

        if (!lovedDateItr->value.IsNull())
        {
            lovedDate = std::make_optional(to_utf8(lovedDateItr->value.GetString()));
        }

        ranked = value[u"ranked"].GetBool();
        qualified = value[u"qualified"].GetBool();
        loved = value[u"loved"].GetBool();
        maxPP = value[u"maxPP"].GetDouble();
        stars = value[u"stars"].GetDouble();
        positiveModifiers = value[u"positiveModifiers"].GetBool();
        plays = value[u"plays"].GetInt();
        dailyPlays = value[u"dailyPlays"].GetInt();
        coverImage = to_utf8(value[u"coverImage"].GetString());

        auto playerScoreItr = value.FindMember(u"playerScore");

        if (!playerScoreItr->value.IsNull())
        {
            playerScore = std::make_optional(Score(playerScoreItr->value.GetObject()));
        }

        auto difficultiesItr = value.FindMember(u"difficulties");

        if (!difficultiesItr->value.IsNull())
        {
            for (auto& diff : value[u"difficulties"].GetArray())
            {
                difficulties.emplace_back(diff.GetObject());
            }
        }
    }

    LeaderboardInfo::LeaderboardInfo(Il2CppString* string) : LeaderboardInfo(getdoc(csstrtostr(string))){};

    LeaderboardInfo::LeaderboardInfo(std::string_view string) : LeaderboardInfo(getdoc(string)){};

    LeaderboardInfo::LeaderboardInfo(std::u16string_view string) : LeaderboardInfo(getdoc(string)){};

    LeaderboardInfo::LeaderboardInfo(){};
}