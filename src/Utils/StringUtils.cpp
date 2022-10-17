#include "Utils/StringUtils.hpp"

#include <chrono>
#include <codecvt>
#include <sstream>
#include <string>
#include <vector>

#include "System/IO/Path.hpp"
#include "System/String.hpp"
#include "logging.hpp"

static std::string color_prefix = "<color=";
static std::string color_suffix = "</color>";
static std::string size_prefix = "<size=";
static std::string size_suffix = "</size>";

static std::u16string color_prefix_u16 = u"<color=";
static std::u16string color_suffix_u16 = u"</color>";
static std::u16string size_prefix_u16 = u"<size=";
static std::u16string size_suffix_u16 = u"</size>";

using namespace std;

namespace StringUtils
{

    std::string GetFormattedHash(StringW levelId)
    {
        std::string thing = levelId;
        if (!Contains(thing, "custom_level_"))
        {
            return "ost_" + thing;
        }
        else
        {
            levelId = levelId->Replace("custom_level_", Il2CppString::_get_Empty());
            return levelId;
        }
    }

    std::string GetEnv(const std::string& var)
    {
        const char* val = std::getenv(var.c_str());
        if (val == nullptr)
        { // invalid to assign nullptr to std::string
            return "";
        }
        else
        {
            return val;
        }
    }

    std::string to_utf8(std::u16string_view view)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}
            .to_bytes(view.data());
    }

    std::string ReplaceInvalidChars(std::string fileName)
    {
        auto fileNameCstr = StringW(fileName);
        auto splitChars = fileNameCstr->Split(System::IO::Path::GetInvalidFileNameChars());
        auto result = System::String::Join("_", splitChars);
        return result;
    }

    std::string GetRoleColor(std::string role)
    {
        if (role.compare("Supporter") == 0)
        {
            return "#f76754";
        }
        if (role.compare("Ranking Team") == 0)
        {
            return "#1cbc9c";
        }
        return "#FFFFFF";
    }

    std::u16string FormatScore(double s)
    {
        return to_utf16(string_format(" - (<color=#ffd42a>%.2f%%</color>)", s));
    }

    std::string FormatScore(std::string s)
    {
        for (int i = 0; i < 2; i++)
        {
            s.pop_back();
        }
        s = s.substr(2);
        s.insert(2, ".");
        s = s + "%";
        return " - (" + Colorize(s, "#ffd42a") + ") ";
    }

    std::string RemoveTrailingZeros(std::string s, int zeros)
    {
        for (int i = 0; i < zeros; i++)
        {
            s.pop_back();
        }
        return s;
    }

    std::string ColorizePosNeg(std::string s)
    {
        float f = stof(s);
        if (f > 0)
        {
            return Colorize(s, "green");
        }
        else if (f < 0)
        {
            return Colorize(s, "red");
        }
        else
        {
            return s;
        }
    }

    std::u16string FormatPP(const ::ScoreSaber::Data::Score&& score)
    {
        const std::string& modifiers = score.modifiers;
        double pp = score.pp;
        std::string ppString = std::to_string(pp);
        for (int i = 0; i < 4; i++)
            ppString.pop_back();
        std::u16string s = to_utf16(ppString) + Resize(u"pp", 50);
        s = Colorize(u" - (", "\"white\"") + s + Colorize(u")", "\"white\"");
        if (pp > 0.0f)
        {
            s = Colorize(s, "#6872e5");
            if (modifiers.compare("") == 0)
            {
                return s;
            }
            else
            {
                return s + u" - " + Colorize(u"[" + to_utf16(modifiers) + u"]", "#464f55");
            }
        }
        else
        {
            if (modifiers.compare("") == 0)
            {
                return u"";
            }
            else
            {
                return u" - " + Colorize(u"[" + to_utf16(modifiers) + u"]", "#464f55");
            }
        }
    }

    std::u16string FormatPP(const ::ScoreSaber::Data::Score& score)
    {
        const std::string& modifiers = score.modifiers;
        double pp = score.pp;
        std::string ppString = std::to_string(pp);
        for (int i = 0; i < 4; i++)
            ppString.pop_back();
        std::u16string s = to_utf16(ppString) + Resize(u"pp", 50);
        s = Colorize(u" - (", "\"white\"") + s + Colorize(u")", "\"white\"");
        if (pp > 0.0f)
        {
            s = Colorize(s, "#6872e5");
            if (modifiers.compare("") == 0)
            {
                return s;
            }
            else
            {
                return s + u" - " + Colorize(u"[" + to_utf16(modifiers) + u"]", "#464f55");
            }
        }
        else
        {
            if (modifiers.compare("") == 0)
            {
                return u"";
            }
            else
            {
                return u" - " + Colorize(u"[" + to_utf16(modifiers) + u"]", "#464f55");
            }
        }
    }

    std::string FormatPP(std::string s,
                         rapidjson::GenericObject<true, rapidjson::Value> score)
    {
        std::string modifiers = std::string(score["modifiers"].GetString());
        double pp = score["pp"].GetDouble();
        for (int i = 0; i < 4; i++)
        {
            s.pop_back();
        }
        s = s + Resize("pp", 50);
        s = Colorize(" - (", "\"white\"") + s + Colorize(")", "\"white\"");
        if (pp > 0.0f)
        {
            s = Colorize(s, "#6872e5");
            if (modifiers.compare("") == 0)
            {
                return s;
            }
            else
            {
                return s + string(" - ") + Colorize("[" + modifiers + "]", "#464f55");
            }
        }
        else
        {
            if (modifiers.compare("") == 0)
            {
                return "";
            }
            else
            {
                return " - " + Colorize("[" + modifiers + "]", "#464f55");
            }
        }
    }

    std::string Colorize(std::string s, std::string color)
    {
        return color_prefix + color + string(">") + s + color_suffix;
    }

    std::u16string Colorize(std::u16string s, std::string color)
    {
        return color_prefix_u16 + to_utf16(color) + u">" + s + color_suffix_u16;
    }

    std::string Resize(std::string s, int sizePercent)
    {
        return size_prefix + to_string(sizePercent) + string("%>") + s + size_suffix;
    }

    std::u16string Resize(std::u16string s, int sizePercent)
    {
        return size_prefix_u16 + to_utf16(to_string(sizePercent)) + u"\%>" + s + size_suffix_u16;
    }

    std::string Truncate(std::string str, size_t width, bool show_ellipsis = true)
    {
        if (str.length() > width)
        {
            if (show_ellipsis)
            {
                return str.substr(0, width) + "...";
            }
            else
            {
                return str.substr(0, width);
            }
        }
        return str;
    }

    bool Contains(std::string haystack, std::string needle)
    {
        if (haystack.find(needle) != std::string::npos)
        {
            return true;
        }
        return false;
    }

    std::vector<std::string> split(const std::string& s, char delim)
    {
        std::stringstream ss(s);
        std::string item;
        std::vector<std::string> elems;
        while (std::getline(ss, item, delim))
        {
            elems.push_back(item);
        }
        return elems;
    }

    bool replace(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

} // namespace StringUtils
