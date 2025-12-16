#include "ReplaySystem/Playback/MultiplierPlayer.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include <System/Action_2.hpp>
#include "logging.hpp"
#include "ReplaySystem/Playback/TimeUpdateUtils.hpp"
#include "metacore/shared/internals.hpp"

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, MultiplierPlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void MultiplierPlayer::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ScoreController* scoreController)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _scoreController = scoreController;
        _sortedMultiplierEvents = ReplayLoader::LoadedReplay->multiplierKeyframes;
    }

    void MultiplierPlayer::TimeUpdate(float newTime)
    {
        INFO("MultiplierPlayer::TimeUpdate newTime: {}", newTime);

        int index = FindNextEventIndex(newTime, _sortedMultiplierEvents);

        if (index == 0)
        {
            UpdateMultiplier(1, 0.0f);
            return;
        }

        if (index < _sortedMultiplierEvents.size())
        {
            const auto& ev = _sortedMultiplierEvents[index - 1];
            UpdateMultiplier(ev.Multiplier, ev.NextMultiplierProgress);
        }
        else
        {
            const auto& last = _sortedMultiplierEvents.back();
            UpdateMultiplier(last.Multiplier, last.NextMultiplierProgress);
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
        MetaCore::Internals::multiplier = multiplier;
        MetaCore::Internals::multiplierProgress = progress;
    }
} // namespace ScoreSaber::ReplaySystem::Playback