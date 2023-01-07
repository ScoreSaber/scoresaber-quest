#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioManagerSO.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/AudioTimeSyncController_InitData.hpp"
#include "GlobalNamespace/BasicBeatmapObjectManager.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BeatmapCallbacksController_InitData.hpp"
#include "GlobalNamespace/NoteCutSoundEffectManager.hpp"
#include "ReplaySystem/Playback/ComboPlayer.hpp"
#include "ReplaySystem/Playback/EnergyPlayer.hpp"
#include "ReplaySystem/Playback/HeightPlayer.hpp"
#include "ReplaySystem/Playback/MultiplierPlayer.hpp"
#include "ReplaySystem/Playback/NotePlayer.hpp"
#include "ReplaySystem/Playback/PosePlayer.hpp"
#include "ReplaySystem/Playback/ScorePlayer.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/IInitializable.hpp"
#include "Zenject/ITickable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#define INTERFACES                   \
    {                                \
        classof(Zenject::ITickable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Playback, ReplayTimeSyncController, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Playback", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::AudioManagerSO*, _audioManagerSO);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::AudioTimeSyncController::InitData*, _audioInitData);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::BasicBeatmapObjectManager*, _basicBeatmapObjectManager);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::NoteCutSoundEffectManager*, _noteCutSoundEffectManager);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::BeatmapCallbacksController::InitData*, _callbackInitData);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::BeatmapCallbacksController*, _beatmapObjectCallbackController);
                                    DECLARE_PRIVATE_FIELD(ScoreSaber::ReplaySystem::Playback::ComboPlayer*, _comboPlayer);
                                    DECLARE_PRIVATE_FIELD(ScoreSaber::ReplaySystem::Playback::EnergyPlayer*, _energyPlayer);
                                    DECLARE_PRIVATE_FIELD(ScoreSaber::ReplaySystem::Playback::HeightPlayer*, _heightPlayer);
                                    DECLARE_PRIVATE_FIELD(ScoreSaber::ReplaySystem::Playback::MultiplierPlayer*, _multiplierPlayer);
                                    DECLARE_PRIVATE_FIELD(ScoreSaber::ReplaySystem::Playback::NotePlayer*, _notePlayer);
                                    DECLARE_PRIVATE_FIELD(ScoreSaber::ReplaySystem::Playback::PosePlayer*, _posePlayer);
                                    DECLARE_PRIVATE_FIELD(ScoreSaber::ReplaySystem::Playback::ScorePlayer*, _scorePlayer);
                                    DECLARE_PRIVATE_FIELD(bool, _paused);

                                    DECLARE_CTOR(ctor,
                                                 GlobalNamespace::AudioTimeSyncController* audioTimeSyncController,
                                                 GlobalNamespace::AudioTimeSyncController::InitData* audioInitData,
                                                 GlobalNamespace::BasicBeatmapObjectManager* basicBeatmapObjectManager,
                                                 GlobalNamespace::NoteCutSoundEffectManager* _noteCutSoundEffectManager,
                                                 GlobalNamespace::BeatmapCallbacksController::InitData* callbackInitData,
                                                 GlobalNamespace::BeatmapCallbacksController* beatmapObjectCallbackController,
                                                 Zenject::DiContainer* container);

                                    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());
                                    DECLARE_INSTANCE_METHOD(void, UpdateTimes);
                                    DECLARE_INSTANCE_METHOD(void, OverrideTime, float time);
                                    DECLARE_INSTANCE_METHOD(void, OverrideTimeScale, float timeScale);
                                    DECLARE_INSTANCE_METHOD(void, CancelAllHitSounds);)

#undef INTERFACES
