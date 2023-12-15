#include "Services/ReplayService.hpp"

#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayWriter.hpp"
#include "ReplaySystem/Recorders/MainRecorder.hpp"

using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::Services::ReplayService
{
    vector<char> CurrentSerializedReplay;
    function<void(const vector<char>&)> ReplaySerialized;
    ReplaySystem::Recorders::MainRecorder* recorder;

    void OnSoftRestart() {
        CurrentSerializedReplay.clear();
        ReplaySerialized = nullptr;
        recorder = nullptr;
    }

    void NewPlayStarted(ReplaySystem::Recorders::MainRecorder* _recorder) {
        recorder = _recorder;
    }

    void WriteSerializedReplay()
    {
        recorder->StopRecording();
        auto replay = recorder->ExportCurrentReplay();
        CurrentSerializedReplay = ScoreSaber::Data::Private::ReplayWriter::Write(replay);
        if (ReplaySerialized != nullptr)
            ReplaySerialized(CurrentSerializedReplay);
    }
}