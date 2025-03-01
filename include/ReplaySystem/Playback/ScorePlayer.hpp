#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/GameEnergyCounter.hpp>
#include <GlobalNamespace/ScoreController.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

#define INTERFACES                   \
    {                                \
        classof(Zenject::ITickable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Playback, ScorePlayer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Playback", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(int, _nextIndex);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::ScoreController>, _scoreController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::GameEnergyCounter>, _gameEnergyCounter);
                                    DECLARE_CTOR(ctor, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ScoreController* scoreController, GlobalNamespace::GameEnergyCounter* gameEnergyCounter);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
                                    DECLARE_INSTANCE_METHOD(void, TimeUpdate, float songTime);
                                    vector<Data::Private::NoteEvent> _sortedNoteEvents;
                                    vector<Data::Private::ScoreEvent> _sortedScoreEvents;
                                    void UpdateScore(int newScore, std::optional<int> immediateMaxPossibleScore, float time);
                                    void UpdateMultiplier();
                                    int CalculatePostNoteCountForTime(float time);)

#undef INTERFACES
