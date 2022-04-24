#include "Services/PlayerService.hpp"
#include "Utils/WebUtils.hpp"

#include "Data/Private/AuthResponse.hpp"
#include "System/IO/Directory.hpp"
#include "UI/Other/ScoreSaberLeaderboardView.hpp"
#include "Utils/StringUtils.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "static.hpp"
#include <chrono>

using namespace StringUtils;

namespace ScoreSaber::Services::PlayerService
{
    playerInfo_t playerInfo;
    void AuthenticateUser(std::function<void(LoginStatus)> finished)
    {

        std::string steamKey = "fb6580ef414bf07";
        std::string playerId = "76561198283584459";

        if (fileexists(ScoreSaber::Static::STEAM_KEY_PATH))
        {
            std::string rawSteamThing = readfile(ScoreSaber::Static::STEAM_KEY_PATH);
            std::vector<std::string> splitRawSteamThing = split(rawSteamThing, ':');

            steamKey = splitRawSteamThing[0];
            playerId = splitRawSteamThing[1];

            // std::string steamKey = readfile(ScoreSaber::Static::STEAM_KEY_PATH);
        }

        // UMBY: Check if steam key is null (for release)
        // UMBY: Obfuscate auth url
        // UMBY: Friends

        std::string postData = "at=2&playerId=" + playerId + "&nonce=" + steamKey + "&friends=3692740027462863,76561198064659288,76561198283584459,76561198278902434,76561198353781972,76561199210789241&name=nah";

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
                finished(LoginStatus::Success);
                UpdatePlayerInfoThread();
            }
            else
            {
                INFO("Authentication error");
                playerInfo.loginStatus = LoginStatus::Error;
                finished(LoginStatus::Error);
            }
        });
    }

    void GetPlayerInfo(std::string playerId, bool full, std::function<void(std::optional<Data::Player>)> finished)
    {

        std::string url = string_format("%s/api/player/%s", ScoreSaber::Static::BASE_URL.c_str(), playerId.c_str());

        // std::string url = string_format("https://scoresaber.com/api/player/%s", playerId.c_str());

        if (full)
        {
            url = string_format("%s/full", url.c_str());
        }
        else
        {
            url = string_format("%s/basic", url.c_str());
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
        std::thread t([] {
            while (true)
            {
                UpdatePlayerInfo();
                std::this_thread::sleep_for(std::chrono::seconds(300));
            }
        });
        t.detach();
    }

    void UpdatePlayerInfo()
    {
        QuestUI::MainThreadScheduler::Schedule([=]() {
            ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->set_loading(true);
        });
        GetPlayerInfo(playerInfo.localPlayerData.id, true, [=](std::optional<Data::Player> playerData) {
            if (playerData.has_value())
            {
                playerInfo.localPlayerData = playerData.value();
                QuestUI::MainThreadScheduler::Schedule([=]() {
                    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->set_ranking(playerInfo.localPlayerData.rank, playerInfo.localPlayerData.pp);
                });
            }
        });
    }

} // namespace ScoreSaber::Services::PlayerService
