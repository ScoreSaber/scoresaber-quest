#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/NoteWasCutDelegate.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "System/Action_1.hpp"
#include "System/Action_2.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Vector3.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <functional>

using namespace UnityEngine;
using namespace Zenject;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder, SwingFinisher);

// Swing Finisher
namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder
{

    void SwingFinisher::ctor()
    {
    }

    void SwingFinisher::Init(NoteID noteID, ByRef<NoteCutInfo> noteCutInfo, std::function<void(SwingFinisher*)> didFinish, float timeWasCut)
    {
        this->noteId = noteID;
        this->_didFinish = didFinish;
        this->noteCutInfo = noteCutInfo.heldRef;
        this->timeWasCut = timeWasCut;
        this->saberSwingRatingCounter = noteCutInfo.heldRef.swingRatingCounter;
        this->saberSwingRatingCounter->RegisterDidFinishReceiver(reinterpret_cast<ISaberSwingRatingCounterDidFinishReceiver*>(this));
    }

    void SwingFinisher::HandleSaberSwingRatingCounterDidFinish(ISaberSwingRatingCounter* saberSwingRatingCounter)
    {
        _didFinish(this);
        this->saberSwingRatingCounter->UnregisterDidFinishReceiver(reinterpret_cast<ISaberSwingRatingCounterDidFinishReceiver*>(this));
    }

} // namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder

// NoteEventRecorder
namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder
{

    AudioTimeSyncController* _audioTimeSyncController;
    ScoreController* _scoreController;
    MemoryPool_1<SwingFinisher*>* _finisherPool;
    vector<NoteEvent> _noteKeyframes;
    VRPosition _none;

    void SetPool(MemoryPool_1<SwingFinisher*>* finisherPool)
    {
        _finisherPool = finisherPool;
    }

    void LevelStarted(ScoreController* scoreController, AudioTimeSyncController* audioTimeSyncController)
    {
        _none = VRPosition(0, 0, 0);
        _noteKeyframes.clear();
        _audioTimeSyncController = audioTimeSyncController;
        _scoreController = scoreController;

        // noteWasCutCallback
        std::function<void(NoteData*, ByRef<NoteCutInfo>, int)> noteWasCutCallback = [&](NoteData* noteData, ByRef<NoteCutInfo> noteCutInfo, int multiplier) {
            ScoreController_noteWasCutEvent(noteData, noteCutInfo, multiplier);
        };

        auto noteWasCutDelegate = il2cpp_utils::MakeDelegate<NoteWasCutDelegate*>(classof(NoteWasCutDelegate*), noteWasCutCallback);
        _scoreController->add_noteWasCutEvent(noteWasCutDelegate);

        // noteWasMissedCallback
        std::function<void(NoteData*, int)> noteWasMissedCallback = [&](NoteData* noteData, int multiplier) {
            ScoreController_noteWasMissedEvent(noteData, multiplier);
        };

        auto noteWasMissedDelegate = il2cpp_utils::MakeDelegate<System::Action_2<NoteData*, int>*>(classof(System::Action_2<NoteData*, int>*), noteWasMissedCallback);
        _scoreController->add_noteWasMissedEvent(noteWasMissedDelegate);
    }

    void ScoreController_noteWasCutEvent(NoteData* noteData, ByRef<NoteCutInfo> noteCutInfo, int multiplier)
    {

        NoteID noteID = NoteID(noteData->time, noteData->lineIndex, (int)noteData->noteLineLayer, (int)noteData->colorType, (int)noteData->cutDirection);
        if (noteData->colorType == ColorType::None)
        {
            _noteKeyframes.push_back(NoteEvent(noteID, NoteEventType::Bomb, _none, _none, _none, (int)noteCutInfo.heldRef.saberType,
                                               noteCutInfo.heldRef.directionOK, noteCutInfo.heldRef.cutDirDeviation,
                                               0, 0, 0, 0, 0, _audioTimeSyncController->songTime, Time::get_timeScale(), _audioTimeSyncController->timeScale));
            return;
        }

        if (noteCutInfo.heldRef.get_allIsOK())
        {
            auto finisher = _finisherPool->Spawn();
            finisher->Init(noteID, noteCutInfo, SwingFinisher_didFinish, _audioTimeSyncController->songTime);
        }
        else
        {
            _noteKeyframes.push_back(NoteEvent(noteID, NoteEventType::BadCut, _none, _none, _none, (int)noteCutInfo.heldRef.saberType,
                                               noteCutInfo.heldRef.directionOK, noteCutInfo.heldRef.cutDirDeviation,
                                               0, 0, 0, 0, 0, _audioTimeSyncController->songTime, Time::get_timeScale(), _audioTimeSyncController->timeScale));
        }
    }

    void SwingFinisher_didFinish(SwingFinisher* swingFinisher)
    {
        _noteKeyframes.push_back(NoteEvent(swingFinisher->noteId, NoteEventType::GoodCut, VRPosition(swingFinisher->noteCutInfo.cutPoint),
                                           VRPosition(swingFinisher->noteCutInfo.cutNormal), VRPosition(swingFinisher->noteCutInfo.saberDir),
                                           (int)swingFinisher->noteCutInfo.saberType,
                                           swingFinisher->noteCutInfo.directionOK, swingFinisher->noteCutInfo.cutDirDeviation,
                                           0, 0, 0, 0, 0, _audioTimeSyncController->songTime, Time::get_timeScale(), _audioTimeSyncController->timeScale));
        _finisherPool->Despawn(swingFinisher);
    }

    void ScoreController_noteWasMissedEvent(NoteData* noteData, int multiplier)
    {
        if (noteData->colorType == ColorType::None)
        {
            return;
        }

        NoteID noteID = NoteID(noteData->time, noteData->lineIndex, (int)noteData->noteLineLayer, (int)noteData->colorType, (int)noteData->cutDirection);
        _noteKeyframes.push_back(NoteEvent(noteID, NoteEventType::Miss, _none, _none, _none, (int)noteData->colorType,
                                           false, 0,
                                           0, 0, 0, 0, 0, _audioTimeSyncController->songTime, Time::get_timeScale(), _audioTimeSyncController->timeScale));
    }

    vector<NoteEvent> Export()
    {
        return _noteKeyframes;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder