#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/IGameEnergyCounter.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/IInitializable.hpp"
#include "Zenject/ITickable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#define INTERFACES                   \
    {                                \
        classof(Zenject::ITickable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Playback, ScorePlayer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Playback", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(int, _lastIndex);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::ScoreController*, _scoreController);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::IGameEnergyCounter*, _gameEnergyCounter);
                                    DECLARE_CTOR(ctor, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ScoreController* scoreController, GlobalNamespace::IGameEnergyCounter* gameEnergyCounter);
                                    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());
                                    DECLARE_INSTANCE_METHOD(void, TimeUpdate, float songTime);
                                    vector<Data::Private::NoteEvent> _sortedNoteEvents;
                                    vector<Data::Private::ScoreEvent> _sortedScoreEvents;
                                    void UpdateScore(int newScore, float time);
                                    int CalculatePostNoteCountForTime(float time);)

#undef INTERFACES
