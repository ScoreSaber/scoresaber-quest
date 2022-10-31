#include "Data/Private/ReplayReader.hpp"

extern "C"
{
#include "Utils/lzma/simple.h"
}

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
    ReplayFile* Read(std::string absolutePath, std::string fileName)
    {
        std::string decompressedPath = DecompressReplay(absolutePath, fileName);
        if (decompressedPath == "")
        {
            return nullptr;
        }
        ifstream inputStream = ifstream(decompressedPath, ios::binary);
        std::locale utf8_locale(std::locale(), new codecvt_utf8<char16_t>);
        inputStream.imbue(utf8_locale);
        Pointers pointers = ReadPointers(inputStream);

        ReplayFile* file = new ReplayFile(ReadMetadata(inputStream, pointers.metadata),
                                          ReadPoseKeyframes(inputStream, pointers.poseKeyframes),
                                          ReadHeightKeyframes(inputStream, pointers.heightKeyframes),
                                          ReadNoteKeyframes(inputStream, pointers.noteKeyframes),
                                          ReadScoreKeyframes(inputStream, pointers.scoreKeyframes),
                                          ReadComboKeyframes(inputStream, pointers.comboKeyframes),
                                          ReadMultiplierKeyframes(inputStream, pointers.multiplierKeyframes),
                                          ReadEnergyKeyframes(inputStream, pointers.energyKeyframes));

        unlink(decompressedPath.c_str());
        return file;
    }

    Pointers ReadPointers(ifstream& inputStream)
    {
        inputStream.seekg(0);
        return Pointers(ReadInt(inputStream), ReadInt(inputStream),
                        ReadInt(inputStream), ReadInt(inputStream),
                        ReadInt(inputStream), ReadInt(inputStream),
                        ReadInt(inputStream), ReadInt(inputStream),
                        ReadInt(inputStream));
    }

    Metadata* ReadMetadata(ifstream& inputStream, int offset)
    {
        inputStream.seekg(offset);
        return new Metadata(ReadString(inputStream), ReadString(inputStream), ReadInt(inputStream), ReadString(inputStream),
                            ReadString(inputStream), ReadStringArray(inputStream), ReadFloat(inputStream), ReadBool(inputStream),
                            ReadFloat(inputStream), ReadFloat(inputStream), ReadVRPosition(inputStream), ReadFloat(inputStream));
    }

    vector<VRPoseGroup> ReadPoseKeyframes(ifstream& inputStream, int offset)
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

    vector<HeightEvent> ReadHeightKeyframes(ifstream& inputStream, int offset)
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

    vector<NoteEvent> ReadNoteKeyframes(ifstream& inputStream, int offset)
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

    vector<ScoreEvent> ReadScoreKeyframes(ifstream& inputStream, int offset)
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

    vector<ComboEvent> ReadComboKeyframes(ifstream& inputStream, int offset)
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

    vector<MultiplierEvent> ReadMultiplierKeyframes(ifstream& inputStream, int offset)
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

    vector<EnergyEvent> ReadEnergyKeyframes(ifstream& inputStream, int offset)
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

    VRPosition ReadVRPosition(ifstream& inputStream)
    {
        return VRPosition(ReadFloat(inputStream), ReadFloat(inputStream),
                          ReadFloat(inputStream));
    }

    VRRotation ReadVRRotation(ifstream& inputStream)
    {
        return VRRotation(ReadFloat(inputStream), ReadFloat(inputStream),
                          ReadFloat(inputStream), ReadFloat(inputStream));
    }

    VRPoseGroup ReadVRPoseGroup(ifstream& inputStream)
    {
        return VRPoseGroup(ReadVRPose(inputStream), ReadVRPose(inputStream), ReadVRPose(inputStream), ReadInt(inputStream), ReadFloat(inputStream));
    }

    VRPose ReadVRPose(ifstream& inputStream)
    {
        return VRPose(ReadVRPosition(inputStream), ReadVRRotation(inputStream));
    }

    HeightEvent ReadHeightEvent(ifstream& inputStream)
    {
        return HeightEvent(ReadFloat(inputStream), ReadFloat(inputStream));
    }

    NoteEvent ReadNoteEvent(ifstream& inputStream)
    {
        return NoteEvent(ReadNoteID(inputStream), (NoteEventType)ReadInt(inputStream), ReadVRPosition(inputStream), ReadVRPosition(inputStream),
                         ReadVRPosition(inputStream), ReadInt(inputStream), ReadBool(inputStream), ReadFloat(inputStream), ReadFloat(inputStream),
                         ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream),
                         ReadFloat(inputStream), ReadFloat(inputStream), ReadFloat(inputStream));
    }

    NoteID ReadNoteID(ifstream& inputStream)
    {
        return NoteID(ReadFloat(inputStream), ReadInt(inputStream), ReadInt(inputStream), ReadInt(inputStream), ReadInt(inputStream));
    }

    ScoreEvent ReadScoreEvent(ifstream& inputStream)
    {
        return ScoreEvent(ReadInt(inputStream), ReadFloat(inputStream));
    }

    ComboEvent ReadComboEvent(ifstream& inputStream)
    {
        return ComboEvent(ReadInt(inputStream), ReadFloat(inputStream));
    }

    MultiplierEvent ReadMultiplierEvent(ifstream& inputStream)
    {
        return MultiplierEvent(ReadInt(inputStream), ReadFloat(inputStream), ReadFloat(inputStream));
    }

    EnergyEvent ReadEnergyEvent(ifstream& inputStream)
    {
        return EnergyEvent(ReadFloat(inputStream), ReadFloat(inputStream));
    }

    // Primitives

    int ReadInt(ifstream& inputStream)
    {
        int value = 0;
        inputStream.read((char*)&value, sizeof(int));
        return value;
    }

    std::string ReadString(ifstream& inputStream)
    {
        size_t stringLength = (size_t)ReadInt(inputStream);
        std::string value;
        value.resize(stringLength);
        inputStream.read(value.data(), stringLength);
        return value;
    }

    std::vector<string> ReadStringArray(ifstream& inputStream)
    {
        int length = ReadInt(inputStream);
        std::vector<string> value = std::vector<string>();
        for (int i = 0; i < length; i++)
        {
            value.push_back(ReadString(inputStream));
        }
        return value;
    }

    float ReadFloat(ifstream& inputStream)
    {
        float value = 0;
        inputStream.read((char*)&value, sizeof(float));
        return value;
    }

    bool ReadBool(ifstream& inputStream)
    {
        bool value = 0;
        inputStream.read((char*)&value, sizeof(bool));
        return value;
    }

    std::string DecompressReplay(std::string absolutePath, std::string fileName)
    {
        std::string tmpDecompressedPath = ScoreSaber::Static::REPLAY_TMP_DIR + "/" + fileName + ".decompressed.tmp";

        // Read tmp replay file into memory
        ifstream readStream(absolutePath, ios_base::in | ios::binary);
        vector<unsigned char> compressedReplayBytes;
        unsigned char currentChar = readStream.get();

        int position = 0;
        while (readStream.good())
        {
            if (position > 27)
            {
                compressedReplayBytes.push_back(currentChar);
            }
            position++;
            currentChar = readStream.get();
        }

        size_t compressedSize = compressedReplayBytes.size();
        unsigned char* compressed = compressedReplayBytes.data();

        // Compress replay file
        unsigned char* uncompressed;
        size_t sz;

        int result = simpleDecompress(ELZMA_lzma, compressed, compressedSize, &uncompressed, &sz);
        if (result == ELZMA_E_OK)
        {
            ofstream tmpUncompressedOutputStream = ofstream(tmpDecompressedPath, ios::binary);
            std::locale utf8_locale(std::locale(), new codecvt_utf8<char16_t>);
            tmpUncompressedOutputStream.imbue(utf8_locale);
            tmpUncompressedOutputStream.write(reinterpret_cast<char*>(uncompressed), sz);
            tmpUncompressedOutputStream.flush();
            tmpUncompressedOutputStream.close();
            return tmpDecompressedPath;
        }
        return "";
    }

} // namespace ScoreSaber::Data::Private::ReplayReader
