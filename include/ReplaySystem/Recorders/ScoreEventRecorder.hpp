#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/PrepareLevelCompletionResults.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::ReplaySystem::Recorders::ScoreEventRecorder
{
    void LevelStarted(ScoreController* scoreController, AudioTimeSyncController* audioTimeSyncController);
    void ComboController_comboDidChangeEvent(int combo);
    void ScoreController_scoreDidChangeEvent(int rawScore, int score);
    void ScoreController_multiplierDidChangeEvent(int multiplier, float nextMultiplierProgress);
    vector<ScoreEvent> ExportScoreKeyframes();
    vector<ComboEvent> ExportComboKeyframes();
    vector<MultiplierEvent> ExportMultiplierKeyframes();
} // namespace ScoreSaber::ReplaySystem::Recorders::ScoreEventRecorder