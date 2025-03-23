#include "ReplaySystem/Recorders/EnergyEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/GameEnergyCounter.hpp>
#include <System/Action_1.hpp>
#include "logging.hpp"
#include <custom-types/shared/delegate.hpp>
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders, EnergyEventRecorder);

namespace ScoreSaber::ReplaySystem::Recorders
{
    void EnergyEventRecorder::ctor(AudioTimeSyncController* audioTimeSyncController, GameEnergyCounter* gameEnergyCounter)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _gameEnergyCounter = gameEnergyCounter;
    }

    void EnergyEventRecorder::Initialize()
    {
        if(_gameEnergyCounter) {
            gameEnergyDidChangeDelegate = { &EnergyEventRecorder::GameEnergyCounter_gameEnergyDidChangeEvent, this };
            _gameEnergyCounter->___gameEnergyDidChangeEvent += gameEnergyDidChangeDelegate;
        }
    }

    void EnergyEventRecorder::Dispose()
    {
        if(_gameEnergyCounter && gameEnergyDidChangeDelegate) {
            _gameEnergyCounter->___gameEnergyDidChangeEvent -= gameEnergyDidChangeDelegate;
        }
    }

    void EnergyEventRecorder::GameEnergyCounter_gameEnergyDidChangeEvent(float energy)
    {
        _energyKeyframes.push_back(EnergyEvent(energy, _audioTimeSyncController->songTime));
    }

    vector<EnergyEvent> EnergyEventRecorder::Export()
    {
        return _energyKeyframes;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders