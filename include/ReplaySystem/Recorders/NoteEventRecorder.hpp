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
#include "Utils/DelegateUtils.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::ReplaySystem::Recorders,
        NoteEventRecorder,
        System::Object,
        System::IDisposable*,
        Zenject::IInitializable*) {
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::ScoreController>, _scoreController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapObjectManager*, _beatmapObjectManager);
    DECLARE_CTOR(ctor, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ScoreController* scoreController, GlobalNamespace::BeatmapObjectManager* beatmapObjectManager);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_INSTANCE_METHOD(void, ScoreController_scoringForNoteStartedEvent, GlobalNamespace::ScoringElement* element);
    DECLARE_INSTANCE_METHOD(void, ScoreController_scoringForNoteFinishedEvent, GlobalNamespace::ScoringElement* element);
    DECLARE_INSTANCE_METHOD(void, BadCutInfoCollector, GlobalNamespace::NoteController* noteController, ByRef<GlobalNamespace::NoteCutInfo> noteCutInfo);
    std::vector<Data::Private::NoteEvent> _noteKeyframes;
    std::map<GlobalNamespace::NoteData*, GlobalNamespace::NoteCutInfo> _collectedBadCutInfos;
    std::map<GlobalNamespace::GoodCutScoringElement*, float> _scoringStartInfo;
    DelegateUtils::DelegateW<System::Action_1<GlobalNamespace::ScoringElement*>> scoringForNoteStartedDelegate;
    DelegateUtils::DelegateW<System::Action_1<GlobalNamespace::ScoringElement*>> scoringForNoteFinishedDelegate;
    DelegateUtils::DelegateW<GlobalNamespace::BeatmapObjectManager::NoteWasCutDelegate> handleNoteWasCutDelegate;
public:
    std::vector<Data::Private::NoteEvent> Export();
};