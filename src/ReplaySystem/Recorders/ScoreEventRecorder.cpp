#include "ReplaySystem/Recorders/ScoreEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/PlayerHeightDetector.hpp>
#include <GlobalNamespace/PrepareLevelCompletionResults.hpp>
#include <System/Action_1.hpp>
#include <System/Action_2.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Time.hpp>
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <custom-types/shared/delegate.hpp>
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders, ScoreEventRecorder);

namespace ScoreSaber::ReplaySystem::Recorders
{
    void ScoreEventRecorder::ctor(AudioTimeSyncController* audioTimeSyncController, ScoreController* scoreController, ComboController* comboController)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _scoreController = scoreController;
        _comboController = comboController;
    }

    void ScoreEventRecorder::Initialize()
    {
        comboDidChangeDelegate = { &ScoreEventRecorder::ComboController_comboDidChangeEvent, this };
        scoreDidChangeDelegate = { &ScoreEventRecorder::ScoreController_scoreDidChangeEvent, this };
        multiplierDidChangeDelegate = { &ScoreEventRecorder::ScoreController_multiplierDidChangeEvent, this };
        
        _comboController->___comboDidChangeEvent += comboDidChangeDelegate;
        _scoreController->___scoreDidChangeEvent += scoreDidChangeDelegate;
        _scoreController->___multiplierDidChangeEvent += multiplierDidChangeDelegate;
    }

    void ScoreEventRecorder::Dispose()
    {
        _comboController->___comboDidChangeEvent -= comboDidChangeDelegate;
        _scoreController->___scoreDidChangeEvent -= scoreDidChangeDelegate;
        _scoreController->___multiplierDidChangeEvent -= multiplierDidChangeDelegate;
    }

    void ScoreEventRecorder::ComboController_comboDidChangeEvent(int combo)
    {
        _comboKeyFrames.push_back(ComboEvent(combo, _audioTimeSyncController->songTime));
    }

    void ScoreEventRecorder::ScoreController_scoreDidChangeEvent(int rawScore, int score)
    {
        _scoreKeyFrames.push_back(ScoreEvent(rawScore, _audioTimeSyncController->songTime, _scoreController->immediateMaxPossibleMultipliedScore));
    }

    void ScoreEventRecorder::ScoreController_multiplierDidChangeEvent(int multiplier, float nextMultiplierProgress)
    {
        _multiplierKeyFrames.push_back(MultiplierEvent(multiplier, nextMultiplierProgress, _audioTimeSyncController->songTime));
    }

    vector<ScoreEvent> ScoreEventRecorder::ExportScoreKeyframes()
    {
        return _scoreKeyFrames;
    }

    vector<ComboEvent> ScoreEventRecorder::ExportComboKeyframes()
    {
        return _comboKeyFrames;
    }

    vector<MultiplierEvent> ScoreEventRecorder::ExportMultiplierKeyframes()
    {
        return _multiplierKeyFrames;
    }

} // namespace ScoreSaber::ReplaySystem::Recorders