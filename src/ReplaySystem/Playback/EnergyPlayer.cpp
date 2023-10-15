#include "ReplaySystem/Playback/EnergyPlayer.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "System/Action_1.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Playables/PlayableDirector.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "logging.hpp"
#include <algorithm>

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, EnergyPlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void EnergyPlayer::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::GameEnergyCounter* gameEnergyCounter)
    {
        _audioTimeSyncController = audioTimeSyncController;
        _gameEnergyCounter = gameEnergyCounter;
        _gameEnergyUIPanel = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameEnergyUIPanel*>()->values[0];
        _sortedEnergyEvents = ReplayLoader::LoadedReplay->energyKeyframes;
    }

    void EnergyPlayer::TimeUpdate(float newTime)
    {
        // TODO: How would this ever be null? We get the object via Zenject and that would fail if it was null     
        if (_gameEnergyUIPanel == nullptr)
        {
            return;
        }
        for (int c = 0; c < _sortedEnergyEvents.size(); c++)
        {
            // TODO: this has potential to have problems if _sortedEnergyEvents[c].Time is within an epsilon of newTime, potentially applying energy changes twice or not at all
            if (_sortedEnergyEvents[c].Time > newTime)
            {
                float energy = c != 0 ? _sortedEnergyEvents[c - 1].Energy : 0.5f;
                UpdateEnergy(energy);
                return;
            }
        }
        UpdateEnergy(0.5f);
        auto lastEvent = _sortedEnergyEvents[_sortedEnergyEvents.size() - 1];
        if (newTime >= lastEvent.Time && lastEvent.Energy <= Mathf::_get_Epsilon())
        {
            UpdateEnergy(0.0f);
        }
    }

    void EnergyPlayer::UpdateEnergy(float energy)
    {
        if (_gameEnergyUIPanel == nullptr)
        {
            return;
        }

        bool isFailingEnergy = energy <= Mathf::_get_Epsilon();
        bool noFail = _gameEnergyCounter->noFail;
        _gameEnergyCounter->noFail = false;
        _gameEnergyCounter->didReach0Energy = isFailingEnergy;
        _gameEnergyCounter->ProcessEnergyChange(energy);
        _gameEnergyCounter->nextFrameEnergyChange = 0.0f;
        _gameEnergyCounter->energy = energy;
        _gameEnergyCounter->noFail = noFail;

        _gameEnergyUIPanel->Init();
        auto director = _gameEnergyUIPanel->playableDirector;
        director->Stop();
        director->Evaluate();
        _gameEnergyUIPanel->energyBar->set_enabled(!isFailingEnergy);
        if (_gameEnergyCounter->gameEnergyDidChangeEvent != nullptr)
        {
            _gameEnergyCounter->gameEnergyDidChangeEvent->Invoke(energy);
        }
        return;
    }

} // namespace ScoreSaber::ReplaySystem::Playback