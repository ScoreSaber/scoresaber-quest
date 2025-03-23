#pragma once

#include <string>

namespace ScoreSaber::Services::FileService
{
    void EnsurePaths();
    std::string GetReplayFileName(std::string levelId, std::string difficultyName, std::string characteristic, std::string playerId, std::string songName);
}