#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/NoteWasCutDelegate.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "System/Action_1.hpp"
#include "System/Action_3.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder, SwingFinisher);

namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder
{
    void SwingFinisher::Init(NoteID noteID, NoteCutInfo noteCutInfo, std::function<void(SwingFinisher*)> didFinish, float timeWasCut)
    {
        this->noteId = noteID;
        this->_didFinish = didFinish;
        this->noteCutInfo = noteCutInfo;
        this->timeWasCut = timeWasCut;
        this->saberSwingRatingCounter = noteCutInfo.swingRatingCounter;
        this->saberSwingRatingCounter->RegisterDidChangeReceiver(reinterpret_cast<ISaberSwingRatingCounterDidChangeReceiver*>(this));
    }

    void SwingFinisher::HandleSaberSwingRatingCounterDidFinish(ISaberSwingRatingCounter* saberSwingRatingCounter)
    {
        _didFinish(this);
        this->saberSwingRatingCounter->UnregisterDidChangeReceiver(reinterpret_cast<ISaberSwingRatingCounterDidChangeReceiver*>(this));
    }
} // namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder

namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder
{

    AudioTimeSyncController* _audioTimeSyncController;
    ScoreController* _scoreController;
    vector<NoteEvent> _noteKeyframes;
    VRPosition _none;
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

        // auto noteWasMissedDelegate = il2cpp_utils::MakeDelegate<System::Action_2<NoteData*, int>*>(classof(System::Action_2<NoteData*, int>*), noteWasMissedCallback);
        // _scoreController->add_noteWasMissedEvent(noteWasMissedDelegate);
    }

    void ScoreController_noteWasCutEvent(NoteData* noteData, ByRef<NoteCutInfo> noteCutInfo, int multiplier)
    {

        NoteID noteID = NoteID(noteData->time, noteData->lineIndex, (int)noteData->noteLineLayer, (int)noteData->colorType, (int)noteData->cutDirection);
        if (noteData->colorType == ColorType::None)
        {
            _noteKeyframes.push_back(NoteEvent(noteID, NoteEventType::Bomb, _none, _none, _none, (int)noteCutInfo.heldRef.saberType,
                                               (int)noteCutInfo.heldRef.directionOK, (int)noteCutInfo.heldRef.cutDirDeviation,
                                               0, 0, 0, 0, 0, _audioTimeSyncController->songTime, Time::get_timeScale(), _audioTimeSyncController->timeScale));
            return;
        }

        if (noteCutInfo.heldRef.get_allIsOK())
        {
        }
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

    // void PlayerHeightDetector_playerHeightDidChangeEvent(float newHeight)
    // {
    //     _heightKeyframes.push_back(HeightEvent(newHeight, _audioTimeSyncController->songTime));
    // }

    vector<NoteEvent> Export()
    {
        return _noteKeyframes;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder