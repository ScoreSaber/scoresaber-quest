#pragma once
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "Utils/StringUtils.hpp"
#include "custom-types/shared/macros.hpp"
#include "logging.hpp"
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder
{
    void LevelStarted(ScoreController* scoreController, AudioTimeSyncController* audioTimeSyncController);
    void ScoreController_scoringForNoteStartedEvent(ScoringElement* element);
    void ScoreController_scoringForNoteFinishedEvent(ScoringElement* element);
    void BadCutInfoCollector(NoteController* noteController, const NoteCutInfo& noteCutInfo);
    vector<NoteEvent> Export();
} // namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder