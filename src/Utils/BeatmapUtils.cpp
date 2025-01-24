#include "Utils/BeatmapUtils.hpp"
#include <GlobalNamespace/PlayerData.hpp>
#include "logging.hpp"

using namespace GlobalNamespace;

namespace BeatmapUtils
{
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