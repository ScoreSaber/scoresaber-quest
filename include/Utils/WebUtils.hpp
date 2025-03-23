#pragma once
#include <beatsaber-hook/shared/config/rapidjson-utils.hpp>
#include <custom-types/shared/coroutine.hpp>

#include <HMUI/ImageView.hpp>

#include <functional>
#include <string>
#include <tuple>

namespace WebUtils
{
    extern std::string cookie;
    std::tuple<long, std::string> GetSync(std::string url, long timeout);
    void GetAsync(std::string url, std::function<void(long, std::string)> finished, bool pureCppCallback = false);
    void GetAsync(std::string url, long timeout, std::function<void(long, std::string)> finished, bool pureCppCallback = false);

    std::tuple<long, std::string> PostSync(std::string url, std::string postData, long timeout);
    /// @brief posts to url and passes them into the finished callback along with the http response code
    /// @param url the url to query
    /// @param postData the postData to send
    /// @param timeout the timeout for the query
    /// @param finished the callback for when we're done downloading
    /// @param pureCppCallback whether we need to use a C# thread or can use C++ threads
    void PostAsync(std::string url, std::string postData, long timeout, std::function<void(long, std::string)> finished, bool pureCppCallback = false);

    std::tuple<long, std::string> PostWithReplaySync(std::string url, const std::vector<char> &replayData, std::string postData, long timeout);
    void PostWithReplayAsync(std::string url, const std::vector<char> replayData, std::string postData, long timeout, std::function<void(long, std::string)> finished);

    /// @brief gets texture @ url and applies it to out->set_sprite() after downloading
    custom_types::Helpers::Coroutine WaitForImageDownload(std::string url, HMUI::ImageView* out);
    /// @brief gets gif @ url and applies it's first frame to out->set_sprite() after downloading
    custom_types::Helpers::Coroutine WaitForGifDownload(std::string url, HMUI::ImageView* out);

    // void DownloadFileAsync(std::string url, std::string filePath, long timeOut, const std::function<void(long)>& finished);
    long DownloadReplaySync(std::string url, std::vector<char> &replayData, long timeOut);

} // namespace WebUtils
