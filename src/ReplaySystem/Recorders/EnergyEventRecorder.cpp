#include "ReplaySystem/Recorders/EnergyEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
#include "System/Action_1.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::ReplaySystem::Recorders::EnergyEventRecorder
{
    AudioTimeSyncController* _audioTimeSyncController;
    GameEnergyCounter* _gameEnergyCounter;
    vector<EnergyEvent> _energyKeyFrames;

    void LevelStarted(GameEnergyCounter* gameEnergyCounter, AudioTimeSyncController* audioTimeSyncController)
    {
        _energyKeyFrames.clear();
        _audioTimeSyncController = audioTimeSyncController;
        _gameEnergyCounter = gameEnergyCounter;

        std::function<void(float)> gameEnergyDidChangeCallback = [&](float energy) {
            GameEnergyCounter_gameEnergyDidChangeEvent(energy);
        };

        auto gameEnergyDidChangeDelegate = il2cpp_utils::MakeDelegate<System::Action_1<float>*>(classof(System::Action_1<float>*), gameEnergyDidChangeCallback);
        gameEnergyCounter->add_gameEnergyDidChangeEvent(gameEnergyDidChangeDelegate);
    }

    void GameEnergyCounter_gameEnergyDidChangeEvent(float energy)
    {
        _energyKeyFrames.push_back(EnergyEvent(energy, _audioTimeSyncController->songTime));
    }

    vector<EnergyEvent> Export()
    {
        return _energyKeyFrames;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders::EnergyEventRecorder