#include "ReplaySystem/ReplayLoader.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayReader.hpp"
#include "ReplaySystem/Playback/NotePlayer.hpp"

namespace ScoreSaber::ReplaySystem::ReplayLoader
{
    ScoreSaber::Data::Private::ReplayFile* LoadedReplay;
    ScoreSaber::ReplaySystem::Playback::NotePlayer* NotePlayerInstance;
    bool IsPlaying;
    void LoadReplay()
    {
        LoadedReplay = ScoreSaber::Data::Private::ReplayReader::Read(
            "76561198283584459-eden-Hard-Standard-64688EE557A6C0916D2108AA3B39DD79C8468DB3.dat");
    }
} // namespace ScoreSaber::ReplaySystem::ReplayLoader