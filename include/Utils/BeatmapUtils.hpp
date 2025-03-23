#pragma once

#include <vector>
#include <GlobalNamespace/GameplayModifiers.hpp>

namespace BeatmapUtils
{
    int OldMaxRawScoreForNumberOfNotes(int noteCount);
    GlobalNamespace::GameplayModifiers* GetModifiersFromStrings(std::vector<std::string> modifiers);
} // namespace BeatmapUtils