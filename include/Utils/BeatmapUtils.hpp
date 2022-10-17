#pragma once

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/register.hpp"
#include "custom-types/shared/types.hpp"

namespace BeatmapUtils
{
    int getDiff(GlobalNamespace::IDifficultyBeatmap* beatmap);
    int getMaxScore(GlobalNamespace::IDifficultyBeatmap* beatmap);
    custom_types::Helpers::Coroutine getMaxScoreCoroutine(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, std::function<void(int maxScore)> callback);
    int getMaxScoreFromCuttableNotesCount(int cuttableNotesCount);

    template <class T>
    void ClearVector(std::vector<T>* vector)
    {
        vector->clear();
        std::vector<T>().swap(*vector);
    };

} // namespace BeatmapUtils