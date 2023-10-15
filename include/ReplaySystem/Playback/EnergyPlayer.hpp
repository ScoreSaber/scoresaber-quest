#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
#include "GlobalNamespace/GameEnergyUIPanel.hpp"
#include "Zenject/DiContainer.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(
    ScoreSaber::ReplaySystem::Playback, EnergyPlayer, Il2CppObject,
    DECLARE_PRIVATE_FIELD(GlobalNamespace::AudioTimeSyncController*, _audioTimeSyncController);
    DECLARE_PRIVATE_FIELD(GlobalNamespace::GameEnergyCounter*, _gameEnergyCounter);
    DECLARE_PRIVATE_FIELD(GlobalNamespace::GameEnergyUIPanel*, _gameEnergyUIPanel);
    DECLARE_CTOR(ctor, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::GameEnergyCounter* gameEnergyCounter, Zenject::DiContainer* container);
    DECLARE_INSTANCE_METHOD(void, TimeUpdate, float songTime);
    vector<Data::Private::EnergyEvent> _sortedEnergyEvents;
    void UpdateEnergy(float energy);)
