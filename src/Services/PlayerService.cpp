#include "Services/PlayerService.hpp"
#include "Utils/WebUtils.hpp"

#include "Data/Private/AuthResponse.hpp"
#include <System/IO/Directory.hpp>
#include <System/Action.hpp>
#include "UI/Other/ScoreSaberLeaderboardView.hpp"
#include "Utils/StringUtils.hpp"
#include <beatsaber-hook/shared/config/rapidjson-utils.hpp>
#include <custom-types/shared/delegate.hpp>
#include "logging.hpp"
#include <bsml/shared/BSML/MainThreadScheduler.hpp>
#include "static.hpp"
#include <chrono>

using namespace StringUtils;
using namespace BSML;

namespace ScoreSaber::Services::PlayerService
{
    playerInfo_t playerInfo;

    std::mutex authLock;
    
    enum AuthState
    {
        NotStarted,
        Started,
        Finished
    } authState;
    std::vector<std::function<void(LoginStatus)>> finishedCallbacks;

    void OnSoftRestart() {
        playerInfo = playerInfo_t();
        authState = NotStarted;
        finishedCallbacks.clear();
    }

    void AuthenticateUser(std::function<void(LoginStatus)> finished)
    {
        {
            std::lock_guard lock(authLock);
            if (authState == Finished) // we are done already, nothing to do
                return;
            finishedCallbacks.push_back(finished);
            if (authState == Started) // store the callback, but don't do anything else
                return;
            authState = Started;
        }
        
        std::string steamKey = "";
        std::string playerId = "";
        std::string friends = "";

        if (fileexists(ScoreSaber::Static::STEAM_KEY_PATH))
        {
            std::string rawSteamThing = readfile(ScoreSaber::Static::STEAM_KEY_PATH);
            std::vector<std::string> splitRawSteamThing = split(rawSteamThing, ':');

            steamKey = splitRawSteamThing[0];
            playerId = splitRawSteamThing[1];
        }

        if (steamKey == "" || playerId == "")
        {
            ERROR("Failed to read player authentication data");
            playerInfo.loginStatus = LoginStatus::Error;
            {
                std::lock_guard lock(authLock);
                authState = Finished;
                for(auto fin : finishedCallbacks)
                    fin(LoginStatus::Error);
                finishedCallbacks.clear();
            }
            return;
        }

        if (fileexists(ScoreSaber::Static::FRIENDS_PATH))
        {
            friends = readfile(ScoreSaber::Static::FRIENDS_PATH);
        }
        else
        {
            writefile(ScoreSaber::Static::FRIENDS_PATH, "76561198283584459," + playerId);
            friends = readfile(ScoreSaber::Static::FRIENDS_PATH);
        }

        std::string postData = fmt::format("at=2&playerId={:s}&nonce={:s}&friends={:s}&name=",
                                             playerId.c_str(), steamKey.c_str(), friends.c_str());

        std::string authUrl = ScoreSaber::Static::BASE_URL + "/api/game/auth";

        WebUtils::PostAsync(authUrl, postData, 6000, [=](long code, std::string result) {
            if (code == 200)
            {
                rapidjson::Document jsonDocument;
                jsonDocument.Parse(result.data());
                ScoreSaber::Data::Private::AuthResponse authResponse(jsonDocument.GetObject());
                playerInfo.playerKey = authResponse.a;
                playerInfo.serverKey = authResponse.e;

                playerInfo.localPlayerData = Data::Player(playerId);

                playerInfo.loginStatus = LoginStatus::Success;
                {
                    std::lock_guard lock(authLock);
                    authState = Finished;
                    for(auto fin : finishedCallbacks)
                        fin(LoginStatus::Success);
                    finishedCallbacks.clear();
                }
                UpdatePlayerInfoThread();
            }
            else
            {
                // ERROR("Authentication error");
                playerInfo.loginStatus = LoginStatus::Error;
                {
                    std::lock_guard lock(authLock);
                    authState = Finished;
                    for(auto fin : finishedCallbacks)
                        fin(LoginStatus::Error);
                    finishedCallbacks.clear();
                }
            }
        });
    }

    void GetPlayerInfo(std::string playerId, bool full, std::function<void(std::optional<Data::Player>)> finished)
    {

        std::string url = fmt::format("{:s}/api/player/{:s}", ScoreSaber::Static::BASE_URL.c_str(), playerId.c_str());

        if (full)
        {
            url = fmt::format("{:s}/full", url.c_str());
        }
        else
        {
            url = fmt::format("{:s}/basic", url.c_str());
        }

        WebUtils::GetAsync(url, [&, finished](long code, std::string result) {
            if (code == 200)
            {
                try
                {
                    rapidjson::Document jsonDocument;
                    jsonDocument.Parse(result.data());
                    Data::Player playerResponse(jsonDocument.GetObject());
                    finished(std::make_optional<Data::Player>(playerResponse));
                }
                catch (const std::exception& e)
                {
                    finished(std::nullopt);
                }
            }
            else
            {
                finished(std::nullopt);
            }
        });
    }

    void UpdatePlayerInfoThread()
    {
        il2cpp_utils::il2cpp_aware_thread([] {
            while (true)
            {
                if(!ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner)
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                else
                {
                    UpdatePlayerInfo(false);
                    std::this_thread::sleep_for(std::chrono::seconds(300));
                }
            }
        }).detach();
    }

    void UpdatePlayerInfo(bool fromMainThread)
    {
        if (!fromMainThread)
        {
            MainThreadScheduler::Schedule([=]() {
                    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->set_loading(true);
            });
        }
        else
        {
            ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->set_loading(true);
        }

        GetPlayerInfo(playerInfo.localPlayerData.id, true, [=](std::optional<Data::Player> playerData) {
            if (playerData.has_value())
            {
                playerInfo.localPlayerData = playerData.value();
                MainThreadScheduler::Schedule([=]() {
                    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->set_ranking(playerInfo.localPlayerData.rank, playerInfo.localPlayerData.pp);
                });
            }
        });
    }

} // namespace ScoreSaber::Services::PlayerService
