
#include "ReplaySystem/Recorders/MainRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayWriter.hpp"
#include "ReplaySystem/Recorders/EnergyEventRecorder.hpp"
#include "ReplaySystem/Recorders/HeightEventRecorder.hpp"
#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "ReplaySystem/Recorders/PoseRecorder.hpp"
#include "ReplaySystem/Recorders/ScoreEventRecorder.hpp"
#include "Services/ReplayService.hpp"
#include <beatsaber-hook/shared/utils/hooking.hpp>
#include "logging.hpp"

using namespace ScoreSaber::ReplaySystem;
using namespace ScoreSaber::Services;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders, MainRecorder);

namespace ScoreSaber::ReplaySystem::Recorders
{
    void MainRecorder::ctor(PoseRecorder* poseRecorder, MetadataRecorder* metadataRecorder, NoteEventRecorder* noteEventRecorder, ScoreEventRecorder* scoreEventRecorder, HeightEventRecorder* heightEventRecorder, EnergyEventRecorder* energyEventRecorder)
    {
        INVOKE_CTOR();
        _poseRecorder = poseRecorder;
        _metadataRecorder = metadataRecorder;
        _noteEventRecorder = noteEventRecorder;
        _scoreEventRecorder = scoreEventRecorder;
        _heightEventRecorder = heightEventRecorder;
        _energyEventRecorder = energyEventRecorder;
    }

    void MainRecorder::Initialize() {
        ReplayService::NewPlayStarted(this);
    }

    void MainRecorder::Dispose() {
        
    }

    std::shared_ptr<ReplayFile> MainRecorder::ExportCurrentReplay()
    {
        std::shared_ptr<Metadata> metadata = _metadataRecorder->Export();
        vector<VRPoseGroup> poseKeyFrames = _poseRecorder->Export();
        vector<HeightEvent> heightKeyframes = _heightEventRecorder->Export();
        vector<NoteEvent> noteKeyframes = _noteEventRecorder->Export();
        vector<ScoreEvent> scoreKeyframes = _scoreEventRecorder->ExportScoreKeyframes();
        vector<ComboEvent> comboKeyframes = _scoreEventRecorder->ExportComboKeyframes();
        vector<MultiplierEvent> multiplierKeyframes = _scoreEventRecorder->ExportMultiplierKeyframes();
        vector<EnergyEvent> energyKeyframes = _energyEventRecorder->Export();
        return std::make_shared<ReplayFile>(metadata, poseKeyFrames, heightKeyframes, noteKeyframes, scoreKeyframes, comboKeyframes, multiplierKeyframes, energyKeyframes);
    }

    void MainRecorder::StopRecording() {
        _poseRecorder->StopRecording();
    }
} // namespace ScoreSaber::ReplaySystem::Recorders