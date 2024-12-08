
#pragma once
#include "Data/Player.hpp"
#include <string>
namespace ScoreSaber::Services::PlayerService
{
    enum LoginStatus
    {
        Error,
        Success,
    };

    typedef struct playerInfo_t
    {
        std::string playerKey;
        std::string serverKey;
        LoginStatus loginStatus;
        Data::Player localPlayerData;
    } playerInfo_t;

    extern playerInfo_t playerInfo;

    void AuthenticateUser(std::function<void(LoginStatus)> finished);
    // void GetPlayerInfo(std::string playerId, bool full, std::function<void(std::optional<Data::Player>)> finished)
    void GetPlayerInfo(std::string playerId, bool full, std::function<void(std::optional<Data::Player>)> finished);

    void UpdatePlayerInfo(bool fromMainThread);
    void UpdatePlayerInfoThread();

    void OnSoftRestart();

} // namespace ScoreSaber::Services::PlayerService
