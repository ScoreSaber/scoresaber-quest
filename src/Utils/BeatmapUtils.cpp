#include "Utils/BeatmapUtils.hpp"
namespace BeatmapUtils
{
    int getDiff(GlobalNamespace::IDifficultyBeatmap* beatmap)
    {
        return beatmap->get_difficultyRank();
    }

    int getMaxScore(GlobalNamespace::IDifficultyBeatmap* beatmap)
    {
        int blockCount = beatmap->get_beatmapData()->get_cuttableNotesCount();
        int maxScore;
        if (blockCount < 14)
        {
            if (blockCount == 1)
            {
                maxScore = 115;
            }
            else if (blockCount < 5)
            {
                maxScore = (blockCount - 1) * 230 + 115;
            }
            else
            {
                maxScore = (blockCount - 5) * 460 + 1035;
            }
        }
        else
        {
            maxScore = (blockCount - 13) * 920 + 4715;
        }
        if (maxScore == 0)
            return -1;
        return maxScore;
    }
} // namespace BeatmapUtils