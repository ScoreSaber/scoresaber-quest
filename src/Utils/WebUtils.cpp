#include "Utils/WebUtils.hpp"

#include <iomanip>
#include <sstream>

#include "UnityEngine/Networking/DownloadHandler.hpp"
#include "UnityEngine/Networking/DownloadHandlerTexture.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestTexture.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "Utils/StringUtils.hpp"
#include "gif-lib/shared/gif_lib.h"
#include "libcurl/shared/curl.h"
#include "libcurl/shared/easy.h"
#include "logging.hpp"
#include "main.hpp"
#include <tuple>

//#include "gif_read.h"

using namespace UnityEngine;
using namespace StringUtils;

// from
// https://github.com/darknight1050/SongDownloader/blob/master/src/Utils/WebUtils.cpp

#define TIMEOUT 10
#define USER_AGENT                               \
    (std::string("ScoreSaber-Quest/") + VERSION) \
        .c_str()

#include "gif-lib/shared/gif_lib.h"
struct Gif
{
    Gif(std::string& text)
        : data(text), datastream(&this->data){};
    Gif(std::vector<uint8_t>& vec)
        : data(reinterpret_cast<std::vector<char>&>(vec)), datastream(&this->data){};
    Gif(std::vector<char>& vec)
        : data(vec), datastream(&this->data){};
    Gif(Array<char>* array)
        : data(array), datastream(&this->data){};
    Gif(Array<uint8_t>* array)
        : Gif(reinterpret_cast<Array<char>*>(array)){};
    Gif(ArrayW<uint8_t> array)
        : Gif(static_cast<Array<uint8_t>*>(array)){};
    ~Gif()
    {
        int error = 0;
        DGifCloseFile(gif, &error);
    }
    int Parse()
    {
        int error = 0;
        gif = DGifOpen(this, &Gif::read, &error);
        return error;
    }

    int Slurp()
    {
        return DGifSlurp(gif);
    }

    static int read(GifFileType* pGifHandle, GifByteType* dest, int toRead)
    {
        Gif& dataWrapper = *(Gif*)pGifHandle->UserData;
        return dataWrapper.datastream.readsome(reinterpret_cast<char*>(dest), toRead);
    }

    Texture2D* get_frame(int idx)
    {
        if (!gif || idx > get_length())
            return nullptr;

        GifColorType* color;
        SavedImage* frame;
        ExtensionBlock* ext = 0;
        GifImageDesc* frameInfo;
        ColorMapObject* colorMap;
        int x, y, j, loc;

        frame = &(gif->SavedImages[idx]);

        frameInfo = &(frame->ImageDesc);

        if (frameInfo->ColorMap)
        {
            colorMap = frameInfo->ColorMap;
        }
        else
        {
            colorMap = gif->SColorMap;
        }

        for (j = 0; j < frame->ExtensionBlockCount; ++j)
        {
            if (frame->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE)
            {
                ext = &(frame->ExtensionBlocks[j]);
                break;
            }
        }

        // entire texture size;
        int width = get_width();
        int height = get_height();
        auto texture = Texture2D::New_ctor(width, height);
        // entire texture
        auto pixelData = texture->GetPixels32();
        uint8_t* px = reinterpret_cast<uint8_t*>(pixelData->values);
        // top -> top + height

        // left -> left + width

        // if directly setting in the color array, the loc is y inverted
        // A frame only describes part of a picture
        long pixelDataOffset = frameInfo->Top * width + frameInfo->Left;
        for (y = 0; y < frameInfo->Height; ++y)
        {
            for (x = 0; x < frameInfo->Width; ++x)
            {
                loc = y * frameInfo->Width + x;
                if (frame->RasterBits[loc] == ext->Bytes[3] && ext->Bytes[0])
                {
                    continue;
                }

                color = &colorMap->Colors[frame->RasterBits[loc]];
                long locWithinFrame = (frameInfo->Height - y - 1) * frameInfo->Width + x + pixelDataOffset;
                pixelData->values[locWithinFrame] = Color32(color->Red, color->Green, color->Blue, 0xff);
            }
        }
        texture->SetAllPixels32(pixelData, 0);
        texture->Apply();
        return texture;
    }
    int get_width()
    {
        return gif ? gif->SWidth : 0;
    };
    int get_height()
    {
        return gif ? gif->SHeight : 0;
    };
    int get_length()
    {
        return gif ? gif->ImageCount : 0;
    };

  public:
    GifFileType* gif = nullptr;

  private:
    template <typename CharT, typename TraitsT = std::char_traits<CharT>>
    class vectorwrapbuf : public std::basic_streambuf<CharT, TraitsT>
    {
      public:
        vectorwrapbuf(std::string& text)
        {
            this->std::basic_streambuf<CharT, TraitsT>::setg(text.data(), text.data(), text.data() + text.size());
        }

        vectorwrapbuf(std::vector<CharT>& vec)
        {
            this->std::basic_streambuf<CharT, TraitsT>::setg(vec.data(), vec.data(), vec.data() + vec.size());
        }

        vectorwrapbuf(Array<CharT>*& arr)
        {
            this->std::basic_streambuf<CharT, TraitsT>::setg(arr->values, arr->values, arr->values + arr->Length());
        }
    };

    std::istream datastream;
    vectorwrapbuf<char> data;
};

namespace WebUtils
{
    // https://stackoverflow.com/a/55660581

    std::string cookie;

    std::string query_encode(const std::string& s)
    {
        std::string ret;
#define IS_BETWEEN(ch, low, high) (ch >= low && ch <= high)
#define IS_ALPHA(ch) (IS_BETWEEN(ch, 'A', 'Z') || IS_BETWEEN(ch, 'a', 'z'))
#define IS_DIGIT(ch) IS_BETWEEN(ch, '0', '9')
#define IS_HEXDIG(ch) \
    (IS_DIGIT(ch) || IS_BETWEEN(ch, 'A', 'F') || IS_BETWEEN(ch, 'a', 'f'))

        for (size_t i = 0; i < s.size();)
        {
            char ch = s[i++];

            if (IS_ALPHA(ch) || IS_DIGIT(ch))
            {
                ret += ch;
            }
            else if ((ch == '%') && IS_HEXDIG(s[i + 0]) && IS_HEXDIG(s[i + 1]))
            {
                ret += s.substr(i - 1, 3);
                i += 2;
            }
            else
            {
                switch (ch)
                {
                    case '-':
                    case '.':
                    case '_':
                    case '~':
                    case '!':
                    case '$':
                    case '&':
                    case '\'':
                    case '(':
                    case ')':
                    case '*':
                    case '+':
                    case ',':
                    case ';':
                    case '=':
                    case ':':
                    case '@':
                    case '/':
                    case '?':
                    case '[':
                    case ']':
                        ret += ch;
                        break;

                    default: {
                        static const char hex[] = "0123456789ABCDEF";
                        char pct[] = "%  ";
                        pct[1] = hex[(ch >> 4) & 0xF];
                        pct[2] = hex[ch & 0xF];
                        ret.append(pct, 3);
                        break;
                    }
                }
            }
        }

        return ret;
    }

    std::size_t CurlWrite_CallbackFunc_StdString(void* contents, std::size_t size,
                                                 std::size_t nmemb,
                                                 std::string* s)
    {
        std::size_t newLength = size * nmemb;
        try
        {
            s->append((char*)contents, newLength);
        }
        catch (std::bad_alloc& e)
        {
            // handle memory problem
            getLogger().critical("Failed to allocate string of size: %lu", newLength);
            return 0;
        }
        return newLength;
    }

    size_t hdf(char* b, size_t size, size_t nitems, void* userdata)
    {
        size_t numbytes = size * nitems;
        std::string header(b, numbytes);

        if (header.starts_with("Set-Cookie"))
        {
            replace(header, "Set-Cookie: ", "");
            header = split(header, ';')[0];
            cookie = header;
        }
        return numbytes;
    }

    std::tuple<long, std::string> GetSync(std::string url, long timeout)
    {
        std::string val;
        // Init curl
        auto* curl = curl_easy_init();
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: */*");

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

        if (!cookie.empty())
        {
            curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());
        }
        // Set headers
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_URL, query_encode(url).c_str());

        // Don't wait forever, time out after TIMEOUT seconds.
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

        // Follow HTTP redirects if necessary.
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         CurlWrite_CallbackFunc_StdString);

        long httpCode(0);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &val);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        auto res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
        {
            getLogger().critical("curl_easy_perform() failed: %u: %s", res,
                                 curl_easy_strerror(res));
        }
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

        return std::make_tuple(httpCode, val);
    }

    void GetAsync(std::string url, std::function<void(long, std::string)> finished)
    {
        GetAsync(url, TIMEOUT, finished);
    }

    void GetAsync(std::string url, long timeout, std::function<void(long, std::string)> finished)
    {
        std::thread t([url, timeout, finished] {
            auto [responseCode, response] = GetSync(url, timeout);
            finished(responseCode, response);
        });
        t.detach();
    }

    std::tuple<long, std::string> PostSync(std::string url, std::string postData, long timeout)
    {
        std::string val;
        // Init curl
        auto* curl = curl_easy_init();
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: */*");
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        // headers = curl_slist_append(headers, "Authorization: 87f0b8e55aad995e96288e0cab45dc73");

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

        if (!cookie.empty())
        {
            curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());
        }

        // Set headers
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, query_encode(url).c_str());

        // Don't wait forever, time out after TIMEOUT seconds.
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

        // Follow HTTP redirects if necessary.
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);

        long httpCode(0);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &val);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, hdf);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        auto res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            getLogger().critical("curl_easy_perform() failed: %u: %s", res, curl_easy_strerror(res));
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

        return std::make_tuple(httpCode, val);
    }

    void PostAsync(std::string url, std::string postData, long timeout, std::function<void(long, std::string)> finished)
    {
        std::thread t(
            [url, postData, timeout, finished] {
                auto [responseCode, response] = PostSync(url, postData, timeout);
                finished(responseCode, response);
            });
        t.detach();
    }

    std::tuple<long, std::string> PostWithFileSync(std::string url, std::string filePath, std::string postData, long timeout)
    {
        std::string val;
        // Init curl
        auto* curl = curl_easy_init();

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: */*");
        headers = curl_slist_append(headers, "Content-Type: multipart/form-data");

        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

        if (!cookie.empty())
        {
            curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());
        }

        // Set headers
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, query_encode(url).c_str());

        // Don't wait forever, time out after TIMEOUT seconds.
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

        // Follow HTTP redirects if necessary.
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_POST, 1);

        curl_mime* mime;
        curl_mimepart* part;
        mime = curl_mime_init(curl);
        part = curl_mime_addpart(mime);

        curl_mime_name(part, "data");
        curl_mime_data(part, postData.c_str(), CURL_ZERO_TERMINATED);
        part = curl_mime_addpart(mime);

        curl_mime_name(part, "zr");
        curl_mime_filedata(part, filePath.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);

        long httpCode(0);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &val);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, hdf);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        auto res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            getLogger().critical("curl_easy_perform() failed: %u: %s", res, curl_easy_strerror(res));
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);
        curl_mime_free(mime);

        return std::make_tuple(httpCode, val);
    }

    void PostWithFileAsync(std::string url, std::string filePath, std::string postData, long timeout, std::function<void(long, std::string)> finished)
    {
        std::thread t(
            [url, filePath, postData, timeout, finished] {
                auto [responseCode, response] = PostWithFileSync(url, filePath, postData, timeout);
                finished(responseCode, response);
            });
        t.detach();
    }

    std::vector<unsigned char> Swap(std::vector<unsigned char> panda1, std::vector<unsigned char> panda2)
    {
        int N1 = 11;
        int N2 = 13;
        int NS = 257;

        for (int i = 0; i <= panda2.size() - 1; i++)
        {
            NS += NS % (panda2[i] + 1);
        }

        std::vector<unsigned char> T(panda1.size());
        for (int i = 0; i <= panda1.size() - 1; i++)
        {
            NS = panda2[i % panda2.size()] + NS;
            N1 = (NS + 5) * (N1 & 255) + (N1 >> 8);
            N2 = (NS + 7) * (N2 & 255) + (N2 >> 8);
            NS = ((N1 << 8) + N2) & 255;

            T[i] = static_cast<unsigned char>(panda1[i] ^ static_cast<unsigned char>(NS));
        }

        return T;
    }

    std::string ConvertToHex(const std::vector<unsigned char>& v)
    {
        std::stringstream buffer;
        for (int i = 0; i < v.size(); i++)
        {
            buffer << std::hex << std::setfill('0');
            buffer << std::setw(2) << static_cast<unsigned>(v[i]);
        }
        return buffer.str();
    }

    custom_types::Helpers::Coroutine WaitForImageDownload(std::string url, HMUI::ImageView* out)
    {
        UnityEngine::Networking::UnityWebRequest* www = UnityEngine::Networking::UnityWebRequestTexture::GetTexture(il2cpp_utils::newcsstr(url));
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(www->SendWebRequest());
        auto downloadHandlerTexture = reinterpret_cast<UnityEngine::Networking::DownloadHandlerTexture*>(www->get_downloadHandler());
        auto texture = downloadHandlerTexture->get_texture();
        auto sprite = Sprite::Create(texture, Rect(0.0f, 0.0f, (float)texture->get_width(), (float)texture->get_height()), Vector2(0.5f, 0.5f), 1024.0f, 1u, SpriteMeshType::FullRect, Vector4(0.0f, 0.0f, 0.0f, 0.0f), false);
        out->set_sprite(sprite);
        co_return;
    }

    custom_types::Helpers::Coroutine WaitForGifDownload(std::string url, HMUI::ImageView* out)
    {
        UnityEngine::Networking::UnityWebRequest* www = UnityEngine::Networking::UnityWebRequest::Get(il2cpp_utils::newcsstr(url));
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(www->SendWebRequest());
        auto downloadHandler = reinterpret_cast<UnityEngine::Networking::DownloadHandler*>(www->get_downloadHandler());
        auto gifDataArr = downloadHandler->GetData();
        Gif gif(gifDataArr);
        int error = gif.Parse();
        co_yield nullptr;
        if (!error && gif.Slurp())
        {
            co_yield nullptr;
            auto texture = gif.get_frame(0);
            auto sprite = Sprite::Create(texture, Rect(0.0f, 0.0f, (float)gif.get_width(), (float)gif.get_height()), Vector2(0.5f, 0.5f), 1024.0f, 1u, SpriteMeshType::FullRect, Vector4(0.0f, 0.0f, 0.0f, 0.0f), false);
            out->set_sprite(sprite);
        }
        else
        {
            INFO("Failed to read gif with error code %d", error);
        }
        co_return;
    }

} // namespace WebUtils
