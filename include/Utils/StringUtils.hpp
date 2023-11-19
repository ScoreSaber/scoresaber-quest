#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "GlobalNamespace/OVRPlugin_Controller.hpp"
#include "GlobalNamespace/OVRPlugin_SystemHeadset.hpp"
#include "Data/Score.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "custom-types/shared/register.hpp"
#include "custom-types/shared/types.hpp"
#include <string>
#include <vector>

namespace StringUtils
{

    std::string GetFormattedHash(StringW levelId);

    std::string GetEnv(const std::string& var);

    std::string to_utf8(std::u16string_view view);

    std::string ReplaceInvalidChars(std::string fileName);

    std::string GetRoleColor(std::string role);

    std::string FormatScore(std::string percent);
    std::u16string FormatScore(double s);

    std::string FormatPP(std::string pp, rapidjson::GenericObject<true, rapidjson::Value> score);
    std::u16string FormatPP(const ::ScoreSaber::Data::Score&& score);
    std::u16string FormatPP(const ::ScoreSaber::Data::Score& score);

    std::string Colorize(std::string s, std::string color);
    std::u16string Colorize(std::u16string s, std::string color);

    std::string Resize(std::string s, int sizePercent);
    std::u16string Resize(std::u16string s, int sizePercent);

    std::string Truncate(std::string str, size_t width, bool show_ellipsis);
    bool Contains(std::string haystack, std::string needle);

    std::string RemoveTrailingZeros(std::string s, int zeros);

    std::string ColorizePosNeg(std::string s);

    std::vector<std::string> split(const std::string& s, char delim);

    bool replace(std::string& str, const std::string& from, const std::string& to);

    std::string stringify_OVRPlugin_SystemHeadset(const GlobalNamespace::OVRPlugin::SystemHeadset& systemHeadset);
    std::string stringify_OVRPlugin_Controller(const GlobalNamespace::OVRPlugin::Controller& controller);

} // namespace StringUtils