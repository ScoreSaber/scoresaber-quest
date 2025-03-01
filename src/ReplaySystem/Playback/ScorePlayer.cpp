
#include "ReplaySystem/Playback/ScorePlayer.hpp"
#include <GlobalNamespace/GameplayModifiersModelSO.hpp>
#include <GlobalNamespace/ScoreModel.hpp>
#include "ReplaySystem/ReplayLoader.hpp"
#include <System/Action_2.hpp>
#include "Utils/BeatmapUtils.hpp"

#include "logging.hpp"

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, ScorePlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void ScorePlayer::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ScoreController* scoreController, GlobalNamespace::GameEnergyCounter* gameEnergyCounter)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _scoreController = scoreController;
        _gameEnergyCounter = gameEnergyCounter;
        _sortedScoreEvents = ReplayLoader::LoadedReplay->scoreKeyframes;
        _sortedNoteEvents = ReplayLoader::LoadedReplay->noteKeyframes;
        std::sort(_sortedNoteEvents.begin(), _sortedNoteEvents.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.Time < rhs.Time;
        });
    }
    void ScorePlayer::Tick()
    {
        optional<int> recentMultipliedScore;
        optional<int> recentImmediateMaxPossibleScore;
        while (_nextIndex < _sortedScoreEvents.size() && _audioTimeSyncController->songTime >= _sortedScoreEvents[_nextIndex].Time) {
            ScoreEvent activeEvent = _sortedScoreEvents[_nextIndex++];
            recentMultipliedScore = activeEvent.Score;
            recentImmediateMaxPossibleScore = activeEvent.ImmediateMaxPossibleScore;
        }

        if (recentMultipliedScore.has_value()) {
            UpdateScore(recentMultipliedScore.value(), recentImmediateMaxPossibleScore, _audioTimeSyncController->songTime);
        }
    }

    void ScorePlayer::TimeUpdate(float newTime)
    {
        ScorePlayer::UpdateMultiplier();

        _nextIndex = _sortedScoreEvents.size();
        for (int c = 0; c < _sortedScoreEvents.size(); c++) {
            if (_sortedScoreEvents[c].Time > newTime) {
                _nextIndex = c;
                break;
            }
        }

        if (_nextIndex > 0) {
            auto scoreEvent = _sortedScoreEvents[_nextIndex - 1];
            UpdateScore(scoreEvent.Score, scoreEvent.ImmediateMaxPossibleScore, newTime);
        } else {
            UpdateScore(0, 0, newTime);
        }
    }

    void ScorePlayer::UpdateMultiplier()
    {
        float totalMultiplier = _scoreController->_gameplayModifiersModel->GetTotalMultiplier(_scoreController->_gameplayModifierParams, _gameEnergyCounter->energy);
        _scoreController->_prevMultiplierFromModifiers = totalMultiplier;
    }

    void ScorePlayer::UpdateScore(int newScore, optional<int> immediateMaxPossibleScore, float time)
    {
        int immediate;
        if (immediateMaxPossibleScore.has_value()) {
            immediate = immediateMaxPossibleScore.value();
        } else {
            immediate = BeatmapUtils::OldMaxRawScoreForNumberOfNotes(CalculatePostNoteCountForTime(time));
        }
        float multiplier = _scoreController->_prevMultiplierFromModifiers;

        int newModifiedScore = GlobalNamespace::ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(newScore, multiplier);

        _scoreController->_multipliedScore = newScore;
        _scoreController->_immediateMaxPossibleMultipliedScore = immediate;
        _scoreController->_modifiedScore = newModifiedScore;
        _scoreController->_immediateMaxPossibleModifiedScore = GlobalNamespace::ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(immediate, multiplier);

        if (_scoreController->scoreDidChangeEvent != nullptr) {
            _scoreController->scoreDidChangeEvent->Invoke(newScore, newModifiedScore);
        }
    }
    int ScorePlayer::CalculatePostNoteCountForTime(float time)
    {
        int count = 0;
        for (int c = 0; c < _sortedNoteEvents.size(); c++)
        {
            auto noteEvent = _sortedNoteEvents[c];
            if (noteEvent.Time > time)
            {
                break;
            }
            auto eventType = noteEvent.EventType;
            if (eventType == NoteEventType::GoodCut || eventType == NoteEventType::BadCut || eventType == NoteEventType::Miss)
            {
                count++;
            }
        }
        return count;
    }
} // namespace ScoreSaber::ReplaySystem::Playback