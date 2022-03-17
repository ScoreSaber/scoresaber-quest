

#include "Data/Metadata.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController_InitData.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"

using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::ReplaySystem::Recorders::MetadataRecorder
{
    void LevelStarted(IDifficultyBeatmap* difficultyBeatmap, MainSettingsModelSO* mainSettingsModelSO, IPreviewBeatmapLevel* previewBeatmapLevel,
                      AudioTimeSyncController* audioTimeSyncController, GameplayCoreSceneSetupData* gameplayCoreSceneSetupData,
                      BeatmapObjectSpawnController::InitData* beatmapObjectSpawnControllerInitData);
    void LevelFailed();
    Metadata* Export();

} // namespace ScoreSaber::ReplaySystem::Recorders