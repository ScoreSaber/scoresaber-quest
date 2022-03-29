

#include "Data/Private/ReplayFile.hpp"
#include "Data/Private/ReplayWriter.hpp"
#include "ReplaySystem/Recorders/EnergyEventRecorder.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "logging.hpp"
using namespace ScoreSaber::ReplaySystem;
namespace ScoreSaber::ReplaySystem::Recorders::MainRecorder
{
    ScoreSaber::Data::Private::ReplayFile* ExportCurrentReplay();
} // namespace ScoreSaber::ReplaySystem::Recorders