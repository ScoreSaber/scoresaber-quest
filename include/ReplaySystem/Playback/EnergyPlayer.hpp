#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/GameEnergyCounter.hpp>
#include <GlobalNamespace/GameEnergyUIPanel.hpp>
#include <GlobalNamespace/PlayerDataModel.hpp>
#include <Zenject/DiContainer.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(ScoreSaber::ReplaySystem::Playback, EnergyPlayer, Il2CppObject) {
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::GameEnergyCounter>, _gameEnergyCounter);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::GameEnergyUIPanel>, _gameEnergyUIPanel);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::PlayerDataModel>, _playerDataModel);
    DECLARE_CTOR(ctor, GlobalNamespace::GameEnergyCounter* gameEnergyCounter, GlobalNamespace::PlayerDataModel* playerDataModel, Zenject::DiContainer* container);
    DECLARE_INSTANCE_METHOD(void, TimeUpdate, float songTime);
    vector<Data::Private::EnergyEvent> _sortedEnergyEvents;
    void UpdateEnergy(float energy);
};
