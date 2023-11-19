#include "ReplaySystem/Playback/NotePlayer.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/ISaberMovementData.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberMovementData.hpp"
#include "GlobalNamespace/SaberSwingRatingCounter.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/SaberTypeObject.hpp"
#include "GlobalNamespace/ScoringElement.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Transform.hpp"
#include "logging.hpp"
#include <algorithm>

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, NotePlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void NotePlayer::ctor(GlobalNamespace::SaberManager* saberManager, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::BasicBeatmapObjectManager* basicBeatmapObjectManager)
    {
        ReplayLoader::NotePlayerInstance = this;
        _saberManager = saberManager;
        _audioTimeSyncController = audioTimeSyncController;
        _basicBeatmapObjectManager = basicBeatmapObjectManager;
        _gameNotePool = basicBeatmapObjectManager->basicGameNotePoolContainer;
        _burstSliderHeadNotePool = basicBeatmapObjectManager->burstSliderHeadGameNotePoolContainer;
        _burstSliderNotePool = basicBeatmapObjectManager->burstSliderGameNotePoolContainer;
        _bombNotePool = basicBeatmapObjectManager->bombNotePoolContainer;
        _sortedNoteEvents = ReplayLoader::LoadedReplay->noteKeyframes;
        std::sort(_sortedNoteEvents.begin(), _sortedNoteEvents.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.Time < rhs.Time;
        });
    }
    void NotePlayer::Tick()
    {
        while (_nextIndex < _sortedNoteEvents.size() && _audioTimeSyncController->songTime >= _sortedNoteEvents[_nextIndex].Time) {
            NoteEvent activeEvent = _sortedNoteEvents[_nextIndex++];
            ProcessEvent(activeEvent);
        }
    }
    void NotePlayer::ProcessEvent(Data::Private::NoteEvent &activeEvent)
    {
        bool foundNote = false;
        if (activeEvent.EventType == NoteEventType::GoodCut || activeEvent.EventType == NoteEventType::BadCut) {
            auto itemsNotes = _gameNotePool->get_activeItems();
            for (int i = 0; i < itemsNotes->get_Count(); i++) {
                auto noteController = itemsNotes->get_Item(i);
                if (HandleEvent(activeEvent, noteController)) {
                    return;
                }
            }
            auto itemsSliderHeads = _burstSliderHeadNotePool->get_activeItems();
            for (int i = 0; i < itemsSliderHeads->get_Count(); i++) {
                auto noteController = itemsSliderHeads->get_Item(i);
                if (HandleEvent(activeEvent, noteController)) {
                    return;
                }
            }
            auto itemsSliderNotes = _burstSliderNotePool->get_activeItems();
            for (int i = 0; i < itemsSliderNotes->get_Count(); i++) {
                auto noteController = itemsSliderNotes->get_Item(i);
                if (HandleEvent(activeEvent, noteController)) {
                    return;
                }
            }
        } else if (activeEvent.EventType == NoteEventType::Bomb) {
            auto items = _bombNotePool->get_activeItems();
            for (int i = 0; i < items->get_Count(); i++) {
                auto bombController = items->get_Item(i);
                if (HandleEvent(activeEvent, bombController)) {
                    return;
                }
            }
        }
    }
    bool NotePlayer::HandleEvent(Data::Private::NoteEvent &activeEvent, GlobalNamespace::NoteController* noteController)
    {
        if (DoesNoteMatchID(activeEvent.TheNoteID, noteController->noteData))
        {
            GlobalNamespace::Saber* correctSaber = noteController->noteData->colorType == GlobalNamespace::ColorType::ColorA ? _saberManager->leftSaber : _saberManager->rightSaber;
            auto noteTransform = noteController->noteTransform;

            auto noteCutInfo = GlobalNamespace::NoteCutInfo(noteController->noteData,
                                                            activeEvent.SaberSpeed > 2.0f,
                                                            activeEvent.DirectionOK,
                                                            activeEvent.SaberType == correctSaber->saberType->saberType.value,
                                                            false,
                                                            activeEvent.SaberSpeed,
                                                            VRVector3(activeEvent.SaberDirection),
                                                            noteController->noteData->colorType == GlobalNamespace::ColorType::ColorA ? GlobalNamespace::SaberType::SaberA : GlobalNamespace::SaberType::SaberB,
                                                            noteController->noteData->time - activeEvent.Time,
                                                            activeEvent.CutDirectionDeviation,
                                                            VRVector3(activeEvent.CutPoint),
                                                            VRVector3(activeEvent.CutNormal),
                                                            activeEvent.CutAngle,
                                                            activeEvent.CutDistanceToCenter,
                                                            noteController->get_worldRotation(),
                                                            noteController->get_inverseWorldRotation(),
                                                            noteTransform->get_rotation(),
                                                            noteTransform->get_position(),
                                                            il2cpp_utils::try_cast<GlobalNamespace::ISaberMovementData>(correctSaber->movementData).value_or(nullptr));
            _recognizedNoteCutInfos.emplace_back(noteCutInfo, activeEvent);
            il2cpp_utils::RunMethodUnsafe(noteController, "SendNoteWasCutEvent", byref(noteCutInfo));
            return true;
        }
        return false;
    }
    bool NotePlayer::DoesNoteMatchID(Data::Private::NoteID &id, GlobalNamespace::NoteData* noteData)
    {
        if (!Mathf::Approximately(id.Time, noteData->time) ||
            id.LineIndex != noteData->lineIndex ||
            id.LineLayer != noteData->noteLineLayer ||
            id.ColorType != noteData->colorType ||
            id.CutDirection != noteData->cutDirection) {
            return false;
        }

        if (id.GameplayType.has_value() && id.GameplayType.value() != noteData->gameplayType.value) {
            return false;
        }
        
        if (id.ScoringType.has_value() && id.ScoringType.value() != noteData->scoringType.value) {
            return false;
        }
        
        if (id.CutDirectionAngleOffset.has_value() && !Mathf::Approximately(id.CutDirectionAngleOffset.value(), noteData->cutDirectionAngleOffset)) {
            return false;
        }
        return true;
    }
    void NotePlayer::TimeUpdate(float newTime)
    {
        for (int c = 0; c < _sortedNoteEvents.size(); c++)
        {
            if (_sortedNoteEvents[c].Time > newTime)
            {
                _nextIndex = c;
                return;
            }
        }
        _nextIndex = _sortedNoteEvents.size();
    }
    static bool operator==(const GlobalNamespace::NoteCutInfo& lhs, const GlobalNamespace::NoteCutInfo& rhs)
    {
        return lhs.noteData == rhs.noteData;
    }
    void NotePlayer::ForceCompleteGoodScoringElements(GlobalNamespace::GoodCutScoringElement* scoringElement, GlobalNamespace::NoteCutInfo noteCutInfo, GlobalNamespace::CutScoreBuffer* cutScoreBuffer)
    {
        NoteEvent activeEvent;
        for (int i = 0; i < _recognizedNoteCutInfos.size(); i++)
        {
            if (_recognizedNoteCutInfos[i].info == noteCutInfo)
            {
                activeEvent = _recognizedNoteCutInfos[i].event;
                _recognizedNoteCutInfos.erase(_recognizedNoteCutInfos.begin() + i);
                break;
            }
        }

        if (!scoringElement->isFinished)
        {
            auto ratingCounter = cutScoreBuffer->saberSwingRatingCounter;

            ratingCounter->afterCutRating = activeEvent.AfterCutRating;
            ratingCounter->beforeCutRating = activeEvent.BeforeCutRating;

            cutScoreBuffer->HandleSaberSwingRatingCounterDidFinish(il2cpp_utils::try_cast<GlobalNamespace::ISaberSwingRatingCounter>(ratingCounter).value_or(nullptr));
            scoringElement->isFinished = true;
        }
    }
} // namespace ScoreSaber::ReplaySystem::Playback