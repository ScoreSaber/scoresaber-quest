#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController_InitData.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IGameEnergyCounter.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/IInitializable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

using namespace GlobalNamespace;

#define INTERFACES                                                        \
    {                                                                     \
        classof(System::IDisposable*), classof(Zenject::IInitializable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, MetadataRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_PRIVATE_FIELD(BeatmapObjectSpawnController::InitData*, _beatmapObjectSpawnControllerInitData);
                                    DECLARE_PRIVATE_FIELD(GameplayCoreSceneSetupData*, _gameplayCoreSceneSetupData);
                                    DECLARE_PRIVATE_FIELD(MainSettingsModelSO*, _mainSettingsModelSO);
                                    DECLARE_PRIVATE_FIELD(IGameEnergyCounter*, _gameEnergyCounter);
                                    DECLARE_CTOR(ctor, AudioTimeSyncController* audioTimeSyncController, GameplayCoreSceneSetupData* gameplayCoreSceneSetupData, BeatmapObjectSpawnController::InitData* beatmapObjectSpawnControllerInitData, IGameEnergyCounter* gameEnergyCounter);
                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    DECLARE_INSTANCE_METHOD(void, GameEnergyCounter_gameEnergyDidReach0Event);
                                    float _failTime;
                                    System::Action *gameEnergyDidReach0Delegate;
                                    public:
                                    std::shared_ptr<Data::Private::Metadata> Export();
                                    )

#undef INTERFACES