#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayReader.hpp"

namespace ScoreSaber::ReplaySystem::ReplayLoader
{
    extern ScoreSaber::Data::Private::ReplayFile* LoadedReplay;
    extern bool IsPlaying;
    void LoadReplay();
} // namespace ScoreSaber::ReplaySystem::ReplayLoader