#pragma once
#include <vector>

namespace ScoreSaber::Services::ReplayService
{
    extern std::vector<char> CurrentSerializedReplay;
    extern std::function<void(const std::vector<char>&)> ReplaySerialized;

    void WriteSerializedReplay();
}