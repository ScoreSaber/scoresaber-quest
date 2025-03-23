#include "Utils/TeamUtils.hpp"

#include <map>

#include "Utils/WebUtils.hpp"
#include "logging.hpp"

#define GetStringValue(in, out)                                      \
    auto itr##out = value.FindMember(in);                            \
    if (itr##out != value.MemberEnd() && itr##out->value.IsString()) \
    {                                                                \
        out = itr##out->value.GetString();                           \
    }                                                                \
    else                                                             \
    {                                                                \
        out = "";                                                    \
    }

namespace TeamUtils
{
    std::map<std::string, std::vector<TeamMember>> members;

    void Download()
    {
        std::string url = "https://raw.githubusercontent.com/ScoreSaber/ScoreSaber-Team/main/team.json";
        WebUtils::GetAsync(url, [](long code, std::string result) {
            if (code == 200)
            {
                ParseMembers(result);
            }
        }, true);
    }

    void ParseMembers(std::string_view doc)
    {
        rapidjson::Document d;
        d.Parse(doc.data());

        auto teamMembersitr = d.FindMember("TeamMembers");
        if (teamMembersitr != d.MemberEnd())
        {
            auto& area = teamMembersitr->value;
            for (auto mem = area.MemberBegin(); mem != area.MemberEnd(); mem++)
            {
                std::string areaString = mem->name.GetString();
                for (auto& c : areaString)
                {
                    c = tolower(c);
                }
                members[areaString] = std::vector<TeamMember>();
                auto theVectorItr = members.find(areaString);

                auto array = mem->value.GetArray();
                for (auto teamMember = array.Begin(); teamMember != array.End(); teamMember++)
                {
                    theVectorItr->second.emplace_back(*teamMember);
                }
            }
        }
    }

    std::vector<TeamMember>& get_members(std::string_view type)
    {
        std::string areaString = std::string(type);
        for (auto& c : areaString)
        {
            c = tolower(c);
        }

        auto theVectorItr = members.find(areaString);
        return theVectorItr->second;
    }

    TeamMember::TeamMember(const rapidjson::Value& value)
    {
        GetStringValue("Name", name);
        GetStringValue("ProfilePicture", profilePicture);
        GetStringValue("Discord", discord);
        GetStringValue("GitHub", github);
        GetStringValue("Twitch", twitch);
        GetStringValue("Twitter", twitter);
        GetStringValue("YouTube", youtube);
    }

    TeamMember::TeamMember(const TeamMember& other)
    {
        name = other.name;
        profilePicture = other.profilePicture;
        discord = other.discord;
        github = other.github;
        twitch = other.twitch;
        twitter = other.twitter;
        youtube = other.youtube;
    }

    std::string TeamMember::get_name()
    {
        return name;
    }

    std::string TeamMember::get_profilePicture()
    {
        return fmt::format("https://raw.githubusercontent.com/ScoreSaber/ScoreSaber-Team/main/images/{:s}", profilePicture);
    }

    std::string TeamMember::get_discord()
    {
        return discord != "" ? fmt::format("https://discordapp.com/users/{:s}", discord) : "";
    }

    std::string TeamMember::get_github()
    {
        return github != "" ? fmt::format("https://github.com/{:s}", github) : "";
    }

    std::string TeamMember::get_twitch()
    {
        return twitch != "" ? fmt::format("https://twitch.tv/{:s}", twitch) : "";
    }

    std::string TeamMember::get_twitter()
    {
        return twitter != "" ? fmt::format("https://twitter.com/{:s}", twitter) : "";
    }

    std::string TeamMember::get_youtube()
    {
        return youtube != "" ? fmt::format("https://youtube.com/channel/{:s}", youtube) : "";
    }
} // namespace TeamUtils