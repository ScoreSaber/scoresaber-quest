#pragma once
#include "ReplaySystem/Recorders/MainRecorder.hpp"
#include <vector>

namespace ScoreSaber::Services::ReplayService
{
    extern std::vector<char> CurrentSerializedReplay;
    extern std::function<void(const std::vector<char>&)> ReplaySerialized;

    void NewPlayStarted(ReplaySystem::Recorders::MainRecorder* _recorder);
    void WriteSerializedReplay();
}