#pragma once
#include <GlobalNamespace/GameplayModifiers.hpp>
#include <GlobalNamespace/LevelCompletionResults.hpp>
#include <GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp>
#include <GlobalNamespace/MultiplayerResultsData.hpp>
#include <GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp>

#include <iomanip>
#include <sstream>
#include <vector>

namespace ScoreSaber::Services::UploadService
{
    extern bool uploading;

    void Three(GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* standardLevelScenesTransitionSetupDataSO, GlobalNamespace::LevelCompletionResults* levelCompletionResults);
    
    void Four(GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO* multiplayerLevelScenesTransitionSetupDataSO, GlobalNamespace::MultiplayerResultsData* multiplayerResultsData);

    void Five(StringW gameMode, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, GlobalNamespace::LevelCompletionResults* levelCompletionResults, bool practicing);

    void Six(GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, GlobalNamespace::LevelCompletionResults* levelCompletionResults);

    void Seven(GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, int modifiedScore, int multipliedScore, std::string uploadPacket, std::string replayFileName);
    
    void SaveReplay(const std::vector<char> &replay, std::string replayFileName);

    // void UploadScore(std::string scorePacket, std::function<void(bool)> finished);

    std::string CreateScorePacket(GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, int rawScore,
                                  int modifiedScore, bool fullCombo, int badCutsCount, int missedCount, int maxCombo, float energy,
                                  GlobalNamespace::GameplayModifiers* gameplayModifiers);
    std::vector<std::string> GetModifierList(GlobalNamespace::GameplayModifiers* gameplayModifiers, float energy);
    std::vector<unsigned char> Swap(std::vector<unsigned char> panda1, std::vector<unsigned char> panda2);
    std::string ConvertToHex(const std::vector<unsigned char>& v);
    std::string GetVersionHash();
} // namespace ScoreSaber::Services::UploadService
