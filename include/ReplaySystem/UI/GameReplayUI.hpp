#pragma once

#include <GlobalNamespace/GameplayCoreSceneSetupData.hpp>
#include <GlobalNamespace/GameplayModifiers.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/RectTransform.hpp>
#include <UnityEngine/Vector2.hpp>
#include <TMPro/TextMeshProUGUI.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(ScoreSaber::ReplaySystem::UI, GameReplayUI, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_FIELD(GlobalNamespace::GameplayCoreSceneSetupData*, _gameplayCoreSceneSetupData);
    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_CTOR(ctor, GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData);
    void CreateReplayUI();
    TMPro::TextMeshProUGUI* CreateText(UnityEngine::RectTransform* parent, StringW text, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, float fontSize);
    std::string GetFriendlyDifficulty(GlobalNamespace::BeatmapDifficulty diff);
};