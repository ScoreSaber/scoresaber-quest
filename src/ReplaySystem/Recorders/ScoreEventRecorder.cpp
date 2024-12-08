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
        comboDidChangeDelegate = custom_types::MakeDelegate<System::Action_1<int>*>((function<void(int)>)[&](int combo) {ComboController_comboDidChangeEvent(combo);});
        scoreDidChangeDelegate = custom_types::MakeDelegate<System::Action_2<int, int>*>((function<void(int, int)>)[&](int rawScore, int score) {ScoreController_scoreDidChangeEvent(rawScore, score);});
        multiplierDidChangeDelegate = custom_types::MakeDelegate<System::Action_2<int, float>*>((function<void(int, float)>)[&](int multiplier, float nextMultiplierProgress) {ScoreController_multiplierDidChangeEvent(multiplier, nextMultiplierProgress);});
        
        _comboController->add_comboDidChangeEvent(comboDidChangeDelegate);
        _scoreController->add_scoreDidChangeEvent(scoreDidChangeDelegate);
        _scoreController->add_multiplierDidChangeEvent(multiplierDidChangeDelegate);
    }

    void ScoreEventRecorder::Dispose()
    {
        _comboController->remove_comboDidChangeEvent(comboDidChangeDelegate);
        _scoreController->remove_scoreDidChangeEvent(scoreDidChangeDelegate);
        _scoreController->remove_multiplierDidChangeEvent(multiplierDidChangeDelegate);
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