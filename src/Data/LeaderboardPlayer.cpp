#include "Data/LeaderboardPlayer.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

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
            name = std::make_optional(to_utf16(std::string(nameItr->value.GetString())));
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
            id = std::make_optional(std::string(to_utf8(idItr->value.GetString())));
        }

        auto nameItr = value.FindMember(u"name");
        if (nameItr != value.MemberEnd())
        {
            name = std::make_optional(to_utf16(std::string(to_utf8(nameItr->value.GetString()))));
        }

        profilePicture = to_utf8(value[u"profilePicture"].GetString());

        auto countryItr = value.FindMember(u"country");
        if (!countryItr->value.IsNull())
        {
            country = std::make_optional(std::string(to_utf8(countryItr->value.GetString())));
        }

        auto permissionsItr = value.FindMember(u"permissions");
        if (!permissionsItr->value.IsNull())
        {
            permissions = std::make_optional(int(permissionsItr->value.GetInt()));
        }

        auto roleItr = value.FindMember(u"role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = std::make_optional(std::string(to_utf8(roleItr->value.GetString())));
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
            name = std::make_optional(to_utf16(std::string(nameItr->value.GetString())));
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
            id = std::make_optional(std::string(to_utf8(idItr->value.GetString())));
        }

        auto nameItr = value.FindMember(u"name");
        if (nameItr != value.MemberEnd())
        {
            name = std::make_optional(to_utf16(std::string(to_utf8(nameItr->value.GetString()))));
        }

        profilePicture = to_utf8(value[u"profilePicture"].GetString());

        auto countryItr = value.FindMember(u"country");
        if (!countryItr->value.IsNull())
        {
            country = std::make_optional(std::string(to_utf8(countryItr->value.GetString())));
        }

        auto permissionsItr = value.FindMember(u"permissions");
        if (!permissionsItr->value.IsNull())
        {
            permissions = std::make_optional(int(permissionsItr->value.GetInt()));
        }

        auto roleItr = value.FindMember(u"role");
        if (roleItr != value.MemberEnd() && roleItr->value.IsString())
            role = std::make_optional(std::string(to_utf8(roleItr->value.GetString())));
    }
}