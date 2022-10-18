#include "Data/Private/ReplayFile.hpp"
#include <fstream>

namespace ScoreSaber::Data::Private::ReplayReader
{
    ReplayFile* Read(std::string fileName);
    Pointers ReadPointers(ifstream& inputStream);
    Metadata* ReadMetadata(ifstream& inputStream, int offset);
    vector<VRPoseGroup> ReadPoseKeyframes(ifstream& inputStream, int offset);
    vector<HeightEvent> ReadHeightKeyframes(ifstream& inputStream, int offset);
    vector<NoteEvent> ReadNoteKeyframes(ifstream& inputStream, int offset);
    vector<ScoreEvent> ReadScoreKeyframes(ifstream& inputStream, int offset);
    vector<ComboEvent> ReadComboKeyframes(ifstream& inputStream, int offset);
    vector<MultiplierEvent> ReadMultiplierKeyframes(ifstream& inputStream, int offset);
    vector<EnergyEvent> ReadEnergyKeyframes(ifstream& inputStream, int offset);
    VRPosition ReadVRPosition(ifstream& inputStream);
    VRRotation ReadVRRotation(ifstream& inputStream);
    VRPoseGroup ReadVRPoseGroup(ifstream& inputStream);
    VRPose ReadVRPose(ifstream& inputStream);
    VRPose ReadVRPose(ifstream& inputStream);
    HeightEvent ReadHeightEvent(ifstream& inputStream);
    NoteEvent ReadNoteEvent(ifstream& inputStream);
    NoteID ReadNoteID(ifstream& inputStream);
    ScoreEvent ReadScoreEvent(ifstream& inputStream);
    ComboEvent ReadComboEvent(ifstream& inputStream);
    MultiplierEvent ReadMultiplierEvent(ifstream& inputStream);
    EnergyEvent ReadEnergyEvent(ifstream& inputStream);
    int ReadInt(ifstream& inputStream);
    std::string ReadString(ifstream& inputStream);
    std::vector<string> ReadStringArray(ifstream& inputStream);
    float ReadFloat(ifstream& inputStream);
    bool ReadBool(ifstream& inputStream);
    std::string DecompressReplay(std::string fileName);
} // namespace ScoreSaber::Data::Private::ReplayReader