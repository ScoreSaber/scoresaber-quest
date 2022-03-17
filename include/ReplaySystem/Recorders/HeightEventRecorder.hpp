#pragma once
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/PlayerHeightDetector.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <functional>

using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::ReplaySystem::Recorders::HeightEventRecorder
{
    void LevelStarted(PlayerHeightDetector* playerHeightDetector, AudioTimeSyncController* audioTimeSyncController);
    void PlayerHeightDetector_playerHeightDidChangeEvent(float newHeight);
    vector<HeightEvent> Export();

} // namespace ScoreSaber::ReplaySystem::Recorders::HeightEventRecorder