#pragma once

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/register.hpp"
#include "custom-types/shared/types.hpp"

namespace BeatmapUtils
{
    int getDiff(GlobalNamespace::IDifficultyBeatmap* beatmap);
    int getMaxScore(GlobalNamespace::IBeatmapDataBasicInfo* beatmapDataBasicInfo);
    std::tuple<GlobalNamespace::IBeatmapDataBasicInfo*, GlobalNamespace::IReadonlyBeatmapData*> getBeatmapData(GlobalNamespace::IDifficultyBeatmap* beatmap);
    bool containsV3Stuff(GlobalNamespace::IReadonlyBeatmapData* beatmapData);
    custom_types::Helpers::Coroutine getMaxScoreCoroutine(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, std::function<void(int maxScore)> callback);
    int getMaxScoreFromCuttableNotesCount(int cuttableNotesCount);
    int OldMaxRawScoreForNumberOfNotes(int noteCount);

    GlobalNamespace::GameplayModifiers* GetModifiersFromStrings(std::vector<std::string> modifiers);

    template <class T>
    void ClearVector(std::vector<T>* vector)
    {
        vector->clear();
        std::vector<T>().swap(*vector);
    };

} // namespace BeatmapUtils