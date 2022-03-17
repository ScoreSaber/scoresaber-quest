#pragma once
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <functional>

using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::ReplaySystem::Recorders::EnergyEventRecorder
{
    void LevelStarted(GameEnergyCounter* gameEnergyCounter, AudioTimeSyncController* audioTimeSyncController);
    void GameEnergyCounter_gameEnergyDidChangeEvent(float energy);
    vector<EnergyEvent> Export();
} // namespace ScoreSaber::ReplaySystem::Recorders::EnergyEventRecorder