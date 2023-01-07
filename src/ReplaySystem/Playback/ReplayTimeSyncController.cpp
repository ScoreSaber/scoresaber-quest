
#include "ReplaySystem/Playback/ReplayTimeSyncController.hpp"
#include "GlobalNamespace/CallbacksInTime.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Time.hpp"

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
        _audioTimeSyncController = audioTimeSyncController;
        _audioInitData = audioInitData;
        _basicBeatmapObjectManager = basicBeatmapObjectManager;
        _noteCutSoundEffectManager = _noteCutSoundEffectManager;
        _callbackInitData = callbackInitData;
        _beatmapObjectCallbackController = beatmapObjectCallbackController;

        _audioManagerSO = _noteCutSoundEffectManager->audioManager;

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

    void ReplayTimeSyncController::UpdateTimes()
    {
        _comboPlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _energyPlayer->TimeUpdate(_audioTimeSyncController->songTime);
        if(_heightPlayer)
            _heightPlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _multiplierPlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _notePlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _posePlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _scorePlayer->TimeUpdate(_audioTimeSyncController->songTime);
    }

    void ReplayTimeSyncController::OverrideTime(float time)
    {
        if (std::abs(time - _audioTimeSyncController->songTime) <= 0.25f)
        {
            return;
        }
        // TODO: Cancel hitsounds

        CancelAllHitSounds();

        auto gameNotePool = _basicBeatmapObjectManager->basicGameNotePoolContainer;
        auto gameNotePoolItems = gameNotePool->get_activeItems();
        for (int i = 0; i < gameNotePoolItems->get_Count(); i++)
        {
            auto item = gameNotePoolItems->get_Item(i);
            item->Hide(false);
            item->Pause(false);
            item->set_enabled(true);
            item->get_gameObject()->SetActive(true);
            item->Dissolve(0.0f);
        }
        auto bombNotePool = _basicBeatmapObjectManager->bombNotePoolContainer;
        auto bombNotePoolItems = bombNotePool->get_activeItems();
        for (int i = 0; i < bombNotePoolItems->get_Count(); i++)
        {
            auto item = bombNotePoolItems->get_Item(i);
            item->Hide(false);
            item->Pause(false);
            item->set_enabled(true);
            item->get_gameObject()->SetActive(true);
            item->Dissolve(0.0f);
        }

        for (int i = 0; i < _basicBeatmapObjectManager->get_activeObstacleControllers()->get_Count(); i++)
        {
            auto item = _basicBeatmapObjectManager->get_activeObstacleControllers()->get_Item(i);
            item->Hide(false);
            item->Pause(false);
            item->set_enabled(true);
            item->get_gameObject()->SetActive(true);
            item->Dissolve(0.0f);
        }
        auto previousState = _audioTimeSyncController->state;
        _audioTimeSyncController->Pause();
        _audioTimeSyncController->SeekTo(time / _audioTimeSyncController->timeScale);
        if (previousState == GlobalNamespace::AudioTimeSyncController::State::Playing)
        {
            _audioTimeSyncController->Resume();
        }
        _callbackInitData->startFilterTime = time;
        _beatmapObjectCallbackController->startFilterTime = time;
        auto callbacks = _beatmapObjectCallbackController->callbacksInTimes;

        auto itr = callbacks->GetEnumerator();
        while (itr.MoveNext())
        {
            auto callback = itr.current.value;
            if (callback->lastProcessedNode != nullptr && callback->lastProcessedNode->item->time > time)
            {
                callback->lastProcessedNode = nullptr;
            }
        }

        _audioTimeSyncController->songTime = time;
        _audioTimeSyncController->Update();
        UpdateTimes();
    }

    void ReplayTimeSyncController::OverrideTimeScale(float newScale)
    {
        CancelAllHitSounds();
        _audioTimeSyncController->audioSource->set_pitch(newScale);
        _audioTimeSyncController->timeScale = newScale;
        _audioTimeSyncController->audioStartTimeOffsetSinceStart = (Time::get_timeSinceLevelLoad() * _audioTimeSyncController->timeScale) - (_audioTimeSyncController->songTime + _audioInitData->songTimeOffset);

        _audioManagerSO->set_musicPitch(1.0f / newScale);
        _audioTimeSyncController->Update();
    }

    void ReplayTimeSyncController::CancelAllHitSounds()
    {
        // auto noteCutPool = _noteCutSoundEffectManager->noteCutSoundEffectPoolContainer;
        // auto noteCutPoolItems = noteCutPool->get_activeItems();
        // for (int i = 0; i < noteCutPoolItems->get_Count(); i++)
        // {
        //     auto effect = noteCutPoolItems->get_Item(i);
        //     if (effect->get_isActiveAndEnabled())
        //     {
        //         effect->StopPlayingAndFinish();
        //     }
        // }
        // _noteCutSoundEffectManager->prevNoteATime = -1.0f;
        // _noteCutSoundEffectManager->prevNoteBTime = -1.0f;
    }
} // namespace ScoreSaber::ReplaySystem::Playback