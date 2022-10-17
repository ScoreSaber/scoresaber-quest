#include "Data/Leaderboard.hpp"
#include "beatsaber-hook/shared/utils/utils-functions.h"
#include "logging.hpp"

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
    Leaderboard::Leaderboard(const rapidjson::Document&& doc)
    {
        for (auto& score : doc["scores"].GetArray())
        {
            scores.emplace_back(score.GetObject());
        }
        auto leaderboardInfoItr = doc.FindMember("leaderboardInfo");
        if (leaderboardInfoItr->value.IsObject())
        {
            leaderboardInfo = LeaderboardInfo(leaderboardInfoItr->value.GetObject());
        }
    }

    Leaderboard::Leaderboard(const rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>&& doc)
    {
        for (auto& score : doc[u"scores"].GetArray())
        {
            scores.emplace_back(score.GetObject());
        }
        auto leaderboardInfoItr = doc.FindMember(u"leaderboardInfo");
        if (leaderboardInfoItr->value.IsObject())
        {
            leaderboardInfo = LeaderboardInfo(leaderboardInfoItr->value.GetObject());
        }
    }

    Leaderboard::Leaderboard(rapidjson::GenericObject<true, rapidjson::Value> doc)
    {
        for (auto& score : doc["scores"].GetArray())
        {
            scores.emplace_back(score.GetObject());
        }
        auto leaderboardInfoItr = doc.FindMember("leaderboardInfo");
        if (leaderboardInfoItr->value.IsObject())
        {
            leaderboardInfo = LeaderboardInfo(leaderboardInfoItr->value.GetObject());
        }
    }

    Leaderboard::Leaderboard(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc)
    {
        for (auto& score : doc[u"scores"].GetArray())
        {
            scores.emplace_back(score.GetObject());
        }
        auto leaderboardInfoItr = doc.FindMember(u"leaderboardInfo");
        if (leaderboardInfoItr->value.IsObject())
        {
            leaderboardInfo = LeaderboardInfo(leaderboardInfoItr->value.GetObject());
        }
    }

    Leaderboard::Leaderboard(rapidjson::GenericObject<false, rapidjson::Value> doc)
    {
        // eee ppp
        for (auto& score : doc["scores"].GetArray())
        {
            scores.emplace_back(score.GetObject());
        }

        auto leaderboardInfoItr = doc.FindMember("leaderboardInfo");
        if (leaderboardInfoItr->value.IsObject())
        {
            leaderboardInfo = LeaderboardInfo(leaderboardInfoItr->value.GetObject());
        }
    }

    Leaderboard::Leaderboard(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> doc)
    {
        for (auto& score : doc[u"scores"].GetArray())
        {
            scores.emplace_back(score.GetObject());
        }
        auto leaderboardInfoItr = doc.FindMember(u"leaderboardInfo");
        if (leaderboardInfoItr->value.IsObject())
        {
            leaderboardInfo = LeaderboardInfo(leaderboardInfoItr->value.GetObject());
        }
    }

    Leaderboard::Leaderboard(StringW string)
        : Leaderboard(getdoc(csstrtostr(string))){};

    Leaderboard::Leaderboard(std::string_view string)
        : Leaderboard(getdoc(string)){};

    Leaderboard::Leaderboard(std::u16string_view string)
        : Leaderboard(getdoc(string)){};
    Leaderboard::Leaderboard(){};
} // namespace ScoreSaber::Data