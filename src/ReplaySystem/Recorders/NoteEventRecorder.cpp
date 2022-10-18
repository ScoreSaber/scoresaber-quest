#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "System/Action_1.hpp"
#include "System/Action_2.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Vector3.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <custom-types/shared/delegate.hpp>
#include <functional>
#include <map>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;
using namespace System::Collections::Generic;
namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder
{

    AudioTimeSyncController* _audioTimeSyncController;
    ScoreController* _scoreController;
    vector<NoteEvent> _noteKeyframes;
    VRPosition _none;
    std::map<NoteData*, NoteCutInfo> _collectedBadCutInfos;
    std::map<GoodCutScoringElement*, float> _scoringStartInfo;

    void LevelStarted(ScoreController* scoreController, AudioTimeSyncController* audioTimeSyncController)
    {
        _none = VRPosition(0, 0, 0);
        _noteKeyframes.clear();
        _audioTimeSyncController = audioTimeSyncController;
        _scoreController = scoreController;
        _collectedBadCutInfos.clear();
        _scoringStartInfo.clear();

        // ScoreController_scoringForNoteStartedEvent
        std::function<void(ScoringElement*)>
            scoringForNoteStartedCallback = [&](ScoringElement* element) {
                ScoreController_scoringForNoteStartedEvent(element);
            };

        auto scoringForNoteStartedDelegate = custom_types::MakeDelegate<System::Action_1<ScoringElement*>*>(classof(System::Action_1<ScoringElement*>*), scoringForNoteStartedCallback);
        _scoreController->add_scoringForNoteStartedEvent(scoringForNoteStartedDelegate);

        // ScoreController_scoringForNoteFinishedEvent
        std::function<void(ScoringElement*)> scoringForNoteFinishedCallback = [&](ScoringElement* element) {
            ScoreController_scoringForNoteFinishedEvent(element);
        };

        auto scoringForNoteFinishedDelegate = custom_types::MakeDelegate<System::Action_1<ScoringElement*>*>(classof(System::Action_1<ScoringElement*>*), scoringForNoteFinishedCallback);
        _scoreController->add_scoringForNoteFinishedEvent(scoringForNoteFinishedDelegate);
    }

    void ScoreController_scoringForNoteStartedEvent(ScoringElement* element)
    {
        if (auto goodCut = il2cpp_utils::try_cast<GoodCutScoringElement>(element).value_or(nullptr))
        {
            _scoringStartInfo.emplace(goodCut, _audioTimeSyncController->songTime);
        }
    }

    void ScoreController_scoringForNoteFinishedEvent(ScoringElement* element)
    {
        auto noteData = element->noteData;
        NoteID noteID = NoteID(noteData->time, (int)noteData->noteLineLayer, noteData->lineIndex, (int)noteData->colorType, (int)noteData->cutDirection);

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
                                               goodCut->cutScoreBuffer->get_beforeCutSwingRating(), goodCut->cutScoreBuffer->get_afterCutSwingRating(),
                                               cutTime, Time::get_timeScale(), _audioTimeSyncController->timeScale));
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
                                               0, 0, _audioTimeSyncController->songTime, Time::get_timeScale(), _audioTimeSyncController->timeScale));
        }
        else if (noteData->colorType != ColorType::None && il2cpp_utils::try_cast<MissScoringElement>(element).value_or(nullptr))
        {
            _noteKeyframes.push_back(NoteEvent(noteID, NoteEventType::Miss, _none, _none, _none, (int)noteData->colorType,
                                               false, 0,
                                               0, 0, 0, 0, 0, _audioTimeSyncController->songTime, Time::get_timeScale(), _audioTimeSyncController->timeScale));
        }
    }

    void BadCutInfoCollector(NoteController* noteController, const NoteCutInfo& noteCutInfo)
    {
        _collectedBadCutInfos.emplace(noteController->noteData, noteCutInfo);
    }

    vector<NoteEvent>
    Export()
    {
        return _noteKeyframes;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder