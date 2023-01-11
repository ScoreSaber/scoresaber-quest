#include "Services/ReplayService.hpp"

#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayWriter.hpp"
#include "ReplaySystem/Recorders/MainRecorder.hpp"

namespace ScoreSaber::Services::ReplayService
{
    std::vector<char> CurrentSerializedReplay;
    std::function<void(const std::vector<char>&)> ReplaySerialized;

    void WriteSerializedReplay()
    {
        ReplayFile* replay = Recorders::MainRecorder::ExportCurrentReplay();
        CurrentSerializedReplay = ScoreSaber::Data::Private::ReplayWriter::Write(replay);
        if (ReplaySerialized != nullptr)
            ReplaySerialized(CurrentSerializedReplay);
    }
}