#include "Utils/BeatmapUtils.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
// #include "System::Threading::Tasks::Task_1.hpp"
// System::Collections::Generic::LinkedListNode_1

#include "GlobalNamespace/BeatmapEnvironmentHelper.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/EnvironmentsListSO.hpp"
#include "GlobalNamespace/IBeatmapDataBasicInfo.hpp"
#include "GlobalNamespace/SliderData.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"
//
#include "System/Collections/Generic/LinkedList_1.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "UnityEngine/Resources.hpp"
#include "logging.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include <tuple>

namespace BeatmapUtils
{

    GlobalNamespace::PlayerDataModel* playerDataModel;
    std::vector<int> routines;

    int getDiff(GlobalNamespace::IDifficultyBeatmap* beatmap)
    {
        return beatmap->get_difficultyRank();
    }

    custom_types::Helpers::Coroutine getMaxScoreCoroutine(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, std::function<void(int maxScore)> callback)
    {
        if (playerDataModel == nullptr)
        {
            playerDataModel = UnityEngine::Object::FindObjectOfType<GlobalNamespace::PlayerDataModel*>();
        }
        int crIndex = routines.size() + 1;
        routines.push_back(crIndex);
        auto* envInfo = GlobalNamespace::BeatmapEnvironmentHelper::GetEnvironmentInfo(difficultyBeatmap);
        auto* result = difficultyBeatmap->GetBeatmapDataAsync(envInfo, playerDataModel->playerData->playerSpecificSettings);
        while (!result->get_IsCompleted())
            co_yield nullptr;
        auto* data = result->get_ResultOnSuccess();
        if (routines.empty() || routines.size() != crIndex)
            co_return;
        ClearVector<int>(&routines);
        auto beatmapDataBasicInfo = il2cpp_utils::try_cast<GlobalNamespace::IBeatmapDataBasicInfo>(data).value_or(nullptr);
        int blockCount = beatmapDataBasicInfo->get_cuttableNotesCount();
        callback(getMaxScoreFromCuttableNotesCount(blockCount));
        co_return;
    }

    int getMaxScore(GlobalNamespace::IBeatmapDataBasicInfo* beatmapDataBasicInfo)
    {
        int blockCount = beatmapDataBasicInfo->get_cuttableNotesCount();
        return getMaxScoreFromCuttableNotesCount(blockCount);
    }

    std::tuple<GlobalNamespace::IBeatmapDataBasicInfo*, GlobalNamespace::IReadonlyBeatmapData*> getBeatmapData(GlobalNamespace::IDifficultyBeatmap* beatmap)
    {
        if (playerDataModel == nullptr)
        {
            playerDataModel = UnityEngine::Object::FindObjectOfType<GlobalNamespace::PlayerDataModel*>();
        }
        auto environmentInfo = GlobalNamespace::BeatmapEnvironmentHelper::GetEnvironmentInfo(beatmap);
        System::Threading::Tasks::Task_1<GlobalNamespace::IReadonlyBeatmapData*>* result = nullptr;
        QuestUI::MainThreadScheduler::Schedule([&]() {
            result = beatmap->GetBeatmapDataAsync(environmentInfo, playerDataModel->playerData->playerSpecificSettings);
        });
        while (!result || !result->get_IsCompleted())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        auto data = result->get_ResultOnSuccess();
        auto beatmapDataBasicInfo = il2cpp_utils::try_cast<GlobalNamespace::IBeatmapDataBasicInfo>(data).value_or(nullptr);
        return std::make_tuple(beatmapDataBasicInfo, data);
    }

    bool containsV3Stuff(GlobalNamespace::IReadonlyBeatmapData* beatmapData)
    {
        // iterate over allBeatmapDataItems

        auto list = beatmapData->get_allBeatmapDataItems();
        for (auto i = list->head; i->next != list->head; i = i->next)
        {
            if (i->item->type == 0 && il2cpp_utils::try_cast<GlobalNamespace::SliderData>(i->item).value_or(nullptr))
            {
                return true;
            }
        }
        return false;
    }

    int getMaxScoreFromCuttableNotesCount(int cuttableNotesCount)
    {
        int maxScore;
        if (cuttableNotesCount < 14)
        {
            if (cuttableNotesCount == 1)
            {
                maxScore = 115;
            }
            else if (cuttableNotesCount < 5)
            {
                maxScore = (cuttableNotesCount - 1) * 230 + 115;
            }
            else
            {
                maxScore = (cuttableNotesCount - 5) * 460 + 1035;
            }
        }
        else
        {
            maxScore = (cuttableNotesCount - 13) * 920 + 4715;
        }
        if (maxScore == 0)
            return -1;
        return maxScore;
    }
} // namespace BeatmapUtils