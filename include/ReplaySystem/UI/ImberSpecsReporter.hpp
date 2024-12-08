#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/MainCamera.hpp>
#include "ReplaySystem/UI/Components/AmeBar.hpp"
#include "ReplaySystem/UI/Components/AmeNode.hpp"
#include <Zenject/DiContainer.hpp>

#include <GlobalNamespace/SaberManager.hpp>
#include "ReplaySystem/Playback/PosePlayer.hpp"
#include <System/IDisposable.hpp>
#include <Zenject/IInitializable.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

#define INTERFACES                                                         \
    {                                                                      \
        classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::UI, ImberSpecsReporter, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::UI", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD(ScoreSaber::ReplaySystem::Playback::PosePlayer*, _posePlayer);
                                    DECLARE_INSTANCE_FIELD(GlobalNamespace::SaberManager*, _saberManager);
                                    DECLARE_CTOR(ctor, ScoreSaber::ReplaySystem::Playback::PosePlayer* posePlayer, GlobalNamespace::SaberManager* _saberManager);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
                                    public
                                    : void PosePlayer_DidUpdatePose(ScoreSaber::Data::Private::VRPoseGroup pose);
                                    std::function<void(int, float, float)>
                                        DidReport;

)

#undef INTERFACES
