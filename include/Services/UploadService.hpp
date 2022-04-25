#pragma once
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"

#include <iomanip>
#include <sstream>
#include <vector>

namespace ScoreSaber::Services::UploadService
{
    extern bool uploading;
    void PrepareAndUploadScore(GlobalNamespace::IDifficultyBeatmap* beatmap, int rawScore,
                               int modifiedScore, bool fullCombo, int goodCutsCount, int badCutsCount, int missedCount, int maxCombo,
                               float energy, GlobalNamespace::GameplayModifiers* gameplayModifiers);
    void UploadScore(std::string scorePacket, std::function<void(bool)> finished);
    std::string CreateScorePacket(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, int rawScore,
                                  int modifiedScore, bool fullCombo, int badCutsCount, int missedCount, int maxCombo, float energy,
                                  GlobalNamespace::GameplayModifiers* gameplayModifiers);
    std::vector<std::string> GetModifierList(GlobalNamespace::GameplayModifiers* gameplayModifiers, float energy);
    std::vector<unsigned char> Swap(std::vector<unsigned char> panda1, std::vector<unsigned char> panda2);
    std::string ConvertToHex(const std::vector<unsigned char>& v);
} // namespace ScoreSaber::Services::UploadService
