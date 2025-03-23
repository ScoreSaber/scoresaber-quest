#include "Data/LeaderboardPlayer.hpp"
#include <beatsaber-hook/shared/utils/utils.h>
#include "paper/shared/string_convert.hpp"

namespace ScoreSaber::Data
{
    LeaderboardPlayer::LeaderboardPlayer(const rapidjson::Value&& value)
    {
        auto idItr = value.FindMember("id");
        if (idItr != value.MemberEnd())
        {
            id = std::make_optional(std::string(idItr->value.GetString()));
        }

        auto nameItr = value.FindMember("name");
        if (nameItr != value.MemberEnd())
        {
            name = std::make_optional(Paper::StringConvert::from_utf8(std::string(nameItr->value.GetString())));
        }

        profilePicture = value["profilePicture"].GetString();

        auto countryItr = value.FindMember("country");
        if (countryItr != value.MemberEnd())
        {
            country = std::make_optional(std::string(countryItr->value.GetString()));
        }

        auto permissionsItr = value.FindMember("permissions");
        if (permissionsItr != value.MemberEnd())
        {
            permissions = std::make_optional(int(permissionsItr->value.GetInt()));
        }

        auto roleItr = value.FindMember("role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = std::make_optional(std::string(roleItr->value.GetString()));
    }

    LeaderboardPlayer::LeaderboardPlayer(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value)
    {
        auto idItr = value.FindMember(u"id");

        if (idItr != value.MemberEnd())
        {
            id = std::make_optional(std::string(Paper::StringConvert::from_utf16(idItr->value.GetString())));
        }

        auto nameItr = value.FindMember(u"name");
        if (nameItr != value.MemberEnd())
        {
            name = std::make_optional(Paper::StringConvert::from_utf8(std::string(Paper::StringConvert::from_utf16(nameItr->value.GetString()))));
        }

        profilePicture = Paper::StringConvert::from_utf16(value[u"profilePicture"].GetString());

        auto countryItr = value.FindMember(u"country");
        if (!countryItr->value.IsNull())
        {
            country = std::make_optional(std::string(Paper::StringConvert::from_utf16(countryItr->value.GetString())));
        }

        auto permissionsItr = value.FindMember(u"permissions");
        if (!permissionsItr->value.IsNull())
        {
            permissions = std::make_optional(int(permissionsItr->value.GetInt()));
        }

        auto roleItr = value.FindMember(u"role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = std::make_optional(std::string(Paper::StringConvert::from_utf16(roleItr->value.GetString())));
    }

    LeaderboardPlayer::LeaderboardPlayer(rapidjson::GenericObject<true, rapidjson::Value> value)
    {
        auto idItr = value.FindMember("id");
        if (idItr != value.MemberEnd())
        {
            id = std::make_optional(std::string(idItr->value.GetString()));
        }

        auto nameItr = value.FindMember("name");
        if (nameItr != value.MemberEnd())
        {
            name = std::make_optional(Paper::StringConvert::from_utf8(std::string(nameItr->value.GetString())));
        }

        profilePicture = value["profilePicture"].GetString();

        auto countryItr = value.FindMember("country");
        if (countryItr != value.MemberEnd())
        {
            country = std::make_optional(std::string(countryItr->value.GetString()));
        }

        auto permissionsItr = value.FindMember("permissions");
        if (permissionsItr != value.MemberEnd())
        {
            permissions = std::make_optional(int(permissionsItr->value.GetInt()));
        }

        auto roleItr = value.FindMember("role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = std::make_optional(std::string(roleItr->value.GetString()));
    }

    LeaderboardPlayer::LeaderboardPlayer(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value)
    {
        auto idItr = value.FindMember(u"id");

        if (idItr != value.MemberEnd())
        {
            id = std::make_optional(std::string(Paper::StringConvert::from_utf16(idItr->value.GetString())));
        }

        auto nameItr = value.FindMember(u"name");
        if (nameItr != value.MemberEnd())
        {
            name = std::make_optional(Paper::StringConvert::from_utf8(std::string(Paper::StringConvert::from_utf16(nameItr->value.GetString()))));
        }

        profilePicture = Paper::StringConvert::from_utf16(value[u"profilePicture"].GetString());

        auto countryItr = value.FindMember(u"country");
        if (!countryItr->value.IsNull())
        {
            country = std::make_optional(std::string(Paper::StringConvert::from_utf16(countryItr->value.GetString())));
        }

        auto permissionsItr = value.FindMember(u"permissions");
        if (!permissionsItr->value.IsNull())
        {
            permissions = std::make_optional(int(permissionsItr->value.GetInt()));
        }

        auto roleItr = value.FindMember(u"role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = std::make_optional(std::string(Paper::StringConvert::from_utf16(roleItr->value.GetString())));
    }
}