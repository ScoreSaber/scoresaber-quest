#include "Data/Player.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include <sstream>
namespace ScoreSaber::Data
{
    Player::Player(std::string _id)
    {
        id = _id;
        pp = 0.0;
        rank = 0;
        countryRank = 0;
    }

    Player::Player(const rapidjson::Value&& value)
    {
        id = value["id"].GetString();
        name = to_utf16(value["name"].GetString());
        profilePicture = value["profilePicture"].GetString();
        country = value["country"].GetString();
        pp = value["pp"].GetDouble();
        rank = value["rank"].GetInt();
        countryRank = value["countryRank"].GetInt();
        auto roleItr = value.FindMember("role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = std::string(roleItr->value.GetString());
        auto badgesItr = value.FindMember("badges");
        if (!badgesItr->value.IsNull() && badgesItr->value.IsArray())
            for (auto& badge : badgesItr->value.GetArray())
                badges.emplace_back(badge.GetObject());
        std::string parsed, histories = std::string(value["histories"].GetString());
        std::stringstream s(histories);

        while (std::getline(s, parsed, ','))
            this->histories.emplace_back(atoi(parsed.c_str()));

        auto scoreStatsItr = value.FindMember("scoreStats");
        if (!scoreStatsItr->value.IsNull())
        {
            scoreStats = std::make_optional(ScoreStats(scoreStatsItr->value.GetObject()));
        }
        permissions = value["permissions"].GetInt();
        banned = value["banned"].GetBool();
        inactive = value["inactive"].GetBool();
    }

    Player::Player(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value)
    {
        id = to_utf8(value[u"id"].GetString());
        name = std::u16string(value[u"name"].GetString());
        profilePicture = to_utf8(value[u"profilePicture"].GetString());
        country = to_utf8(value[u"country"].GetString());
        pp = value[u"pp"].GetDouble();
        rank = value[u"rank"].GetInt();
        countryRank = value[u"countryRank"].GetInt();
        auto roleItr = value.FindMember(u"role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = to_utf8(roleItr->value.GetString());
        auto badgesItr = value.FindMember(u"badges");
        if (!badgesItr->value.IsNull() && badgesItr->value.IsArray())
            for (auto& badge : badgesItr->value.GetArray())
                badges.emplace_back(badge.GetObject());
        std::string parsed, histories = ::to_utf8(value[u"histories"].GetString());
        std::stringstream s(histories);

        while (std::getline(s, parsed, ','))
            this->histories.emplace_back(atoi(parsed.c_str()));
        auto scoreStatsItr = value.FindMember(u"scoreStats");
        if (!scoreStatsItr->value.IsNull())
        {
            scoreStats = std::make_optional(ScoreStats(scoreStatsItr->value.GetObject()));
        }
        permissions = value[u"permissions"].GetInt();
        banned = value[u"banned"].GetBool();
        inactive = value[u"inactive"].GetBool();
    }

    Player::Player(rapidjson::GenericObject<true, rapidjson::Value> value)
    {
        id = value["id"].GetString();
        name = to_utf16(value["name"].GetString());
        profilePicture = value["profilePicture"].GetString();
        country = value["country"].GetString();
        pp = value["pp"].GetDouble();
        rank = value["rank"].GetInt();
        countryRank = value["countryRank"].GetInt();
        auto roleItr = value.FindMember("role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = std::string(roleItr->value.GetString());
        auto badgesItr = value.FindMember("badges");
        if (!badgesItr->value.IsNull() && badgesItr->value.IsArray())
            for (auto& badge : badgesItr->value.GetArray())
                badges.emplace_back(badge.GetObject());
        std::string parsed, histories = std::string(value["histories"].GetString());
        std::stringstream s(histories);

        while (std::getline(s, parsed, ','))
            this->histories.emplace_back(atoi(parsed.c_str()));

        auto scoreStatsItr = value.FindMember("scoreStats");
        if (!scoreStatsItr->value.IsNull())
        {
            scoreStats = std::make_optional(ScoreStats(scoreStatsItr->value.GetObject()));
        }
        permissions = value["permissions"].GetInt();
        banned = value["banned"].GetBool();
        inactive = value["inactive"].GetBool();
    }

    Player::Player(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value)
    {
        id = to_utf8(value[u"id"].GetString());
        name = std::u16string(value[u"name"].GetString());
        profilePicture = to_utf8(value[u"profilePicture"].GetString());
        country = to_utf8(value[u"country"].GetString());
        pp = value[u"pp"].GetDouble();
        rank = value[u"rank"].GetInt();
        countryRank = value[u"countryRank"].GetInt();
        auto roleItr = value.FindMember(u"role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = to_utf8(roleItr->value.GetString());
        auto badgesItr = value.FindMember(u"badges");
        if (!badgesItr->value.IsNull() && badgesItr->value.IsArray())
            for (auto& badge : badgesItr->value.GetArray())
                badges.emplace_back(badge.GetObject());
        std::string parsed, histories = ::to_utf8(value[u"histories"].GetString());
        std::stringstream s(histories);

        while (std::getline(s, parsed, ','))
            this->histories.emplace_back(atoi(parsed.c_str()));
        auto scoreStatsItr = value.FindMember(u"scoreStats");
        if (scoreStatsItr != value.MemberEnd() && !scoreStatsItr->value.IsNull())
        {
            scoreStats = std::make_optional(ScoreStats(scoreStatsItr->value.GetObject()));
        }
        permissions = value[u"permissions"].GetInt();
        banned = value[u"banned"].GetBool();
        inactive = value[u"inactive"].GetBool();
    }
    Player::Player(){};
} // namespace ScoreSaber::Data