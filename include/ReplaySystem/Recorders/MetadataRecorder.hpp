#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "Utils/DelegateUtils.hpp"
#include <BeatSaber/GameSettings/MainSettingsHandler.hpp>
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/BeatmapObjectSpawnController.hpp>
#include <GlobalNamespace/GameplayCoreSceneSetupData.hpp>
#include <GlobalNamespace/GameEnergyCounter.hpp>
#include <System/IDisposable.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include "Utils/DelegateUtils.hpp"

using namespace GlobalNamespace;

#define INTERFACES                                                        \
    {                                                                     \
        classof(System::IDisposable*), classof(Zenject::IInitializable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, MetadataRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(BeatmapObjectSpawnController::InitData*, _beatmapObjectSpawnControllerInitData);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GameplayCoreSceneSetupData*, _gameplayCoreSceneSetupData);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(BeatSaber::GameSettings::MainSettingsHandler*, _mainSettingsHandler);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GameEnergyCounter*, _gameEnergyCounter);
                                    DECLARE_CTOR(ctor, AudioTimeSyncController* audioTimeSyncController, GameplayCoreSceneSetupData* gameplayCoreSceneSetupData, BeatmapObjectSpawnController::InitData* beatmapObjectSpawnControllerInitData, GameEnergyCounter* gameEnergyCounter, BeatSaber::GameSettings::MainSettingsHandler* mainSettingsHandler);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
                                    DECLARE_INSTANCE_METHOD(void, GameEnergyCounter_gameEnergyDidReach0Event);
                                    float _failTime;
                                    DelegateUtils::DelegateW<System::Action> gameEnergyDidReach0Delegate;
                                    public:
                                    std::shared_ptr<Data::Private::Metadata> Export();
                                    )

#undef INTERFACES