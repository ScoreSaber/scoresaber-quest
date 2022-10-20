#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController_InitData.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnMovementData.hpp"
#include "GlobalNamespace/ComboController.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/GameplayModifiersModelSO.hpp"
#include "GlobalNamespace/GoodCutScoringElement.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/PauseController.hpp"
#include "GlobalNamespace/PlayerHeightDetector.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PlayerTransforms.hpp"
#include "GlobalNamespace/PrepareLevelCompletionResults.hpp"
#include "GlobalNamespace/RankModel.hpp"
#include "GlobalNamespace/RelativeScoreAndImmediateRankCounter.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/SinglePlayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/StandardGameplayInstaller.hpp"
#include "GlobalNamespace/StandardLevelGameplayManager.hpp"
#include "ReplaySystem/Recorders/EnergyEventRecorder.hpp"
#include "ReplaySystem/Recorders/HeightEventRecorder.hpp"
#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "ReplaySystem/Recorders/PoseRecorder.hpp"
#include "ReplaySystem/Recorders/ScoreEventRecorder.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "System/Action.hpp"
#include "System/Action_1.hpp"
#include "System/Action_2.hpp"
#include "UnityEngine/Resources.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/MemoryPoolIdInitialSizeMaxSizeBinder_1.hpp"
#include "Zenject/MemoryPool_1.hpp"
#include "Zenject/SceneContext.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"

using namespace UnityEngine;
using namespace QuestUI;
using namespace Zenject;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;
using namespace ScoreSaber::ReplaySystem;

GameplayCoreSceneSetupData* _gameplayCoreSceneSetupData;
BeatmapObjectSpawnController::InitData* _beatmapObjectSpawnControllerInitData;
AudioTimeSyncController* _audioTimeSyncController;

MAKE_AUTO_HOOK_MATCH(AudioTimeSyncController_Start, &AudioTimeSyncController::Start, void, AudioTimeSyncController* self)
{
    AudioTimeSyncController_Start(self);
    _audioTimeSyncController = self;

    if (!ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        MainSettingsModelSO* mainSettingsModelSO = ArrayUtil::First(Resources::FindObjectsOfTypeAll<MainSettingsModelSO*>());
        SaberManager* saberManager = ArrayUtil::First(Resources::FindObjectsOfTypeAll<SaberManager*>());
        PlayerHeightDetector* playerHeightDetector = ArrayUtil::First(Resources::FindObjectsOfTypeAll<PlayerHeightDetector*>());
        ScoreController* scoreController = ArrayUtil::First(Resources::FindObjectsOfTypeAll<ScoreController*>());
        ComboController* comboController = ArrayUtil::First(Resources::FindObjectsOfTypeAll<ComboController*>());
        GameEnergyCounter* gameEnergyCountry = ArrayUtil::First(Resources::FindObjectsOfTypeAll<GameEnergyCounter*>());

        Recorders::MetadataRecorder::LevelStarted(_gameplayCoreSceneSetupData->difficultyBeatmap, mainSettingsModelSO,
                                                  _gameplayCoreSceneSetupData->previewBeatmapLevel, _audioTimeSyncController,
                                                  _gameplayCoreSceneSetupData, _beatmapObjectSpawnControllerInitData);
        Recorders::PoseRecorder::LevelStarted(saberManager, _audioTimeSyncController);
        Recorders::HeightEventRecorder::LevelStarted(playerHeightDetector, _audioTimeSyncController);
        Recorders::ScoreEventRecorder::LevelStarted(scoreController, _audioTimeSyncController, comboController);
        Recorders::EnergyEventRecorder::LevelStarted(gameEnergyCountry, _audioTimeSyncController);
        Recorders::NoteEventRecorder::LevelStarted(scoreController, _audioTimeSyncController);
    }
}

MAKE_AUTO_HOOK_FIND_CLASS_UNSAFE_INSTANCE(GameplayCoreSceneSetupData_ctor, "", "GameplayCoreSceneSetupData", ".ctor", void,
                                          GameplayCoreSceneSetupData* self, IDifficultyBeatmap* difficultyBeatmap, IPreviewBeatmapLevel* previewBeatmapLevel,
                                          GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings,
                                          bool useTestNoteCutSoundEffects, EnvironmentInfoSO* environmentInfo, ColorScheme* colorScheme, MainSettingsModelSO* mainSettingsModel)
{
    GameplayCoreSceneSetupData_ctor(self, difficultyBeatmap, previewBeatmapLevel, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects,
                                    environmentInfo, colorScheme, mainSettingsModel);
    _gameplayCoreSceneSetupData = self;
}

MAKE_AUTO_HOOK_FIND_CLASS_UNSAFE_INSTANCE(BeatmapObjectSpawnControllerInitData_ctor, "", "BeatmapObjectSpawnController/InitData", ".ctor", void,
                                          BeatmapObjectSpawnController::InitData* self, float beatsPerMinute,
                                          int noteLinesCount, float noteJumpMovementSpeed, BeatmapObjectSpawnMovementData::NoteJumpValueType noteJumpValueType, float noteJumpValue)
{
    BeatmapObjectSpawnControllerInitData_ctor(self, beatsPerMinute, noteLinesCount, noteJumpMovementSpeed, noteJumpValueType, noteJumpValue);
    _beatmapObjectSpawnControllerInitData = self;
}

MAKE_AUTO_HOOK_MATCH(PlayerTransforms_Update, &PlayerTransforms::Update, void, PlayerTransforms* self)
{
    PlayerTransforms_Update(self);

    if (_audioTimeSyncController != nullptr && !ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        Recorders::PoseRecorder::Tick(self->get_headPseudoLocalPos(), self->get_headPseudoLocalRot());
    }
}

MAKE_AUTO_HOOK_MATCH(StandardLevelGameplayManager_HandleGameEnergyDidReach0, &StandardLevelGameplayManager::HandleGameEnergyDidReach0, void,
                     StandardLevelGameplayManager* self)
{
    StandardLevelGameplayManager_HandleGameEnergyDidReach0(self);
    if (!ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        Recorders::MetadataRecorder::LevelFailed();
    }
}

MAKE_AUTO_HOOK_MATCH(ScoreController_HandleNoteWasCut, &ScoreController::HandleNoteWasCut, void, ScoreController* self,
                     GlobalNamespace::NoteController* noteController, ByRef<GlobalNamespace::NoteCutInfo> noteCutInfo)
{
    ScoreController_HandleNoteWasCut(self, noteController, noteCutInfo);
    if (!ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        Recorders::NoteEventRecorder::BadCutInfoCollector(noteController, noteCutInfo.heldRef);
    }
}

// Player Hooks

MAKE_AUTO_HOOK_MATCH(GoodCutScoringElement_Init, &GoodCutScoringElement::Init, void, GoodCutScoringElement* self, NoteCutInfo noteCutInfo)
{
    GoodCutScoringElement_Init(self, noteCutInfo);

    if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        ScoreSaber::ReplaySystem::ReplayLoader::NotePlayerInstance->ForceCompleteGoodScoringElements(self, noteCutInfo, self->cutScoreBuffer);
    }
}

MAKE_AUTO_HOOK_MATCH(PauseController_HandleHMDUnmounted, &PauseController::HandleHMDUnmounted, void, PauseController* self)
{
    if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying)
    {
        return;
    }
    PauseController_HandleHMDUnmounted(self);
}

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
    float num = (self->sortedNoteTimesWithoutScoringElements->get_Count() > 0) ? self->sortedNoteTimesWithoutScoringElements->get_Item(0) : 3.4028235E+38f;
    float num2 = self->audioTimeSyncController->songTime + 0.15f;

    int num3 = 0;
    bool flag = false;
    for (int i = 0; i < self->sortedScoringElementsWithoutMultiplier->get_Count(); i++)
    {
        auto scoringElement = self->sortedScoringElementsWithoutMultiplier->get_Item(i);
        if (scoringElement->get_time() >= num2 && scoringElement->get_time() <= num)
        {
            break;
        }
        flag |= self->scoreMultiplierCounter->ProcessMultiplierEvent(scoringElement->get_multiplierEventType());
        if (scoringElement->get_wouldBeCorrectCutBestPossibleMultiplierEventType() == ScoreMultiplierCounter::MultiplierEventType::Positive)
        {
            self->maxScoreMultiplierCounter->ProcessMultiplierEvent(ScoreMultiplierCounter::MultiplierEventType::Positive);
        }
        scoringElement->SetMultipliers(self->scoreMultiplierCounter->multiplier, self->maxScoreMultiplierCounter->multiplier);
        self->scoringElementsWithMultiplier->Add(scoringElement);
        num3++;
    }
    self->sortedScoringElementsWithoutMultiplier->RemoveRange(0, num3);
    if (flag)
    {
        if (self->multiplierDidChangeEvent != nullptr)
        {
            self->multiplierDidChangeEvent->Invoke(self->scoreMultiplierCounter->multiplier, self->scoreMultiplierCounter->get_normalizedProgress());
        }
    }
    bool flag2 = false;
    self->scoringElementsToRemove->Clear();
    for (int j = 0; j < self->scoringElementsWithMultiplier->get_Count(); j++)
    {
        auto scoringElement2 = self->scoringElementsWithMultiplier->get_Item(j);
        if (scoringElement2->isFinished)
        {
            if ((float)scoringElement2->get_maxPossibleCutScore() > 0.0f)
            {
                flag2 = true;
                // self->multipliedScore += scoringElement2->get_cutScore() * scoringElement2->multiplier;
                self->immediateMaxPossibleMultipliedScore += scoringElement2->get_maxPossibleCutScore() * scoringElement2->maxMultiplier;
            }
            self->scoringElementsToRemove->Add(scoringElement2);
            if (self->scoringForNoteFinishedEvent)
            {
                self->scoringForNoteFinishedEvent->Invoke(scoringElement2);
            }
        }
    }
    for (int k = 0; k < self->scoringElementsToRemove->get_Count(); k++)
    {
        auto scoringElement3 = self->scoringElementsToRemove->get_Item(k);
        self->DespawnScoringElement(scoringElement3);
        self->scoringElementsWithMultiplier->Remove(scoringElement3);
    }
    self->scoringElementsToRemove->Clear();
    float totalMultiplier = self->gameplayModifiersModel->GetTotalMultiplier(self->gameplayModifierParams, self->gameEnergyCounter->get_energy());
    if (self->prevMultiplierFromModifiers != totalMultiplier)
    {
        self->prevMultiplierFromModifiers = totalMultiplier;
        flag2 = true;
    }
    if (flag2)
    {
        self->modifiedScore = ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(self->multipliedScore, totalMultiplier);
        self->immediateMaxPossibleModifiedScore = ScoreModel::GetModifiedScoreForGameplayModifiersScoreMultiplier(self->immediateMaxPossibleMultipliedScore, totalMultiplier);
        if (self->scoreDidChangeEvent != nullptr)
        {
            self->scoreDidChangeEvent->Invoke(self->multipliedScore, self->modifiedScore);
        }
    }
}
