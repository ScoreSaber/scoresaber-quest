
#include "ReplaySystem/Playback/ReplayTimeSyncController.hpp"
#include "UnityEngine/Mathf.hpp"

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
                                        GlobalNamespace::BeatmapObjectSpawnController* beatmapObjectCallbackController,
                                        ScoreSaber::ReplaySystem::Playback::PosePlayer* posePlayer,
                                        ScoreSaber::ReplaySystem::Playback::NotePlayer* notePlayer)
    {
        _audioTimeSyncController = audioTimeSyncController;
        _audioInitData = audioInitData;
        _basicBeatmapObjectManager = basicBeatmapObjectManager;
        _noteCutSoundEffectManager = _noteCutSoundEffectManager;
        _callbackInitData = callbackInitData;
        _beatmapObjectCallbackController = beatmapObjectCallbackController;

        _audioManagerSO = _noteCutSoundEffectManager->audioManager;

        _posePlayer = posePlayer;
        _notePlayer = notePlayer;
    }

    void ReplayTimeSyncController::Tick()
    {
        // Potential input?
    }

    void ReplayTimeSyncController::UpdateTimes()
    {
        _posePlayer->TimeUpdate(_audioTimeSyncController->songTime);
        _notePlayer->TimeUpdate(_audioTimeSyncController->songTime);
    }

    void ReplayTimeSyncController::OverrideTime(float time)
    {
        if (std::abs(time - _audioTimeSyncController->songTime) <= 0.25f)
        {
            return;
        }
    }

    void ReplayTimeSyncController::OverrideTimeScale(float timeScale)
    {
    }

    void ReplayTimeSyncController::CancelAllHitSounds()
    {
    }

} // namespace ScoreSaber::ReplaySystem::Playback