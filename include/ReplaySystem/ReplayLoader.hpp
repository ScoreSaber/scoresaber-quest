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
    extern std::shared_ptr<ScoreSaber::Data::Private::ReplayFile> LoadedReplay;
    extern GlobalNamespace::IDifficultyBeatmap* CurrentLevel;
    extern std::u16string CurrentPlayerName;
    extern std::string CurrentModifiers;
    extern ScoreSaber::ReplaySystem::Playback::NotePlayer* NotePlayerInstance;

    extern bool IsPlaying;
    void StartReplay(GlobalNamespace::IDifficultyBeatmap* beatmap);
    void Load(const std::vector<char> &replayData, GlobalNamespace::IDifficultyBeatmap* beatmap, std::string modifiers, std::u16string playerName);
    void GetReplayData(GlobalNamespace::IDifficultyBeatmap* beatmap, int leaderboardId, std::string replayFileName, ScoreSaber::Data::Score& score, const std::function<void(bool)>& finished);

} // namespace ScoreSaber::ReplaySystem::ReplayLoader