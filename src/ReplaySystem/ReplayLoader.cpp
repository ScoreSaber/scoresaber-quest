#include "ReplaySystem/ReplayLoader.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayReader.hpp"

namespace ScoreSaber::ReplaySystem::ReplayLoader
{
    ScoreSaber::Data::Private::ReplayFile* LoadedReplay;
    bool IsPlaying;
    void LoadReplay()
    {
        LoadedReplay = ScoreSaber::Data::Private::ReplayReader::Read(
            "76561198136159568-Sjeng Oan de Geng-Hard-Standard-C54227D4DD7AD43EFDF437D3B9B4C290FC98D247.dat");
    }
} // namespace ScoreSaber::ReplaySystem::ReplayLoader