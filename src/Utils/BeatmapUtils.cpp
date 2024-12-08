#include "Utils/BeatmapUtils.hpp"
#include <GlobalNamespace/BeatmapDataItem.hpp>
#include <GlobalNamespace/IDifficultyBeatmap.hpp>
#include <GlobalNamespace/PlayerData.hpp>
#include <GlobalNamespace/ScoreModel.hpp>
// #include <System::Threading::Tasks::Task_1.hpp>
// System::Collections::Generic::LinkedListNode_1

#include <GlobalNamespace/BeatmapEnvironmentHelper.hpp>
#include <GlobalNamespace/EnvironmentInfoSO.hpp>
#include <GlobalNamespace/EnvironmentsListSO.hpp>
#include <GlobalNamespace/IBeatmapDataBasicInfo.hpp>
#include <GlobalNamespace/SliderData.hpp>
#include <System/Collections/Generic/LinkedListNode_1.hpp>
//
#include <System/Collections/Generic/LinkedList_1.hpp>
#include <System/Threading/Tasks/Task_1.hpp>
#include <UnityEngine/Resources.hpp>
#include "logging.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include <tuple>

using namespace GlobalNamespace;

namespace BeatmapUtils
{
    std::vector<int> routines;

    int getDiff(IDifficultyBeatmap* beatmap)
    {
        return beatmap->difficultyRank;
    }

    custom_types::Helpers::Coroutine getMaxScoreCoroutine(IDifficultyBeatmap* difficultyBeatmap, std::function<void(int maxScore)> callback)
    {
        if (playerDataModel == nullptr)
        {
            playerDataModel = UnityEngine::Object::FindObjectOfType<PlayerDataModel*>();
        }
        int crIndex = routines.size() + 1;
        routines.push_back(crIndex);
        auto* envInfo = BeatmapEnvironmentHelper::GetEnvironmentInfo(difficultyBeatmap);
        auto* result = difficultyBeatmap->GetBeatmapDataAsync(envInfo, playerDataModel->playerData->playerSpecificSettings);
        while (!result->IsCompleted)
            co_yield nullptr;
        auto* data = result->ResultOnSuccess;
        if (routines.empty() || routines.size() != crIndex)
            co_return;
        ClearVector<int>(&routines);
        int maxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(data);
        callback(maxScore);
        co_return;
    }

    int getMaxScore(IReadonlyBeatmapData* beatmapData)
    {
        return ScoreModel::ComputeMaxMultipliedScoreForBeatmap(beatmapData);
    }

    std::tuple<IBeatmapDataBasicInfo*, IReadonlyBeatmapData*> getBeatmapData(IDifficultyBeatmap* beatmap)
    {
        if (playerDataModel == nullptr)
        {
            playerDataModel = UnityEngine::Object::FindObjectOfType<PlayerDataModel*>();
        }
        auto environmentInfo = BeatmapEnvironmentHelper::GetEnvironmentInfo(beatmap);
        System::Threading::Tasks::Task_1<IReadonlyBeatmapData*>* result = nullptr;
        QuestUI::MainThreadScheduler::Schedule([&]() {
            result = beatmap->GetBeatmapDataAsync(environmentInfo, playerDataModel->playerData->playerSpecificSettings);
        });
        while (!result || !result->IsCompleted)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        auto data = result->ResultOnSuccess;
        auto beatmapDataBasicInfo = il2cpp_utils::try_cast<IBeatmapDataBasicInfo>(data).value_or(nullptr);
        return std::make_tuple(beatmapDataBasicInfo, data);
    }

    int OldMaxRawScoreForNumberOfNotes(int noteCount)
    {
        int num = 0;
        int num2 = 1;
        while (num2 < 8)
        {
            if (noteCount >= num2 * 2)
            {
                num += num2 * num2 * 2 + num2;
                noteCount -= num2 * 2;
                num2 *= 2;
                continue;
            }
            num += num2 * noteCount;
            noteCount = 0;
            break;
        }
        num += noteCount * num2;
        return num * 115;
    }

    GameplayModifiers* GetModifiersFromStrings(std::vector<std::string> modifiers)
    {
        auto energyType = GameplayModifiers::EnergyType::Bar;
        auto obstacleType = GameplayModifiers::EnabledObstacleType::All;
        auto songSpeed = GameplayModifiers::SongSpeed::Normal;

        bool NF = false;
        bool IF = false;
        bool NB = false;
        bool DA = false;
        bool GN = false;
        bool NA = false;
        bool PM = false;
        bool SC = false;
        bool SA = false;

        // iterate modifiers
        for (auto& modifier : modifiers)
        {
            if (modifier == "BE")
            {
                energyType = GameplayModifiers::EnergyType::Battery;
            }
            else if (modifier == "NF")
            {
                NF = true;
            }
            else if (modifier == "IF")
            {
                IF = true;
            }
            else if (modifier == "NO")
            {
                obstacleType = GameplayModifiers::EnabledObstacleType::NoObstacles;
            }
            else if (modifier == "NB")
            {
                NB = true;
            }
            else if (modifier == "DA")
            {
                DA = true;
            }
            else if (modifier == "GN")
            {
                GN = true;
            }
            else if (modifier == "NA")
            {
                NA = true;
            }
            else if (modifier == "SS")
            {
                songSpeed = GameplayModifiers::SongSpeed::Slower;
            }
            else if (modifier == "FS")
            {
                songSpeed = GameplayModifiers::SongSpeed::Faster;
            }
            else if (modifier == "SF")
            {
                songSpeed = GameplayModifiers::SongSpeed::SuperFast;
            }
            else if (modifier == "PM")
            {
                PM = true;
            }
            else if (modifier == "SC")
            {
                SC = true;
            }
            else if (modifier == "SA")
            {
                SA = true;
            }
        }

        return GameplayModifiers::New_ctor(energyType, NF, IF, false, obstacleType, NB, false, SA, DA, songSpeed, NA, GN, PM, false, SC);
    }

} // namespace BeatmapUtils