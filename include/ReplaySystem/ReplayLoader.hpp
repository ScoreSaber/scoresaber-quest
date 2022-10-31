#pragma once
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayReader.hpp"
#include "Data/Score.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"

#include "ReplaySystem/Playback/NotePlayer.hpp"

namespace ScoreSaber::ReplaySystem::ReplayLoader
{
    extern ScoreSaber::Data::Private::ReplayFile* LoadedReplay;
    extern ScoreSaber::ReplaySystem::Playback::NotePlayer* NotePlayerInstance;
    extern bool IsPlaying;
    void LoadReplay();
    void StartReplay(GlobalNamespace::IDifficultyBeatmap* beatmap);
    void GetReplayData(GlobalNamespace::IDifficultyBeatmap* beatmap, int leaderboardId, ScoreSaber::Data::Score& score, const std::function<void(bool)>& finished);

} // namespace ScoreSaber::ReplaySystem::ReplayLoader