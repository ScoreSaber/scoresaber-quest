#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/CutScoreBuffer.hpp>
#include <GlobalNamespace/ISaberSwingRatingCounter.hpp>
#include <GlobalNamespace/NoteController.hpp>
#include <GlobalNamespace/NoteCutInfo.hpp>
#include <GlobalNamespace/NoteData.hpp>
#include <GlobalNamespace/ScoreController.hpp>
#include <System/Action_1.hpp>
#include <UnityEngine/Quaternion.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Time.hpp>
#include <UnityEngine/Vector3.hpp>
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <custom-types/shared/delegate.hpp>
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;
using namespace System::Collections::Generic;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders, NoteEventRecorder);

namespace ScoreSaber::ReplaySystem::Recorders
{
    void NoteEventRecorder::ctor(AudioTimeSyncController* audioTimeSyncController, ScoreController* scoreController, BeatmapObjectManager* beatmapObjectManager)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _scoreController = scoreController;
        _beatmapObjectManager = beatmapObjectManager;
    }

    void NoteEventRecorder::Initialize()
    {
        scoringForNoteStartedDelegate = custom_types::MakeDelegate<System::Action_1<ScoringElement*>*>((function<void(ScoringElement*)>)[&](ScoringElement* element) {ScoreController_scoringForNoteStartedEvent(element);});
        scoringForNoteFinishedDelegate = custom_types::MakeDelegate<System::Action_1<ScoringElement*>*>((function<void(ScoringElement*)>)[&](ScoringElement* element) {ScoreController_scoringForNoteFinishedEvent(element);});
        handleNoteWasCutDelegate = custom_types::MakeDelegate<BeatmapObjectManager::NoteWasCutDelegate*>(
            (function<void(NoteController*, NoteCutInfo*)>)
            [&](NoteController* noteController, NoteCutInfo* noteCutInfo) {
                BadCutInfoCollector(noteController, noteCutInfo);
            }
        );
        
        _scoreController->add_scoringForNoteStartedEvent(scoringForNoteStartedDelegate);
        _scoreController->add_scoringForNoteFinishedEvent(scoringForNoteFinishedDelegate);
        _beatmapObjectManager->add_noteWasCutEvent(handleNoteWasCutDelegate);
    }

    void NoteEventRecorder::Dispose()
    {
        _scoreController->remove_scoringForNoteStartedEvent(scoringForNoteStartedDelegate);
        _scoreController->remove_scoringForNoteFinishedEvent(scoringForNoteFinishedDelegate);
        _beatmapObjectManager->remove_noteWasCutEvent(handleNoteWasCutDelegate);
    }

    void NoteEventRecorder::ScoreController_scoringForNoteStartedEvent(ScoringElement* element)
    {
        if (auto goodCut = il2cpp_utils::try_cast<GoodCutScoringElement>(element).value_or(nullptr))
        {
            _scoringStartInfo.emplace(goodCut, _audioTimeSyncController->songTime);
        }
    }

    void NoteEventRecorder::ScoreController_scoringForNoteFinishedEvent(ScoringElement* element)
    {
        auto noteData = element->noteData;
        NoteID noteID = NoteID(noteData->time, (int)noteData->noteLineLayer, noteData->lineIndex, (int)noteData->colorType, (int)noteData->cutDirection,
                               (int)noteData->gameplayType, (int)noteData->scoringType, noteData->cutDirectionAngleOffset);

        if (auto goodCut = il2cpp_utils::try_cast<GoodCutScoringElement>(element).value_or(nullptr))
        {
            auto cutTime = _scoringStartInfo[goodCut];
            auto noteCutInfo = goodCut->cutScoreBuffer->noteCutInfo;
            _scoringStartInfo.erase(goodCut);
            _noteKeyframes.push_back(NoteEvent(noteID, NoteEventType::GoodCut, VRPosition(noteCutInfo.cutPoint),
                                               VRPosition(noteCutInfo.cutNormal), VRPosition(noteCutInfo.saberDir),
                                               (int)noteCutInfo.saberType, noteCutInfo.directionOK,
                                               noteCutInfo.saberSpeed, noteCutInfo.cutAngle,
                                               noteCutInfo.cutDistanceToCenter, noteCutInfo.cutDirDeviation,
                                               goodCut->cutScoreBuffer->beforeCutSwingRating, goodCut->cutScoreBuffer->afterCutSwingRating,
                                               cutTime, Time::timeScale, _audioTimeSyncController->timeScale,
                                               
                                               noteCutInfo.timeDeviation, VRRotation(noteCutInfo.worldRotation), VRRotation(noteCutInfo.inverseWorldRotation),
                                               VRRotation(noteCutInfo.noteRotation), VRPosition(noteCutInfo.notePosition)));
        }
        else if (auto badCut = il2cpp_utils::try_cast<BadCutScoringElement>(element).value_or(nullptr))
        {
            auto badCutEventType = noteData->colorType == ColorType::None ? NoteEventType::Bomb : NoteEventType::BadCut;
            auto noteCutInfo = _collectedBadCutInfos[badCut->noteData];
            _collectedBadCutInfos.erase(badCut->noteData);
            _noteKeyframes.push_back(NoteEvent(noteID, badCutEventType, VRPosition(noteCutInfo.cutPoint),
                                               VRPosition(noteCutInfo.cutNormal), VRPosition(noteCutInfo.saberDir),
                                               (int)noteCutInfo.saberType, noteCutInfo.directionOK,
                                               noteCutInfo.saberSpeed, noteCutInfo.cutAngle,
                                               noteCutInfo.cutDistanceToCenter, noteCutInfo.cutDirDeviation,
                                               0, 0, _audioTimeSyncController->songTime, Time::timeScale, _audioTimeSyncController->timeScale,
                                               
                                               noteCutInfo.timeDeviation, VRRotation(noteCutInfo.worldRotation), VRRotation(noteCutInfo.inverseWorldRotation),
                                               VRRotation(noteCutInfo.noteRotation), VRPosition(noteCutInfo.notePosition)));
        }
        else if (noteData->colorType != ColorType::None && il2cpp_utils::try_cast<MissScoringElement>(element).value_or(nullptr))
        {
            _noteKeyframes.push_back(NoteEvent(noteID, NoteEventType::Miss, VRPosition(), VRPosition(), VRPosition(), (int)noteData->colorType,
                                               false, 0,
                                               0, 0, 0, 0, 0, _audioTimeSyncController->songTime, Time::timeScale, _audioTimeSyncController->timeScale,
                                               
                                               // I couldn't find where to grab these for misses
                                               0, VRRotation(), VRRotation(),
                                               VRRotation(), VRPosition()));
        }
    }

    void NoteEventRecorder::BadCutInfoCollector(NoteController* noteController, NoteCutInfo* noteCutInfo)
    {
        _collectedBadCutInfos.emplace(noteController->noteData, *noteCutInfo);
    }

    vector<NoteEvent> NoteEventRecorder::Export()
    {
        return _noteKeyframes;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders