#include "ReplaySystem/UI/SpectateAreaController.hpp"
#include "GlobalNamespace/ColorExtensions.hpp"
#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/MaterialPropertyBlockController.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberMovementData.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/RectTransformUtility.hpp"
#include "UnityEngine/Vector2.hpp"
#include "questui/shared/BeatSaberUI.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, SpectateAreaController);

namespace ScoreSaber::ReplaySystem::UI
{
    void SpectateAreaController::ctor(Zenject::DiContainer* diContainer, Tweening::TimeTweeningManager* timeTweeningManager)
    {
        int gameplayType = 0;
        _gameNoteControllerPool = diContainer->ResolveId<GameNoteController::Pool*>(il2cpp_functions::value_box(classof(int), &gameplayType));
        _timeTweeningManager = timeTweeningManager;
    }

    void SpectateAreaController::AnimateTo(StringW poseID)
    {

        auto pose = TryGetPose(poseID);
        if (pose == Vector3::get_zero())
        {
            return;
        }

        if (_statusTween != nullptr)
        {
            _statusTween->Kill();
        }

        if (_activeNote == nullptr)
        {
            _activeNote = _gameNoteControllerPool->Spawn();
            _activeNote->set_enabled(false);
            _activeNote->get_transform()->set_localScale(Vector3::get_zero());
            _initialQuaternion = _activeNote->noteTransform->get_localRotation();
            _activeNote->get_transform()->set_localPosition(pose);
            _activeNote->get_transform()->set_localRotation(Quaternion::get_identity());

            auto visuals = _activeNote->GetComponent<ColorNoteVisuals*>();
            visuals->set_showCircle(false);
            visuals->set_showArrow(false);
            auto color = ColorExtensions::ColorWithAlpha(Color::get_cyan(), 3.0f);
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

        // if (_despawned) {
        //     _activeNote->get_gameObject()->SetActive(true);
        //     _statusTween  = _timeTweeningManager->TweenValueTo(0.0f, 1.0f, 0.5f, Tweening::EaseType::EaseOutQuad, [this](float value) {
        //         _activeNote->get_transform()->set_localScale(Vector3::get_one() * value);
        //     }, [this]() {
        //         _despawned = false;
        //     });
        // }
    }

    void SpectateAreaController::JumpToCallback(StringW poseID)
    {
    }

    void SpectateAreaController::Dismiss()
    {
    }

    void SpectateAreaController::Tick()
    {
    }

    void SpectateAreaController::UpdateNoteScale(Vector3 scale)
    {
    }
    void SpectateAreaController::DespawnActiveNote()
    {
    }

    Vector3 SpectateAreaController::TryGetPose(StringW poseID)
    {
        return Vector3::get_zero();
    }

    void SpectateAreaController::Dispose()
    {
    }

} // namespace ScoreSaber::ReplaySystem::UI