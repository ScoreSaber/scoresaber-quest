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
        _bombNotePool = basicBeatmapObjectManager->bombNotePoolContainer;
        _sortedNoteEvents = ReplayLoader::LoadedReplay->noteKeyframes;
        std::sort(_sortedNoteEvents.begin(), _sortedNoteEvents.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.Time < rhs.Time;
        });
    }
    void NotePlayer::Tick()
    {
        if (_lastIndex >= _sortedNoteEvents.size())
        {
            return;
        }
        while (_audioTimeSyncController->songTime >= _sortedNoteEvents[_lastIndex].Time)
        {
            NoteEvent activeEvent = _sortedNoteEvents[_lastIndex++];
            ProcessEvent(activeEvent);
            if (_lastIndex >= _sortedNoteEvents.size())
            {
                break;
            }
        }
    }
    bool NotePlayer::ProcessEvent(Data::Private::NoteEvent activeEvent)
    {
        bool foundNote = false;
        if (activeEvent.EventType == NoteEventType::GoodCut || activeEvent.EventType == NoteEventType::BadCut)
        {
            auto items = _gameNotePool->get_activeItems();
            for (int i = 0; i < items->get_Count(); i++)
            {
                auto noteController = items->get_Item(i);
                if (HandleEvent(activeEvent, noteController))
                {
                    foundNote = true;
                    break;
                }
            }
        }
        else if (activeEvent.EventType == NoteEventType::Bomb)
        {
            auto items = _bombNotePool->get_activeItems();
            for (int i = 0; i < items->get_Count(); i++)
            {
                auto bombController = items->get_Item(i);
                if (HandleEvent(activeEvent, bombController))
                {
                    foundNote = true;
                    break;
                }
            }
        }
        return foundNote;
    }
    bool NotePlayer::HandleEvent(Data::Private::NoteEvent activeEvent, GlobalNamespace::NoteController* noteController)
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
    bool NotePlayer::DoesNoteMatchID(Data::Private::NoteID id, GlobalNamespace::NoteData* noteData)
    {
        return Mathf::Approximately(id.Time, noteData->time) && id.LineIndex == noteData->lineIndex && id.LineLayer == noteData->noteLineLayer && id.ColorType == noteData->colorType && id.CutDirection == noteData->cutDirection;
    }
    void NotePlayer::TimeUpdate(float newTime)
    {
        for (int c = 0; c < _sortedNoteEvents.size(); c++)
        {
            if (_sortedNoteEvents[c].Time >= newTime)
            {
                _lastIndex = c;
                Tick();
                break;
            }
        }
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
            auto element = il2cpp_utils::try_cast<GlobalNamespace::ScoringElement>(scoringElement).value_or(nullptr);
            element->isFinished = true;
        }
    }
} // namespace ScoreSaber::ReplaySystem::Playback