#pragma once

#include <GlobalNamespace/AudioManagerSO.hpp>
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/BasicBeatmapObjectManager.hpp>
#include <GlobalNamespace/BeatmapCallbacksController.hpp>
#include <GlobalNamespace/NoteCutSoundEffectManager.hpp>
#include "ReplaySystem/Playback/ComboPlayer.hpp"
#include "ReplaySystem/Playback/EnergyPlayer.hpp"
#include "ReplaySystem/Playback/HeightPlayer.hpp"
#include "ReplaySystem/Playback/MultiplierPlayer.hpp"
#include "ReplaySystem/Playback/NotePlayer.hpp"
#include "ReplaySystem/Playback/PosePlayer.hpp"
#include "ReplaySystem/Playback/ScorePlayer.hpp"
#include <System/IDisposable.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::ReplaySystem::Playback,
        ReplayTimeSyncController,
        System::Object,
        Zenject::ITickable*) {
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioManagerSO>, _audioManagerSO);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::AudioTimeSyncController::InitData*, _audioInitData);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BasicBeatmapObjectManager*, _basicBeatmapObjectManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::NoteCutSoundEffectManager>, _noteCutSoundEffectManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapCallbacksController::InitData*, _callbackInitData);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapCallbacksController*, _beatmapObjectCallbackController);
    DECLARE_INSTANCE_FIELD_PRIVATE(ScoreSaber::ReplaySystem::Playback::ComboPlayer*, _comboPlayer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ScoreSaber::ReplaySystem::Playback::EnergyPlayer*, _energyPlayer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ScoreSaber::ReplaySystem::Playback::HeightPlayer*, _heightPlayer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ScoreSaber::ReplaySystem::Playback::MultiplierPlayer*, _multiplierPlayer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ScoreSaber::ReplaySystem::Playback::NotePlayer*, _notePlayer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ScoreSaber::ReplaySystem::Playback::PosePlayer*, _posePlayer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ScoreSaber::ReplaySystem::Playback::ScorePlayer*, _scorePlayer);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _paused);

    DECLARE_CTOR(ctor,
        GlobalNamespace::AudioTimeSyncController* audioTimeSyncController,
        GlobalNamespace::AudioTimeSyncController::InitData* audioInitData,
        GlobalNamespace::BasicBeatmapObjectManager* basicBeatmapObjectManager,
        GlobalNamespace::NoteCutSoundEffectManager* _noteCutSoundEffectManager,
        GlobalNamespace::BeatmapCallbacksController::InitData* callbackInitData,
        GlobalNamespace::BeatmapCallbacksController* beatmapObjectCallbackController,
        Zenject::DiContainer* container);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
    DECLARE_INSTANCE_METHOD(void, UpdateTimes);
    DECLARE_INSTANCE_METHOD(void, OverrideTime, float time);
    DECLARE_INSTANCE_METHOD(void, OverrideTimeScale, float timeScale);
    DECLARE_INSTANCE_METHOD(void, CancelAllHitSounds);
};