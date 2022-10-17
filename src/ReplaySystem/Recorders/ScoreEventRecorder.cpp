#include "ReplaySystem/Recorders/ScoreEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/PlayerHeightDetector.hpp"
#include "GlobalNamespace/PrepareLevelCompletionResults.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "System/Action_1.hpp"
#include "System/Action_2.hpp"
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
    AudioTimeSyncController* _audioTimeSyncController;
    ScoreController* _scoreController;
    vector<ScoreEvent> _scoreKeyFrames;
    vector<ComboEvent> _comboKeyFrames;
    vector<MultiplierEvent> _multiplierKeyFrames;

    void LevelStarted(ScoreController* scoreController, AudioTimeSyncController* audioTimeSyncController)
    {
        _scoreKeyFrames.clear();
        _comboKeyFrames.clear();
        _multiplierKeyFrames.clear();
        _audioTimeSyncController = audioTimeSyncController;
        _scoreController = scoreController;

        // std::function<void(int)> comboDidChangeCallback = [&](int combo) {
        //     ComboController_comboDidChangeEvent(combo);
        // };

        // std::function<void(int, int)> scoreDidChangeCallback = [&](int rawScore, int score) {
        //     ScoreController_scoreDidChangeEvent(rawScore, score);
        // };

        // std::function<void(int, float)> multiplierDidChangeCallback = [&](int multiplier, float nextMultiplierProgress) {
        //     ScoreController_multiplierDidChangeEvent(multiplier, nextMultiplierProgress);
        // };

        // auto comboDidChangeDelegate = il2cpp_utils::MakeDelegate<System::Action_1<int>*>(classof(System::Action_1<int>*), comboDidChangeCallback);
        // auto scoreDidChangeDelegate = il2cpp_utils::MakeDelegate<System::Action_2<int, int>*>(classof(System::Action_2<int, int>*), scoreDidChangeCallback);
        // auto multiplierDidChangeDelegate = il2cpp_utils::MakeDelegate<System::Action_2<int, float>*>(classof(System::Action_2<int, float>*), multiplierDidChangeCallback);

        // scoreController->add_comboDidChangeEvent(comboDidChangeDelegate);
        // scoreController->add_scoreDidChangeEvent(scoreDidChangeDelegate);
        // scoreController->add_multiplierDidChangeEvent(multiplierDidChangeDelegate);
    }

    void ComboController_comboDidChangeEvent(int combo)
    {
        _comboKeyFrames.push_back(ComboEvent(combo, _audioTimeSyncController->songTime));
    }

    void ScoreController_scoreDidChangeEvent(int rawScore, int score)
    {
        _scoreKeyFrames.push_back(ScoreEvent(rawScore, _audioTimeSyncController->songTime));
    }

    void ScoreController_multiplierDidChangeEvent(int multiplier, float nextMultiplierProgress)
    {
        _multiplierKeyFrames.push_back(MultiplierEvent(multiplier, nextMultiplierProgress, _audioTimeSyncController->songTime));
    }

    vector<ScoreEvent> ExportScoreKeyframes()
    {
        return _scoreKeyFrames;
    }

    vector<ComboEvent> ExportComboKeyframes()
    {
        return _comboKeyFrames;
    }

    vector<MultiplierEvent> ExportMultiplierKeyframes()
    {
        return _multiplierKeyFrames;
    }

} // namespace ScoreSaber::ReplaySystem::Recorders::ScoreEventRecorder