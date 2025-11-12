#pragma once

#include <GlobalNamespace/GameNoteController.hpp>
#include <System/IDisposable.hpp>
#include <Tweening/TimeTweeningManager.hpp>
#include <Tweening/Tween.hpp>
#include <UnityEngine/Quaternion.hpp>
#include <UnityEngine/Shader.hpp>
#include <UnityEngine/Vector3.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/ITickable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::ReplaySystem::UI,
        SpectateAreaController,
        System::Object,
        Zenject::ITickable*,
        System::IDisposable*) {
    DECLARE_INSTANCE_FIELD_DEFAULT(int, _colorID, UnityEngine::Shader::PropertyToID("Color"));
    DECLARE_INSTANCE_FIELD(UnityW<Tweening::TimeTweeningManager>, _timeTweeningManager);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::GameNoteController::Pool*, _gameNoteControllerPool);

    DECLARE_INSTANCE_FIELD(UnityW<GlobalNamespace::GameNoteController>, _activeNote);
    DECLARE_INSTANCE_FIELD(UnityEngine::Quaternion, _initialQuaternion);
    DECLARE_INSTANCE_FIELD(Tweening::Tween*, _movementTween);
    DECLARE_INSTANCE_FIELD(Tweening::Tween*, _statusTween);
    DECLARE_INSTANCE_FIELD(bool, _despawned);
    DECLARE_CTOR(ctor, Zenject::DiContainer* diContainer, Tweening::TimeTweeningManager* timeTweeningManager);

    DECLARE_INSTANCE_METHOD(void, AnimateTo, StringW poseID);
    DECLARE_INSTANCE_METHOD(void, JumpToCallback, StringW poseID);
    DECLARE_INSTANCE_METHOD(void, Dismiss);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);

    DECLARE_INSTANCE_METHOD(void, UpdateNoteScale, UnityEngine::Vector3 scale);
    DECLARE_INSTANCE_METHOD(void, DespawnActiveNote);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    std::optional<UnityEngine::Vector3> TryGetPose(StringW poseID);
public:
    std::function<void(UnityEngine::Vector3, UnityEngine::Quaternion)> DidUpdatePlayerSpectatorPose;
};