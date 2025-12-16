#include "ReplaySystem/Playback/ReplayTimeSyncController.hpp"
#include <GlobalNamespace/CallbacksInTime.hpp>
#include <GlobalNamespace/BurstSliderGameNoteController.hpp>
#include <GlobalNamespace/ObstacleController.hpp>
#include <System/Collections/Generic/Dictionary_2.hpp>
#include <System/Collections/Generic/LinkedListNode_1.hpp>
#include <UnityEngine/AudioSource.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/Time.hpp>
#include <GlobalNamespace/NoteCutSoundEffect.hpp>
#include <GlobalNamespace/NoteCutSoundEffectManager.hpp>
#include "logging.hpp"

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, ReplayTimeSyncController);

namespace ScoreSaber::ReplaySystem::Playback
{
    void ReplayTimeSyncController::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController,
                                        GlobalNamespace::AudioTimeSyncController::InitData* audioInitData,
                                        GlobalNamespace::BasicBeatmapObjectManager* basicBeatmapObjectManager,
                                        GlobalNamespace::NoteCutSoundEffectManager* _noteCutSoundEffectManager,
                                        GlobalNamespace::BeatmapCallbacksController::InitData* callbackInitData,
                                        GlobalNamespace::BeatmapCallbacksController* beatmapObjectCallbackController,
                                        Zenject::DiContainer* container)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _audioInitData = audioInitData;
        _basicBeatmapObjectManager = basicBeatmapObjectManager;
        _noteCutSoundEffectManager = _noteCutSoundEffectManager;
        _callbackInitData = callbackInitData;
        _beatmapObjectCallbackController = beatmapObjectCallbackController;

        _audioManagerSO = _noteCutSoundEffectManager->_audioManager;

        _comboPlayer = container->Resolve<ComboPlayer*>();
        _energyPlayer = container->Resolve<EnergyPlayer*>();
        _heightPlayer = container->TryResolve<HeightPlayer*>();
        _multiplierPlayer = container->Resolve<MultiplierPlayer*>();
        _notePlayer = container->Resolve<NotePlayer*>();
        _posePlayer = container->Resolve<PosePlayer*>();
        _scorePlayer = container->Resolve<ScorePlayer*>();
    }

    void ReplayTimeSyncController::Tick()
    {
        // Potential input?
    }

    void ReplayTimeSyncController::Initialize()
    {
        if(!_noteCutSoundEffectManager){
            INFO("Finding NoteCutSoundEffectManager via Resources");
            _noteCutSoundEffectManager = UnityEngine::Object::FindObjectsOfType<GlobalNamespace::NoteCutSoundEffectManager*>()->FirstOrDefault(); // weird that this fixes it, why isnt it being injected?
        }
    }

    void ReplayTimeSyncController::UpdateTimes()
    {
        if(_heightPlayer){
            _heightPlayer->TimeUpdate(_audioTimeSyncController->songTime);
        }
        _energyPlayer->TimeUpdate(_audioTimeSyncController->songTime); // needs to be run before the ScorePlayer
        _comboPlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _multiplierPlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _scorePlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _posePlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _notePlayer->TimeUpdate(_audioTimeSyncController->songTime);
    }

    void ReplayTimeSyncController::OverrideTime(float time)
    {
        if (std::abs(time - _audioTimeSyncController->songTime) <= 0.25f)
        {
            return;
        }

        CancelAllHitSounds();
        auto previousState = _audioTimeSyncController->state;

        _audioTimeSyncController->Pause();

        ListW<GlobalNamespace::IBeatmapObjectController*> allBmObj = _basicBeatmapObjectManager->_allBeatmapObjects;

        for (auto bmObj : allBmObj) {
            bmObj->Pause(false);
            bmObj->Hide(false);
            ((UnityEngine::Component*) bmObj)->gameObject->active = true;
            bmObj->Dissolve(0.0f);
        }
        
        _callbackInitData->startFilterTime = time;
        auto callbacks = _beatmapObjectCallbackController->_callbacksInTimes;

        auto itr = callbacks->GetEnumerator();
        while (itr.MoveNext())
        {
            auto callback = itr._current.value;
            if (callback->lastProcessedNode != nullptr && callback->lastProcessedNode->item->time > time)
            {
                callback->lastProcessedNode = nullptr;
            }
        }

        _beatmapObjectCallbackController->_prevSongTime = time - 0.01;
        _beatmapObjectCallbackController->_songTime = time;
        _beatmapObjectCallbackController->_startFilterTime = time;
        _audioTimeSyncController->SeekTo(time / _audioTimeSyncController->timeScale);
        _audioTimeSyncController->_songTime = time;
        _audioTimeSyncController->Update();
        
        if (previousState == GlobalNamespace::AudioTimeSyncController::State::Playing)
        {
            _audioTimeSyncController->Resume();
        }
        UpdateTimes();
    }

    void ReplayTimeSyncController::OverrideTimeScale(float newScale)
    {
        CancelAllHitSounds();
        _audioTimeSyncController->_audioSource->pitch = newScale;
        _audioTimeSyncController->_timeScale = newScale;
        _audioTimeSyncController->_audioStartTimeOffsetSinceStart = (Time::get_timeSinceLevelLoad() * _audioTimeSyncController->timeScale) - (_audioTimeSyncController->songTime + _audioInitData->songTimeOffset);

        _audioManagerSO->musicPitch = 1.0f / newScale;
        _audioTimeSyncController->Update();
    }

    void ReplayTimeSyncController::CancelAllHitSounds()
    {
        auto noteCutPool = _noteCutSoundEffectManager->_noteCutSoundEffectPoolContainer;
        auto noteCutPoolItems = noteCutPool->activeItems;
        for (int i = 0; i < noteCutPoolItems->Count; i++)
        {
            auto effect = noteCutPoolItems->get_Item(i);
            if (effect->isActiveAndEnabled)
            {
                effect->StopPlayingAndFinish();
            }
        }
        _noteCutSoundEffectManager->_prevNoteATime = -1.0f;
        _noteCutSoundEffectManager->_prevNoteBTime = -1.0f;
    }
} // namespace ScoreSaber::ReplaySystem::Playback