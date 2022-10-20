
#include "ReplaySystem/Playback/ScorePlayer.hpp"
#include "GlobalNamespace/GameplayModifiersModelSO.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "System/Action_2.hpp"
#include "Utils/BeatmapUtils.hpp"

#include "logging.hpp"

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, ScorePlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void ScorePlayer::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ScoreController* scoreController, GlobalNamespace::IGameEnergyCounter* gameEnergyCounter)
    {
        _audioTimeSyncController = audioTimeSyncController;
        _scoreController = scoreController;
        _gameEnergyCounter = gameEnergyCounter;
        _sortedNoteEvents = ReplayLoader::LoadedReplay->noteKeyframes;
        _sortedScoreEvents = ReplayLoader::LoadedReplay->scoreKeyframes;
        std::sort(_sortedNoteEvents.begin(), _sortedNoteEvents.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.Time < rhs.Time;
        });
    }
    void ScorePlayer::Tick()
    {
        if (_lastIndex >= _sortedScoreEvents.size())
        {
            return;
        }
        int recentMultipliedScore = -1;
        while (_audioTimeSyncController->songTime >= _sortedScoreEvents[_lastIndex].Time)
        {
            ScoreEvent activeEvent = _sortedScoreEvents[_lastIndex++];
            recentMultipliedScore = _scoreController->multipliedScore = activeEvent.Score;
            if (_lastIndex >= _sortedScoreEvents.size())
            {
                break;
            }
        }

        if (recentMultipliedScore != -1)
        {
            auto postNoteCount = CalculatePostNoteCountForTime(_audioTimeSyncController->songTime);
            _scoreController->immediateMaxPossibleMultipliedScore = BeatmapUtils::OldMaxRawScoreForNumberOfNotes(postNoteCount);

            if (_scoreController->scoreDidChangeEvent != nullptr)
            {
                _scoreController->scoreDidChangeEvent->Invoke(recentMultipliedScore, GlobalNamespace::ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(recentMultipliedScore, _scoreController->prevMultiplierFromModifiers));
            }
        }
    }

    void ScorePlayer::TimeUpdate(float newTime)
    {
        for (int c = 0; c < _sortedScoreEvents.size(); c++)
        {
            if (_sortedScoreEvents[c].Time >= newTime)
            {
                _lastIndex = c;
                Tick();
                UpdateScore(c != 0 ? _sortedScoreEvents[c - 1].Score : 0, newTime);
                break;
            }
        }
        UpdateScore(_sortedScoreEvents[_sortedScoreEvents.size() - 1].Score, newTime);
    }

    void ScorePlayer::UpdateScore(int newScore, float time)
    {
        int postNoteCount = CalculatePostNoteCountForTime(time);
        float totalMultiplier = _scoreController->gameplayModifiersModel->GetTotalMultiplier(_scoreController->gameplayModifierParams, _gameEnergyCounter->get_energy());
        _scoreController->prevMultiplierFromModifiers = totalMultiplier;
        int immediate = _scoreController->immediateMaxPossibleMultipliedScore = BeatmapUtils::OldMaxRawScoreForNumberOfNotes(postNoteCount);
        int earlyScore = newScore;
        _scoreController->multipliedScore = earlyScore;
        if (_scoreController->scoreDidChangeEvent != nullptr)
        {
            _scoreController->scoreDidChangeEvent->Invoke(earlyScore, GlobalNamespace::ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(earlyScore, totalMultiplier));
        }
    }
    int ScorePlayer::CalculatePostNoteCountForTime(float time)
    {
        int count = 0;
        for (int c = 0; c < _sortedNoteEvents.size(); c++)
        {
            if (_sortedNoteEvents[c].Time > time)
            {
                break;
            }
            auto eventType = _sortedNoteEvents[c].EventType;
            if (eventType == NoteEventType::GoodCut || eventType == NoteEventType::BadCut || eventType == NoteEventType::Miss)
            {
                count++;
            }
        }
        return count;
    }
} // namespace ScoreSaber::ReplaySystem::Playback