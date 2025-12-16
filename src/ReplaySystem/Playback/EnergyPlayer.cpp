#include "ReplaySystem/Playback/EnergyPlayer.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include <GlobalNamespace/PlayerData.hpp>
#include <GlobalNamespace/PlayerSpecificSettings.hpp>
#include <System/Action_1.hpp>
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/Playables/PlayableDirector.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/UI/Image.hpp>
#include <UnityEngine/Mathf.hpp>
#include "logging.hpp"
#include <metacore/shared/internals.hpp>

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, EnergyPlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void EnergyPlayer::ctor(GlobalNamespace::GameEnergyCounter* gameEnergyCounter, GlobalNamespace::PlayerDataModel* playerDataModel, Zenject::DiContainer* container)
    {
        INVOKE_CTOR();
        _gameEnergyCounter = gameEnergyCounter;
        _playerDataModel = playerDataModel;
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
        if (newTime >= lastEvent.Time && lastEvent.Energy <= Mathf::getStaticF_Epsilon())
        {
            UpdateEnergy(0.0f);

        }
    }

    void EnergyPlayer::UpdateEnergy(float energy)
    {
        bool isFailingEnergy = energy <= Mathf::getStaticF_Epsilon();
        bool noFail = _gameEnergyCounter->noFail;
        _gameEnergyCounter->noFail = false;
        _gameEnergyCounter->_didReach0Energy = isFailingEnergy;
        _gameEnergyCounter->ProcessEnergyChange(energy);
        _gameEnergyCounter->_nextFrameEnergyChange = 0.0f;
        _gameEnergyCounter->energy = energy;
        _gameEnergyCounter->noFail = noFail;

        if (_gameEnergyUIPanel != nullptr && !_playerDataModel->playerData->playerSpecificSettings->noTextsAndHuds) {
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
        
        // forgive me
        MetaCore::Internals::health = energy;

        MetaCore::Internals::wallsHit = 0;

        float lastEnergy = 0.5f;
        for (const auto& ev : _sortedEnergyEvents)
        {
            if (ev.Time > MetaCore::Internals::audioTimeSyncController->_songTime)
            {
                break;
            }
            if (ev.Energy < lastEnergy) // weve lost energy by any means so just set wallhit for ui to be correct
            {
                MetaCore::Internals::wallsHit = 1; // just needs to be > 0 for ui to be correct (will implement precalculation later)
            }
        }
        return;
    }

} // namespace ScoreSaber::ReplaySystem::Playback