#include "ReplaySystem/UI/SpectateAreaController.hpp"

#include "Data/Private/Settings.hpp"

#include <GlobalNamespace/ColorExtensions.hpp>
#include <GlobalNamespace/ColorNoteVisuals.hpp>
#include <GlobalNamespace/MaterialPropertyBlockController.hpp>
#include <GlobalNamespace/Saber.hpp>
#include <GlobalNamespace/SaberMovementData.hpp>
#include <System/Action.hpp>
#include <System/Action_1.hpp>
#include "Tweening/Vector3Tween.hpp"
#include <UnityEngine/MaterialPropertyBlock.hpp>
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/RectTransformUtility.hpp>
#include <UnityEngine/Vector2.hpp>
#include "logging.hpp"
#include <custom-types/shared/delegate.hpp>

#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, SpectateAreaController);

namespace ScoreSaber::ReplaySystem::UI
{
    void SpectateAreaController::ctor(Zenject::DiContainer* diContainer, Tweening::TimeTweeningManager* timeTweeningManager)
    {
        INVOKE_CTOR();
        NoteData::GameplayType normal = NoteData::GameplayType::Normal;
        _gameNoteControllerPool = diContainer->ResolveId<GameNoteController::Pool*>(il2cpp_functions::value_box(classof(NoteData::GameplayType), &normal));
        _timeTweeningManager = timeTweeningManager;
    }

    void SpectateAreaController::AnimateTo(StringW poseID)
    {
        auto pose = TryGetPose(poseID);
        if (!pose) {
            return;
        }

        if (_statusTween != nullptr) {
            _statusTween->Kill();
        }

        if (_activeNote == nullptr) {
            _activeNote = _gameNoteControllerPool->Spawn();
            _activeNote->enabled = false;
            _activeNote->transform->localScale = Vector3::zero;
            _initialQuaternion = _activeNote->noteTransform->localRotation;
            _activeNote->transform->localPosition = *pose;
            _activeNote->transform->localRotation = Quaternion::identity;
            _activeNote->noteTransform->localPosition = Vector3::zero;
            _activeNote->noteTransform->localRotation = Quaternion::Euler(45, 45, 45);

            auto visuals = _activeNote->GetComponent<ColorNoteVisuals*>();
            visuals->showCircle = false;
            visuals->showArrow = false;
            auto color = ColorExtensions::ColorWithAlpha(Color::cyan, 3.0f);
            visuals->noteColor = color;

            // iteral visuals materialPropertyBlockControllers

            auto blocks = visuals->materialPropertyBlockControllers;
            for (int i = 0; i < blocks->Length(); i++)
            {
                auto block = blocks->values[i];
                block->materialPropertyBlock->SetColor(_colorID, color);
                block->ApplyChanges();
            }
            _despawned = true;
        }

        if (_despawned) {
            _activeNote->gameObject->SetActive(true);
            auto updateNoteScaleDelegate = custom_types::MakeDelegate<System::Action_1<Vector3>*>(classof(System::Action_1<Vector3>*), (std::function<void(Vector3)>)[&](Vector3 scale) { UpdateNoteScale(scale); });
            _statusTween = Tweening::Vector3Tween::New_ctor(Vector3::zero, Vector3::one, updateNoteScaleDelegate, 0.5f, EaseType::OutElastic, 0);
            _timeTweeningManager->AddTween(_statusTween, _activeNote);
            _despawned = false;
        }

        if (_movementTween != nullptr) {
            _movementTween->Kill();
        }
        _activeNote->gameObject->SetActive(true);
        auto movementTweenDelegate = custom_types::MakeDelegate<System::Action_1<Vector3>*>(classof(System::Action_1<Vector3>*), (std::function<void(Vector3)>)[&](Vector3 val) {
            _activeNote->transform->localPosition = val;
        });
        _movementTween = Tweening::Vector3Tween::New_ctor(_activeNote->transform->localPosition, *pose, movementTweenDelegate, 0.75f, EaseType::OutQuart, 0);
        _timeTweeningManager->AddTween(_movementTween, _activeNote);
    }

    void SpectateAreaController::JumpToCallback(StringW poseID)
    {
        auto pose = TryGetPose(poseID);
        if (!pose) {
            return;
        }

        if (DidUpdatePlayerSpectatorPose != nullptr) {
            DidUpdatePlayerSpectatorPose(*pose, Quaternion::identity);
        }
    }

    void SpectateAreaController::Dismiss()
    {
        if (_activeNote == nullptr) {
            return;
        }

        _despawned = true;
        if (_statusTween != nullptr) {
            _statusTween->Kill();
        }
        if (_movementTween != nullptr) {
            _movementTween->Kill();
        }
        auto updateNoteScaleDelegate = custom_types::MakeDelegate<System::Action_1<Vector3>*>(classof(System::Action_1<Vector3>*), (std::function<void(Vector3)>)[&](Vector3 scale) { UpdateNoteScale(scale); });
        _statusTween = Tweening::Vector3Tween::New_ctor(Vector3::one, Vector3::zero, updateNoteScaleDelegate, 0.5f, EaseType::OutQuart, 0);
        _statusTween->onCompleted = custom_types::MakeDelegate<System::Action*>(classof(System::Action*), (std::function<void()>)[&]() { DespawnActiveNote(); });
        _timeTweeningManager->AddTween(_statusTween, _activeNote);
    }

    void SpectateAreaController::Tick()
    {
        if (_activeNote != nullptr) {
            _activeNote->transform->Rotate(Vector3::up * 0.5);
        }
    }

    void SpectateAreaController::UpdateNoteScale(Vector3 scale)
    {
        if (_activeNote != nullptr) {
            _activeNote->transform->localScale = scale;
        }
    }
    void SpectateAreaController::DespawnActiveNote()
    {
        _despawned = true;
        if (_statusTween != nullptr) {
            _statusTween->Kill();
        }
        if (_movementTween != nullptr) {
            _movementTween->Kill();
        }
        _activeNote->gameObject->active = false;
    }
    std::optional<Vector3> SpectateAreaController::TryGetPose(StringW poseID)
    {
        for (auto &sp : Settings::spectatorPositions) {
            if (sp.name == poseID) {
                return Vector3(sp.spectatorPose.x, sp.spectatorPose.y, sp.spectatorPose.z);
            }
        }
        return std::nullopt;
    }

    void SpectateAreaController::Dispose()
    {
        if (_activeNote != nullptr) {
            _timeTweeningManager->KillAllTweens(_activeNote);
        }
    }

} // namespace ScoreSaber::ReplaySystem::UI