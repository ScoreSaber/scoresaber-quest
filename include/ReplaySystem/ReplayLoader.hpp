#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayReader.hpp"
#include "ReplaySystem/Playback/NotePlayer.hpp"

namespace ScoreSaber::ReplaySystem::ReplayLoader
{
    extern ScoreSaber::Data::Private::ReplayFile* LoadedReplay;
    extern ScoreSaber::ReplaySystem::Playback::NotePlayer* NotePlayerInstance;
    extern bool IsPlaying;
    void LoadReplay();
} // namespace ScoreSaber::ReplaySystem::ReplayLoader