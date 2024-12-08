#include "ReplaySystem/Playback/MultiplierPlayer.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include <System/Action_2.hpp>
#include "logging.hpp"
#include <algorithm>

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, MultiplierPlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void MultiplierPlayer::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ScoreController* scoreController)
    {
        _audioTimeSyncController = audioTimeSyncController;
        _scoreController = scoreController;
        _sortedMultiplierEvents = ReplayLoader::LoadedReplay->multiplierKeyframes;
    }
    void MultiplierPlayer::TimeUpdate(float newTime)
    {
        for (int c = 0; c < _sortedMultiplierEvents.size(); c++)
        {
            // TODO: this has potential to have problems if _sortedMultiplierEvents[c].Time is within an epsilon of newTime, potentially applying combo changes twice or not at all
            if (_sortedMultiplierEvents[c].Time > newTime)
            {
                int multiplier = c != 0 ? _sortedMultiplierEvents[c - 1].Multiplier : 1;
                float progress = c != 0 ? _sortedMultiplierEvents[c - 1].NextMultiplierProgress : 0.0f;
                UpdateMultiplier(multiplier, progress);
                return;
            }
        }
        if (_sortedMultiplierEvents.size() > 0) {
            auto lastEvent = _sortedMultiplierEvents[_sortedMultiplierEvents.size() - 1];
            UpdateMultiplier(lastEvent.Multiplier, lastEvent.NextMultiplierProgress);
        }
    }
    void MultiplierPlayer::UpdateMultiplier(int multiplier, float progress)
    {
        auto counter = _scoreController->_scoreMultiplierCounter;
        counter->_multiplier = multiplier;
        counter->_multiplierIncreaseMaxProgress = multiplier * 2;
        counter->_multiplierIncreaseProgress = (int)(progress * (multiplier * 2));
        if (_scoreController->multiplierDidChangeEvent != nullptr)
        {
            _scoreController->multiplierDidChangeEvent->Invoke(multiplier, progress);
        }
    }
} // namespace ScoreSaber::ReplaySystem::Playback