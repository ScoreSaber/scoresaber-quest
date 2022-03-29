

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
        INFO("metadata");
        Metadata* metadata = Recorders::MetadataRecorder::Export();
        INFO("poseKeyFrames");
        vector<VRPoseGroup> poseKeyFrames = Recorders::PoseRecorder::Export();
        INFO("heightKeyframes");
        vector<HeightEvent> heightKeyframes = Recorders::HeightEventRecorder::Export();
        INFO("noteKeyframes");
        vector<NoteEvent> noteKeyframes = Recorders::NoteEventRecorder::Export();
        INFO("scoreKeyframes");
        vector<ScoreEvent> scoreKeyframes = Recorders::ScoreEventRecorder::ExportScoreKeyframes();
        INFO("comboKeyframes");
        vector<ComboEvent> comboKeyframes = Recorders::ScoreEventRecorder::ExportComboKeyframes();
        INFO("multiplierKeyframes");
        vector<MultiplierEvent> multiplierKeyframes = Recorders::ScoreEventRecorder::ExportMultiplierKeyframes();
        INFO("energyKeyframes");
        vector<EnergyEvent> energyKeyframes = Recorders::EnergyEventRecorder::Export();
        INFO("replayFile");
        auto replayFile = new ReplayFile(metadata, poseKeyFrames, heightKeyframes, noteKeyframes, scoreKeyframes, comboKeyframes, multiplierKeyframes, energyKeyframes);
        INFO("ScoreSaber::Data::Private::ReplayWriter::Write(replayFile);");
        ScoreSaber::Data::Private::ReplayWriter::Write(replayFile);
        return replayFile;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders