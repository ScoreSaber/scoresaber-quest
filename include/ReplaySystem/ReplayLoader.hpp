#pragma once
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayReader.hpp"
#include "Data/Score.hpp"
#include <GlobalNamespace/BeatmapKey.hpp>
#include <GlobalNamespace/BeatmapLevel.hpp>
#include <GlobalNamespace/LevelCompletionResults.hpp>
#include <GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp>

#include "ReplaySystem/Playback/NotePlayer.hpp"

namespace ScoreSaber::ReplaySystem::ReplayLoader
{
    extern std::shared_ptr<ScoreSaber::Data::Private::ReplayFile> LoadedReplay;
    extern GlobalNamespace::BeatmapKey CurrentBeatmapKey;
    extern SafePtr<GlobalNamespace::BeatmapLevel> CurrentBeatmapLevel;
    extern std::u16string CurrentPlayerName;
    extern std::string CurrentModifiers;
    extern SafePtr<ScoreSaber::ReplaySystem::Playback::NotePlayer> NotePlayerInstance;

    extern bool IsPlaying;
    void StartReplay(GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey);
    void Load(const std::vector<char> &replayData, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, std::string modifiers, std::u16string playerName);
    void GetReplayData(GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, int leaderboardId, std::string replayFileName, ScoreSaber::Data::Score& score, const std::function<void(bool)>& finished);

    void OnSoftRestart();

} // namespace ScoreSaber::ReplaySystem::ReplayLoader