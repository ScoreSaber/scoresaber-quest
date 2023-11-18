#include "Data/Private/ReplayReader.hpp"

#include "Utils/lzma/lzma.hpp"

#include <codecvt>
#include <locale>
#include <stdlib.h>
#include <string>

#include "Services/PlayerService.hpp"

#include "logging.hpp"
#include "static.hpp"

using namespace std;

namespace ScoreSaber::Data::Private::ReplayReader
{
    shared_ptr<ReplayFile> Read(const vector<char> &replayData)
    {
        std::vector<char> decompressed;
        if(!DecompressReplay(replayData, decompressed)) {
            return nullptr;
        }
        stringstream inputStream;
        inputStream.write(decompressed.data(), decompressed.size());
        Pointers pointers = ReadPointers(inputStream);

        shared_ptr<Metadata> metadata = ReadMetadata(inputStream, pointers.metadata);

        if (metadata->Version == "2.0.0") {
            return make_shared<ReplayFile>(metadata,
                                           ReadPoseKeyframes(inputStream, pointers.poseKeyframes),
                                           ReadHeightKeyframes(inputStream, pointers.heightKeyframes),
                                           ReadNoteKeyframes(inputStream, pointers.noteKeyframes),
                                           ReadScoreKeyframes(inputStream, pointers.scoreKeyframes),
                                           ReadComboKeyframes(inputStream, pointers.comboKeyframes),
                                           ReadMultiplierKeyframes(inputStream, pointers.multiplierKeyframes),
                                           ReadEnergyKeyframes(inputStream, pointers.energyKeyframes));
        } else if (metadata->Version == "3.0.0") {
            return make_shared<ReplayFile>(metadata,
                                           ReadPoseKeyframes(inputStream, pointers.poseKeyframes),
                                           ReadHeightKeyframes(inputStream, pointers.heightKeyframes),
                                           ReadNoteKeyframes_v3(inputStream, pointers.noteKeyframes),
                                           ReadScoreKeyframes_v3(inputStream, pointers.scoreKeyframes),
                                           ReadComboKeyframes(inputStream, pointers.comboKeyframes),
                                           ReadMultiplierKeyframes(inputStream, pointers.multiplierKeyframes),
                                           ReadEnergyKeyframes(inputStream, pointers.energyKeyframes));
        } else {
            return nullptr;
        }
    }

    Pointers ReadPointers(stringstream& inputStream)
    {
        inputStream.seekg(0);
        return Pointers(ReadInt(inputStream), ReadInt(inputStream),
                        ReadInt(inputStream), ReadInt(inputStream),
                        ReadInt(inputStream), ReadInt(inputStream),
                        ReadInt(inputStream), ReadInt(inputStream),
                        ReadInt(inputStream));
    }

    shared_ptr<Metadata> ReadMetadata(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        return make_shared<Metadata>(ReadString(inputStream), ReadString(inputStream), ReadInt(inputStream), ReadString(inputStream),
                            ReadString(inputStream), ReadStringArray(inputStream), ReadFloat(inputStream), ReadBool(inputStream),
                            ReadFloat(inputStream), ReadFloat(inputStream), ReadVRPosition(inputStream), ReadFloat(inputStream));
    }

    vector<VRPoseGroup> ReadPoseKeyframes(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        int count = ReadInt(inputStream);
        vector<VRPoseGroup> poseKeyframes = vector<VRPoseGroup>();
        for (int i = 0; i < count; i++)
        {
            poseKeyframes.push_back(ReadVRPoseGroup(inputStream));
        }
        return poseKeyframes;
    }

    vector<HeightEvent> ReadHeightKeyframes(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        int count = ReadInt(inputStream);
        vector<HeightEvent> heightKeyframes = vector<HeightEvent>();
        for (int i = 0; i < count; i++)
        {
            heightKeyframes.push_back(ReadHeightEvent(inputStream));
        }
        return heightKeyframes;
    }

    vector<NoteEvent> ReadNoteKeyframes(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        int count = ReadInt(inputStream);
        vector<NoteEvent> noteKeyframes = vector<NoteEvent>();
        for (int i = 0; i < count; i++)
        {
            noteKeyframes.push_back(ReadNoteEvent(inputStream));
        }
        return noteKeyframes;
    }

    vector<NoteEvent> ReadNoteKeyframes_v3(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        int count = ReadInt(inputStream);
        vector<NoteEvent> noteKeyframes = vector<NoteEvent>();
        for (int i = 0; i < count; i++)
        {
            noteKeyframes.push_back(ReadNoteEvent_v3(inputStream));
        }
        return noteKeyframes;
    }

    vector<ScoreEvent> ReadScoreKeyframes(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        int count = ReadInt(inputStream);
        vector<ScoreEvent> scoreKeyframes = vector<ScoreEvent>();
        for (int i = 0; i < count; i++)
        {
            scoreKeyframes.push_back(ReadScoreEvent(inputStream));
        }
        return scoreKeyframes;
    }

    vector<ScoreEvent> ReadScoreKeyframes_v3(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        int count = ReadInt(inputStream);
        vector<ScoreEvent> scoreKeyframes = vector<ScoreEvent>();
        for (int i = 0; i < count; i++)
        {
            scoreKeyframes.push_back(ReadScoreEvent_v3(inputStream));
        }
        return scoreKeyframes;
    }

    vector<ComboEvent> ReadComboKeyframes(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        int count = ReadInt(inputStream);
        vector<ComboEvent> comboKeyframes = vector<ComboEvent>();
        for (int i = 0; i < count; i++)
        {
            comboKeyframes.push_back(ReadComboEvent(inputStream));
        }
        return comboKeyframes;
    }

    vector<MultiplierEvent> ReadMultiplierKeyframes(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        int count = ReadInt(inputStream);
        vector<MultiplierEvent> multiplierKeyframes = vector<MultiplierEvent>();
        for (int i = 0; i < count; i++)
        {
            multiplierKeyframes.push_back(ReadMultiplierEvent(inputStream));
        }
        return multiplierKeyframes;
    }

    vector<EnergyEvent> ReadEnergyKeyframes(stringstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        int count = ReadInt(inputStream);
        vector<EnergyEvent> energyKeyframes = vector<EnergyEvent>();
        for (int i = 0; i < count; i++)
        {
            energyKeyframes.push_back(ReadEnergyEvent(inputStream));
        }
        return energyKeyframes;
    }

    VRPosition ReadVRPosition(stringstream& inputStream)
    {
        return VRPosition(ReadFloat(inputStream), ReadFloat(inputStream),
                          ReadFloat(inputStream));
    }

    VRRotation ReadVRRotation(stringstream& inputStream)
    {
        return VRRotation(ReadFloat(inputStream), ReadFloat(inputStream),
                          ReadFloat(inputStream), ReadFloat(inputStream));
    }

    VRPoseGroup ReadVRPoseGroup(stringstream& inputStream)
    {
        return VRPoseGroup(ReadVRPose(inputStream), ReadVRPose(inputStream), ReadVRPose(inputStream), ReadInt(inputStream), ReadFloat(inputStream));
    }

    VRPose ReadVRPose(stringstream& inputStream)
    {
        return VRPose(ReadVRPosition(inputStream), ReadVRRotation(inputStream));
    }

    HeightEvent ReadHeightEvent(stringstream& inputStream)
    {
        return HeightEvent(ReadFloat(inputStream), ReadFloat(inputStream));
    }

    NoteEvent ReadNoteEvent(stringstream& inputStream)
    {
        return NoteEvent(ReadNoteID(inputStream), (NoteEventType)ReadInt(inputStream), ReadVRPosition(inputStream), ReadVRPosition(inputStream),
                         ReadVRPosition(inputStream), ReadInt(inputStream), ReadBool(inputStream), ReadFloat(inputStream), ReadFloat(inputStream),
                         ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream),
                         ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream));
    }

    NoteEvent ReadNoteEvent_v3(stringstream& inputStream)
    {
        return NoteEvent(ReadNoteID_v3(inputStream), (NoteEventType)ReadInt(inputStream), ReadVRPosition(inputStream), ReadVRPosition(inputStream),
                         ReadVRPosition(inputStream), ReadInt(inputStream), ReadBool(inputStream), ReadFloat(inputStream), ReadFloat(inputStream),
                         ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream),
                         ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream),
                         ReadVRRotation(inputStream), ReadVRRotation(inputStream), ReadVRRotation(inputStream), ReadVRPosition(inputStream));
    }

    NoteID ReadNoteID(stringstream& inputStream)
    {
        return NoteID(ReadFloat(inputStream), ReadInt(inputStream), ReadInt(inputStream), ReadInt(inputStream), ReadInt(inputStream));
    }

    NoteID ReadNoteID_v3(stringstream& inputStream)
    {
        return NoteID(ReadFloat(inputStream), ReadInt(inputStream), ReadInt(inputStream), ReadInt(inputStream), ReadInt(inputStream), ReadInt(inputStream), ReadInt(inputStream), ReadFloat(inputStream));
    }

    ScoreEvent ReadScoreEvent(stringstream& inputStream)
    {
        return ScoreEvent(ReadInt(inputStream), ReadFloat(inputStream));
    }
    ScoreEvent ReadScoreEvent_v3(stringstream& inputStream)
    {
        return ScoreEvent(ReadInt(inputStream), ReadFloat(inputStream), ReadInt(inputStream));
    }

    ComboEvent ReadComboEvent(stringstream& inputStream)
    {
        return ComboEvent(ReadInt(inputStream), ReadFloat(inputStream));
    }

    MultiplierEvent ReadMultiplierEvent(stringstream& inputStream)
    {
        return MultiplierEvent(ReadInt(inputStream), ReadFloat(inputStream), ReadFloat(inputStream));
    }

    EnergyEvent ReadEnergyEvent(stringstream& inputStream)
    {
        return EnergyEvent(ReadFloat(inputStream), ReadFloat(inputStream));
    }

    // Primitives

    int ReadInt(stringstream& inputStream)
    {
        int value = 0;
        inputStream.read((char*)&value, sizeof(int));
        return value;
    }

    std::string ReadString(stringstream& inputStream)
    {
        size_t stringLength = (size_t)ReadInt(inputStream);
        std::string value;
        value.resize(stringLength);
        inputStream.read(value.data(), stringLength);
        return value;
    }

    std::vector<string> ReadStringArray(stringstream& inputStream)
    {
        int length = ReadInt(inputStream);
        std::vector<string> value = std::vector<string>();
        for (int i = 0; i < length; i++)
        {
            value.push_back(ReadString(inputStream));
        }
        return value;
    }

    float ReadFloat(stringstream& inputStream)
    {
        float value = 0;
        inputStream.read((char*)&value, sizeof(float));
        return value;
    }

    bool ReadBool(stringstream& inputStream)
    {
        bool value = 0;
        inputStream.read((char*)&value, sizeof(bool));
        return value;
    }

    bool DecompressReplay(const std::vector<char> &replay, std::vector<char> &decompressed)
    {
        if(replay[0] == (char)93 && replay[1] == 0 && replay[2] == 0 && replay[3] == (char)128) {
            ERROR("Can't load legacy replays");
            return false; // legacy replay
        }

        string magic = "ScoreSaber Replay 👌🤠\r\n";
        if (replay.size() < 28 || string(replay.begin(), replay.begin() + 28) != magic) {
            return false; // invalid magic
        }

        // remove magic bytes
        vector<char> compressedReplayBytes(replay.begin() + 28, replay.end());

        return LZMA::lzmaDecompress(compressedReplayBytes, decompressed);
    }

} // namespace ScoreSaber::Data::Private::ReplayReader
