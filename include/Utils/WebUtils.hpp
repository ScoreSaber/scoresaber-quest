#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "HMUI/ImageView.hpp"

#include <functional>
#include <string>

namespace WebUtils
{

    std::optional<rapidjson::Document> GetJSON(std::string url);

    long Get(std::string url, std::string& val);

    long Get(std::string url, long timeout, std::string& val);

    void GetAsync(std::string url, std::function<void(long, std::string)> finished);

    void GetAsync(std::string url, long timeout, std::function<void(long, std::string)> finished);

    /// @brief posts to url and passes them into the finished callback along with the http response code, calls progressUpdate with progressupdates if given
    /// @param url the url to query
    /// @param postData the postData to send
    /// @param timeout the timeout for the query
    /// @param finished the callback for when we're done downloading
    /// @param progressUpdate callback to give a progressupdate bar or something
    void PostAsync(std::string url, std::string postData, long timeout, std::function<void(long, std::string)> finished);

    std::vector<unsigned char> Swap(std::vector<unsigned char> panda1, std::vector<unsigned char> panda2);
    std::string ConvertToHex(const std::vector<unsigned char>& v);

    /// @brief gets texture @ url and applies it to out->set_sprite() after downloading
    custom_types::Helpers::Coroutine WaitForImageDownload(std::string url, HMUI::ImageView* out);
    /// @brief gets gif @ url and applies it's first frame to out->set_sprite() after downloading
    custom_types::Helpers::Coroutine WaitForGifDownload(std::string url, HMUI::ImageView* out);

} // namespace WebUtils
