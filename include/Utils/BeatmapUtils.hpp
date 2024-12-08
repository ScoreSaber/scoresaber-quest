#pragma once

#include <GlobalNamespace/BeatmapData.hpp>
#include <GlobalNamespace/GameplayModifiers.hpp>
#include <GlobalNamespace/PlayerDataModel.hpp>
#include <GlobalNamespace/PlayerSpecificSettings.hpp>

#include <custom-types/shared/coroutine.hpp>
#include <custom-types/shared/register.hpp>
#include <custom-types/shared/types.hpp>

namespace BeatmapUtils
{
    static GlobalNamespace::PlayerDataModel* playerDataModel;
    int getDiff(GlobalNamespace::IDifficultyBeatmap* beatmap);
    int getMaxScore(GlobalNamespace::IReadonlyBeatmapData* beatmapData);
    std::tuple<GlobalNamespace::IBeatmapDataBasicInfo*, GlobalNamespace::IReadonlyBeatmapData*> getBeatmapData(GlobalNamespace::IDifficultyBeatmap* beatmap);
    custom_types::Helpers::Coroutine getMaxScoreCoroutine(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, std::function<void(int maxScore)> callback);
    int OldMaxRawScoreForNumberOfNotes(int noteCount);

    GlobalNamespace::GameplayModifiers* GetModifiersFromStrings(std::vector<std::string> modifiers);

    template <class T>
    void ClearVector(std::vector<T>* vector)
    {
        vector->clear();
        std::vector<T>().swap(*vector);
    };

} // namespace BeatmapUtils