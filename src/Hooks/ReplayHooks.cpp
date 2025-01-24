#include "hooks.hpp"

#include <GlobalNamespace/BeatmapObjectSpawnController.hpp>
#include <GlobalNamespace/BeatmapObjectSpawnMovementData.hpp>
#include <GlobalNamespace/ComboController.hpp>
#include <GlobalNamespace/EnvironmentInfoSO.hpp>
#include <GlobalNamespace/GameNoteController.hpp>
#include <GlobalNamespace/GameplayCoreSceneSetupData.hpp>
#include <GlobalNamespace/GameplayModifiersModelSO.hpp>
#include <GlobalNamespace/GoodCutScoringElement.hpp>
#include <GlobalNamespace/LevelCompletionResults.hpp>
#include <GlobalNamespace/MainCamera.hpp>
#include <GlobalNamespace/NoteController.hpp>
#include <GlobalNamespace/PauseController.hpp>
#include <GlobalNamespace/PlayerHeightDetector.hpp>
#include <GlobalNamespace/PlayerSpecificSettings.hpp>
#include <GlobalNamespace/PlayerTransforms.hpp>
#include <GlobalNamespace/PrepareLevelCompletionResults.hpp>
#include <GlobalNamespace/RankModel.hpp>
#include <GlobalNamespace/RelativeScoreAndImmediateRankCounter.hpp>
#include <GlobalNamespace/SaberManager.hpp>
#include <GlobalNamespace/ScoreController.hpp>
#include <GlobalNamespace/IGameEnergyCounter.hpp>
#include <GlobalNamespace/SinglePlayerLevelSelectionFlowCoordinator.hpp>
#include <GlobalNamespace/StandardGameplayInstaller.hpp>
#include <GlobalNamespace/ScoreModel.hpp>
#include <GlobalNamespace/StandardLevelGameplayManager.hpp>
#include "ReplaySystem/ReplayLoader.hpp"
#include <System/Action.hpp>
#include <System/Action_1.hpp>
#include <System/Action_2.hpp>
#include <UnityEngine/Resources.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/MemoryPoolIdInitialSizeMaxSizeBinder_1.hpp>
#include <Zenject/MemoryPool_1.hpp>
#include <Zenject/SceneContext.hpp>
#include "logging.hpp"

using namespace UnityEngine;
using namespace Zenject;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;
using namespace ScoreSaber::ReplaySystem;

// Player Hooks

MAKE_AUTO_HOOK_MATCH(GoodCutScoringElement_Init, &::GlobalNamespace::GoodCutScoringElement::Init, void, GlobalNamespace::GoodCutScoringElement* self, GlobalNamespace::NoteCutInfo noteCutInfo)
{
    GoodCutScoringElement_Init(self, noteCutInfo);

    if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        ScoreSaber::ReplaySystem::ReplayLoader::NotePlayerInstance->ForceCompleteGoodScoringElements(self, noteCutInfo, self->_cutScoreBuffer);
    }
}

// TODO find a proper replacement for this hook. HandleHMDUnmounted has too little IL instructions to be hooked directly without breaking the PauseController
// It is only called as a single callback though, so we could hijack the callback accordingly.
//MAKE_AUTO_HOOK_MATCH(PauseController_HandleHMDUnmounted, &PauseController::HandleHMDUnmounted, void, PauseController* self)
//{
//    if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
//    {
//        return;
//    }
//    PauseController_HandleHMDUnmounted(self);
//}

MAKE_AUTO_HOOK_MATCH(GameNoteController_HandleCut_Event, &GameNoteController::HandleCut,
                     void, GameNoteController* self, GlobalNamespace::Saber* saber, UnityEngine::Vector3 cutPoint, UnityEngine::Quaternion orientation, UnityEngine::Vector3 cutDirVec, bool allowBadCut)
{
    if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        return;
    }

    GameNoteController_HandleCut_Event(self, saber, cutPoint, orientation, cutDirVec, allowBadCut);
}

MAKE_AUTO_HOOK_MATCH(RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, &RelativeScoreAndImmediateRankCounter::UpdateRelativeScoreAndImmediateRank,
                     void, RelativeScoreAndImmediateRankCounter* self, int score, int modifiedScore, int maxPossibleScore, int maxPossibleModifiedScore)
{
    if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        if (score == 0 && maxPossibleScore == 0)
        {
            self->relativeScore = 1.0f;
            self->immediateRank = RankModel::Rank::SS;
            self->relativeScoreOrImmediateRankDidChangeEvent->Invoke();
            return;
        }
        RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank(self, score, modifiedScore, maxPossibleScore, maxPossibleModifiedScore);
    }
    RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank(self, score, modifiedScore, maxPossibleScore, maxPossibleModifiedScore);
}

MAKE_AUTO_HOOK_MATCH(PrepareLevelCompletionResults_FillLevelCompletionResults, &PrepareLevelCompletionResults::FillLevelCompletionResults,
                     LevelCompletionResults*, PrepareLevelCompletionResults* self, LevelCompletionResults::LevelEndStateType levelEndStateType, LevelCompletionResults::LevelEndAction levelEndAction)
{
    if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        levelEndStateType = LevelCompletionResults::LevelEndStateType::Incomplete;
    }
    auto value = PrepareLevelCompletionResults_FillLevelCompletionResults(self, levelEndStateType, levelEndAction);
    return value;
}

// CancelScoreControllerBufferFinisher
MAKE_AUTO_HOOK_ORIG_MATCH(ScoreController_LateUpdate, &ScoreController::LateUpdate, void, ScoreController* self)
{
    if (!ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        ScoreController_LateUpdate(self);
        return;
    }
    float num = (self->_sortedNoteTimesWithoutScoringElements->Count > 0) ? self->_sortedNoteTimesWithoutScoringElements->get_Item(0) : 3.4028235E+38f;
    float num2 = self->_audioTimeSyncController->songTime + 0.15f;

    int num3 = 0;
    bool flag = false;
    for (int i = 0; i < self->_sortedScoringElementsWithoutMultiplier->Count; i++)
    {
        auto scoringElement = self->_sortedScoringElementsWithoutMultiplier->get_Item(i);
        if (scoringElement->time >= num2 && scoringElement->time <= num)
        {
            break;
        }
        flag |= self->_scoreMultiplierCounter->ProcessMultiplierEvent(scoringElement->multiplierEventType);
        if (scoringElement->wouldBeCorrectCutBestPossibleMultiplierEventType == ScoreMultiplierCounter::MultiplierEventType::Positive)
        {
            self->_maxScoreMultiplierCounter->ProcessMultiplierEvent(ScoreMultiplierCounter::MultiplierEventType::Positive);
        }
        scoringElement->SetMultipliers(self->_scoreMultiplierCounter->multiplier, self->_maxScoreMultiplierCounter->multiplier);
        self->_scoringElementsWithMultiplier->Add(scoringElement);
        num3++;
    }
    self->_sortedScoringElementsWithoutMultiplier->RemoveRange(0, num3);
    if (flag)
    {
        if (self->multiplierDidChangeEvent != nullptr)
        {
            self->multiplierDidChangeEvent->Invoke(self->_scoreMultiplierCounter->multiplier, self->_scoreMultiplierCounter->normalizedProgress);
        }
    }
    bool flag2 = false;
    self->_scoringElementsToRemove->Clear();
    for (int j = 0; j < self->_scoringElementsWithMultiplier->Count; j++)
    {
        auto scoringElement2 = self->_scoringElementsWithMultiplier->get_Item(j);
        if (scoringElement2->isFinished)
        {
            if ((float)scoringElement2->maxPossibleCutScore > 0.0f)
            {
                flag2 = true;
                // self->multipliedScore += scoringElement2->cutScore * scoringElement2->multiplier;
                // self->immediateMaxPossibleMultipliedScore += scoringElement2->maxPossibleCutScore * scoringElement2->maxMultiplier;
            }
            self->_scoringElementsToRemove->Add(scoringElement2);
            if (self->scoringForNoteFinishedEvent)
            {
                self->scoringForNoteFinishedEvent->Invoke(scoringElement2);
            }
        }
    }
    for (int k = 0; k < self->_scoringElementsToRemove->Count; k++)
    {
        auto scoringElement3 = self->_scoringElementsToRemove->get_Item(k);
        self->DespawnScoringElement(scoringElement3);
        self->_scoringElementsWithMultiplier->Remove(scoringElement3);
    }
    self->_scoringElementsToRemove->Clear();
    float totalMultiplier = self->_gameplayModifiersModel->GetTotalMultiplier(self->_gameplayModifierParams, self->_gameEnergyCounter->energy);
    if (self->_prevMultiplierFromModifiers != totalMultiplier)
    {
        self->_prevMultiplierFromModifiers = totalMultiplier;
        flag2 = true;
    }
    if (flag2)
    {
        self->_modifiedScore = ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(self->multipliedScore, totalMultiplier);
        self->_immediateMaxPossibleModifiedScore = ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(self->immediateMaxPossibleMultipliedScore, totalMultiplier);
        if (self->scoreDidChangeEvent != nullptr)
        {
            self->scoreDidChangeEvent->Invoke(self->multipliedScore, self->modifiedScore);
        }
    }
}
