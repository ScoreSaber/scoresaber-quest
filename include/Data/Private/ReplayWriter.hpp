#include "Data/Private/ReplayFile.hpp"
#include <fstream>
namespace ScoreSaber::Data::Private::ReplayWriter
{
    void Write(ReplayFile* file);
    int WriteMetadata(Metadata* metadata, ofstream& outputStream);
    int WriteVRPoseGroup(VRPoseGroup vrPoseGroup, ofstream& outputStream);
    int WriteVRPose(VRPose vrPose, ofstream& outputStream);
    int WriteHeightEvent(HeightEvent heightEvent, ofstream& outputStream);
    int WriteNoteEvent(NoteEvent noteEvent, ofstream& outputStream);
    int WriteNoteID(NoteID noteID, ofstream& outputStream);
    int WriteScoreEvent(ScoreEvent scoreEvent, ofstream& outputStream);
    int WriteComboEvent(ComboEvent scoreEvent, ofstream& outputStream);
    int WriteMultiplierEvent(MultiplierEvent multiplierEvent, ofstream& outputStream);
    int WriteEnergyEvent(EnergyEvent energyEvent, ofstream& outputStream);
    int WriteStringArray(vector<string> values, ofstream& outputStream);
    int WriteVRPoseGroupList(vector<VRPoseGroup> values, ofstream& outputStream);
    int WriteHeightChangeList(vector<HeightEvent> values, ofstream& outputStream);
    int WriteNoteEventList(vector<NoteEvent> values, ofstream& outputStream);
    int WriteScoreEventList(vector<ScoreEvent> values, ofstream& outputStream);
    int WriteComboEventList(vector<ComboEvent> values, ofstream& outputStream);
    int WriteMultiplierEventList(vector<MultiplierEvent> values, ofstream& outputStream);
    int WriteEnergyEventList(vector<EnergyEvent> values, ofstream& outputStream);
    int WriteString(string value, ofstream& outputStream);
    int WriteRawString(string value, ofstream& outputStream);
    int WriteInt(int value, ofstream& outputStream);
    int WriteFloat(float value, ofstream& outputStream);
    int WriteBool(bool value, ofstream& outputStream);
    int WriteVRPosition(VRPosition position, ofstream& outputStream);
    int WriteVRRotation(VRRotation rotation, ofstream& outputStream);
} // namespace ScoreSaber::Data::Private::ReplayWriter