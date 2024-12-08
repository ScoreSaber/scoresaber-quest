#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/BeatmapObjectManager.hpp>
#include <GlobalNamespace/NoteController.hpp>
#include <GlobalNamespace/NoteCutInfo.hpp>
#include <GlobalNamespace/BeatmapObjectManager.hpp>
#include <GlobalNamespace/ScoreController.hpp>
#include <System/IDisposable.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include <vector>
#include <map>

using namespace UnityEngine;
using namespace GlobalNamespace;


#define INTERFACES                                                        \
    {                                                                     \
        classof(System::IDisposable*), classof(Zenject::IInitializable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, NoteEventRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(ScoreController*, _scoreController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(BeatmapObjectManager*, _beatmapObjectManager);
                                    DECLARE_CTOR(ctor, AudioTimeSyncController* audioTimeSyncController, ScoreController* scoreController, BeatmapObjectManager* beatmapObjectManager);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
                                    DECLARE_INSTANCE_METHOD(void, ScoreController_scoringForNoteStartedEvent, ScoringElement* element);
                                    DECLARE_INSTANCE_METHOD(void, ScoreController_scoringForNoteFinishedEvent, ScoringElement* element);
                                    DECLARE_INSTANCE_METHOD(void, BadCutInfoCollector, NoteController* noteController, NoteCutInfo* noteCutInfo);
                                    std::vector<Data::Private::NoteEvent> _noteKeyframes;
                                    std::map<NoteData*, NoteCutInfo> _collectedBadCutInfos;
                                    std::map<GoodCutScoringElement*, float> _scoringStartInfo;
                                    System::Action_1<ScoringElement*> *scoringForNoteStartedDelegate;
                                    System::Action_1<ScoringElement*> *scoringForNoteFinishedDelegate;
                                    BeatmapObjectManager::NoteWasCutDelegate *handleNoteWasCutDelegate;
                                    public:
                                    std::vector<Data::Private::NoteEvent> Export();
                                    )

#undef INTERFACES
