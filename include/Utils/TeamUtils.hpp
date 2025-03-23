#pragma once
#include <beatsaber-hook/shared/config/rapidjson-utils.hpp>

#include <UnityEngine/Sprite.hpp>
#include <string>

namespace TeamUtils
{
    class TeamMember
    {
    public:
        TeamMember(const rapidjson::Value& value);
        TeamMember(const TeamMember& other);

        std::string get_profilePicture();
        std::string get_name();
        std::string get_discord();
        std::string get_github();
        std::string get_twitch();
        std::string get_twitter();
        std::string get_youtube();

    private:
        std::string name;
        std::string profilePicture;
        std::string discord;
        std::string github;
        std::string twitch;
        std::string twitter;
        std::string youtube;
    };

    void ParseMembers(std::string_view doc);
    void Download();
    std::vector<TeamMember>& get_members(std::string_view type);
} // namespace TeamUtils