#include "Data/Private/ReplayFile.hpp"
#include <fstream>
#include <sstream>

namespace ScoreSaber::Data::Private::ReplayWriter
{
    std::vector<char> Write(ReplayFile* file);
    int WriteMetadata(Metadata* metadata, stringstream& outputStream);
    int WriteVRPoseGroup(VRPoseGroup vrPoseGroup, stringstream& outputStream);
    int WriteVRPose(VRPose vrPose, stringstream& outputStream);
    int WriteHeightEvent(HeightEvent heightEvent, stringstream& outputStream);
    int WriteNoteEvent(NoteEvent noteEvent, stringstream& outputStream);
    int WriteNoteID(NoteID noteID, stringstream& outputStream);
    int WriteScoreEvent(ScoreEvent scoreEvent, stringstream& outputStream);
    int WriteComboEvent(ComboEvent scoreEvent, stringstream& outputStream);
    int WriteMultiplierEvent(MultiplierEvent multiplierEvent, stringstream& outputStream);
    int WriteEnergyEvent(EnergyEvent energyEvent, stringstream& outputStream);
    int WriteStringArray(vector<string> values, stringstream& outputStream);
    int WriteVRPoseGroupList(vector<VRPoseGroup> values, stringstream& outputStream);
    int WriteHeightChangeList(vector<HeightEvent> values, stringstream& outputStream);
    int WriteNoteEventList(vector<NoteEvent> values, stringstream& outputStream);
    int WriteScoreEventList(vector<ScoreEvent> values, stringstream& outputStream);
    int WriteComboEventList(vector<ComboEvent> values, stringstream& outputStream);
    int WriteMultiplierEventList(vector<MultiplierEvent> values, stringstream& outputStream);
    int WriteEnergyEventList(vector<EnergyEvent> values, stringstream& outputStream);
    int WriteString(string value, stringstream& outputStream);
    int WriteRawString(string value, stringstream& outputStream);
    int WriteInt(int value, stringstream& outputStream);
    int WriteFloat(float value, stringstream& outputStream);
    int WriteBool(bool value, stringstream& outputStream);
    int WriteVRPosition(VRPosition position, stringstream& outputStream);
    int WriteVRRotation(VRRotation rotation, stringstream& outputStream);
} // namespace ScoreSaber::Data::Private::ReplayWriter