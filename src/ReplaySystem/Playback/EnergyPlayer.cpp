#include "ReplaySystem/Playback/EnergyPlayer.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include <System/Action_1.hpp>
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/Playables/PlayableDirector.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/UI/Image.hpp>
#include "logging.hpp"
#include <algorithm>

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, EnergyPlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void EnergyPlayer::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::GameEnergyCounter* gameEnergyCounter, Zenject::DiContainer* container)
    {
        _audioTimeSyncController = audioTimeSyncController;
        _gameEnergyCounter = gameEnergyCounter;
        _gameEnergyUIPanel = container->TryResolve<GlobalNamespace::GameEnergyUIPanel*>();
        _sortedEnergyEvents = ReplayLoader::LoadedReplay->energyKeyframes;
    }

    void EnergyPlayer::TimeUpdate(float newTime)
    {
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
        if (newTime >= lastEvent.Time && lastEvent.Energy <= Mathf::Epsilon)
        {
            UpdateEnergy(0.0f);
        }
    }

    void EnergyPlayer::UpdateEnergy(float energy)
    {
        bool isFailingEnergy = energy <= Mathf::Epsilon;
        bool noFail = _gameEnergyCounter->noFail;
        _gameEnergyCounter->noFail = false;
        _gameEnergyCounter->_didReach0Energy = isFailingEnergy;
        _gameEnergyCounter->ProcessEnergyChange(energy);
        _gameEnergyCounter->_nextFrameEnergyChange = 0.0f;
        _gameEnergyCounter->energy = energy;
        _gameEnergyCounter->noFail = noFail;

        if (_gameEnergyUIPanel != nullptr) {
            _gameEnergyUIPanel->Init();
            auto director = _gameEnergyUIPanel->_playableDirector;
            director->Stop();
            director->Evaluate();
            _gameEnergyUIPanel->_energyBar->enabled = !isFailingEnergy;
        }
        if (_gameEnergyCounter->gameEnergyDidChangeEvent != nullptr)
        {
            _gameEnergyCounter->gameEnergyDidChangeEvent->Invoke(energy);
        }
        return;
    }

} // namespace ScoreSaber::ReplaySystem::Playback