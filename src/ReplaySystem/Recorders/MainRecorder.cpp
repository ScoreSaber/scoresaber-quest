

#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayWriter.hpp"
#include "ReplaySystem/Recorders/EnergyEventRecorder.hpp"
#include "ReplaySystem/Recorders/HeightEventRecorder.hpp"
#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "ReplaySystem/Recorders/PoseRecorder.hpp"
#include "ReplaySystem/Recorders/ScoreEventRecorder.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "logging.hpp"
using namespace ScoreSaber::ReplaySystem;
namespace ScoreSaber::ReplaySystem::Recorders::MainRecorder
{
    ReplayFile* ExportCurrentReplay()
    {
        Metadata* metadata = Recorders::MetadataRecorder::Export();
        vector<VRPoseGroup> poseKeyFrames = Recorders::PoseRecorder::Export();
        vector<HeightEvent> heightKeyframes = Recorders::HeightEventRecorder::Export();
        vector<NoteEvent> noteKeyframes = Recorders::NoteEventRecorder::Export();
        vector<ScoreEvent> scoreKeyframes = Recorders::ScoreEventRecorder::ExportScoreKeyframes();
        vector<ComboEvent> comboKeyframes = Recorders::ScoreEventRecorder::ExportComboKeyframes();
        vector<MultiplierEvent> multiplierKeyframes = Recorders::ScoreEventRecorder::ExportMultiplierKeyframes();
        vector<EnergyEvent> energyKeyframes = Recorders::EnergyEventRecorder::Export();
        auto replayFile = new ReplayFile(metadata, poseKeyFrames, heightKeyframes, noteKeyframes, scoreKeyframes, comboKeyframes, multiplierKeyframes, energyKeyframes);
        return replayFile;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders::MainRecorder