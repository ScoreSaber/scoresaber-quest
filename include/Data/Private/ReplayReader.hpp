#include "Data/Private/ReplayFile.hpp"
#include <fstream>
#include <sstream>

namespace ScoreSaber::Data::Private::ReplayReader
{
    std::shared_ptr<ReplayFile> Read(const vector<char> &replayData);
    Pointers ReadPointers(stringstream& inputStream);
    std::shared_ptr<Metadata> ReadMetadata(stringstream& inputStream, int offset);
    vector<VRPoseGroup> ReadPoseKeyframes(stringstream& inputStream, int offset);
    vector<HeightEvent> ReadHeightKeyframes(stringstream& inputStream, int offset);
    vector<NoteEvent> ReadNoteKeyframes(stringstream& inputStream, int offset);
    vector<ScoreEvent> ReadScoreKeyframes(stringstream& inputStream, int offset);
    vector<ComboEvent> ReadComboKeyframes(stringstream& inputStream, int offset);
    vector<MultiplierEvent> ReadMultiplierKeyframes(stringstream& inputStream, int offset);
    vector<EnergyEvent> ReadEnergyKeyframes(stringstream& inputStream, int offset);
    VRPosition ReadVRPosition(stringstream& inputStream);
    VRRotation ReadVRRotation(stringstream& inputStream);
    VRPoseGroup ReadVRPoseGroup(stringstream& inputStream);
    VRPose ReadVRPose(stringstream& inputStream);
    VRPose ReadVRPose(stringstream& inputStream);
    HeightEvent ReadHeightEvent(stringstream& inputStream);
    NoteEvent ReadNoteEvent(stringstream& inputStream);
    NoteID ReadNoteID(stringstream& inputStream);
    ScoreEvent ReadScoreEvent(stringstream& inputStream);
    ComboEvent ReadComboEvent(stringstream& inputStream);
    MultiplierEvent ReadMultiplierEvent(stringstream& inputStream);
    EnergyEvent ReadEnergyEvent(stringstream& inputStream);
    int ReadInt(stringstream& inputStream);
    std::string ReadString(stringstream& inputStream);
    std::vector<string> ReadStringArray(stringstream& inputStream);
    float ReadFloat(stringstream& inputStream);
    bool ReadBool(stringstream& inputStream);
    bool DecompressReplay(const std::vector<char> &replay, std::vector<char> &decompressed);
} // namespace ScoreSaber::Data::Private::ReplayReader