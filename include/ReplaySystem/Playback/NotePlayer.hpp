#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/BasicBeatmapObjectManager.hpp>
#include <GlobalNamespace/BombNoteController.hpp>
#include <GlobalNamespace/CutScoreBuffer.hpp>
#include <GlobalNamespace/GameNoteController.hpp>
#include <GlobalNamespace/GoodCutScoringElement.hpp>
#include <GlobalNamespace/MemoryPoolContainer_1.hpp>
#include <GlobalNamespace/NoteController.hpp>
#include <GlobalNamespace/NoteCutInfo.hpp>
#include <GlobalNamespace/NoteData.hpp>
#include <GlobalNamespace/SaberManager.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include <vector>

// std::map<GoodCutScoringElement*, float> _scoringStartInfo;

struct RecognizedNoteCutInfo
{
    RecognizedNoteCutInfo(GlobalNamespace::NoteCutInfo info, ScoreSaber::Data::Private::NoteEvent event)
        : info(info), event(event)
    {
    }
    GlobalNamespace::NoteCutInfo info;
    ScoreSaber::Data::Private::NoteEvent event;
};

#define INTERFACES                   \
    {                                \
        classof(Zenject::ITickable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Playback, NotePlayer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Playback", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(int, _nextIndex);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::SaberManager>, _saberManager);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BasicBeatmapObjectManager*, _basicBeatmapObjectManager);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MemoryPoolContainer_1<UnityW<GlobalNamespace::GameNoteController>>*, _gameNotePool);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MemoryPoolContainer_1<UnityW<GlobalNamespace::GameNoteController>>*, _burstSliderHeadNotePool);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MemoryPoolContainer_1<UnityW<GlobalNamespace::BurstSliderGameNoteController>>*, _burstSliderNotePool);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MemoryPoolContainer_1<UnityW<GlobalNamespace::BombNoteController>>*, _bombNotePool);
                                    DECLARE_INSTANCE_METHOD(void, ForceCompleteGoodScoringElements, GlobalNamespace::GoodCutScoringElement* scoringElement, GlobalNamespace::NoteCutInfo noteCutInfo, GlobalNamespace::CutScoreBuffer* cutScoreBuffer);
                                    DECLARE_CTOR(ctor, GlobalNamespace::SaberManager* saberManager, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::BasicBeatmapObjectManager* basicBeatmapObjectManager);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
                                    DECLARE_INSTANCE_METHOD(void, TimeUpdate, float songTime);
                                    std::vector<Data::Private::NoteEvent> _sortedNoteEvents;
                                    std::vector<RecognizedNoteCutInfo> _recognizedNoteCutInfos;
                                    // std::map<GlobalNamespace::NoteCutInfo, ScoreSaber::Data::Private::NoteEvent> _recognizedNoteCutInfos;
                                    // System::Collections::Generic::Dictionary_2<GlobalNamespace::NoteCutInfo, ScoreSaber::Data::Private::NoteEvent> * _recognizedNoteCutInfos;
                                    void ProcessEvent(Data::Private::NoteEvent &activeEvent);
                                    bool HandleEvent(Data::Private::NoteEvent &activeEvent, GlobalNamespace::NoteController* noteController);
                                    bool DoesNoteMatchID(Data::Private::NoteID &id, GlobalNamespace::NoteData* noteData);)

#undef INTERFACES
