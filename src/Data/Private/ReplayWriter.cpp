#include "Data/Private/ReplayWriter.hpp"

#include "Utils/lzma/lzma.hpp"

#include <codecvt>
#include <locale>
#include <stdlib.h>
#include <string>

#include "Services/PlayerService.hpp"

#include "logging.hpp"
#include "static.hpp"

using namespace std;

namespace ScoreSaber::Data::Private::ReplayWriter
{
    int _pointerSize = 38;
    std::vector<char> Write(std::shared_ptr<ReplayFile> file)
    {
        stringstream outputStream;

        int pointerLocation = outputStream.tellp();
        for (int i = 0; i < _pointerSize; i += 4)
        {
            WriteInt(0, outputStream);
        }

        // Write Data
        int metadataPointer = outputStream.tellp();
        WriteMetadata(file->metadata, outputStream);
        int poseKeyframePointer = outputStream.tellp();
        WriteVRPoseGroupList(file->poseKeyframes, outputStream);
        int heightEventsPointer = outputStream.tellp();
        WriteHeightChangeList(file->heightKeyframes, outputStream);
        int noteEventsPointer = outputStream.tellp();
        WriteNoteEventList(file->noteKeyframes, outputStream);
        int scoreEventsPointer = outputStream.tellp();
        WriteScoreEventList(file->scoreKeyframes, outputStream);
        int comboEventsPointer = outputStream.tellp();
        WriteComboEventList(file->comboKeyframes, outputStream);
        int multiplierEventsPointer = outputStream.tellp();
        WriteMultiplierEventList(file->multiplierKeyframes, outputStream);
        int energyEventsPointer = outputStream.tellp();
        WriteEnergyEventList(file->energyKeyframes, outputStream);

        // Write pointers
        outputStream.seekp(pointerLocation);
        WriteInt(metadataPointer, outputStream);
        WriteInt(poseKeyframePointer, outputStream);
        WriteInt(heightEventsPointer, outputStream);
        WriteInt(noteEventsPointer, outputStream);
        WriteInt(scoreEventsPointer, outputStream);
        WriteInt(comboEventsPointer, outputStream);
        WriteInt(multiplierEventsPointer, outputStream);
        WriteInt(energyEventsPointer, outputStream);

        outputStream.seekg(0);
        vector<char> uncompressedReplayBytes((std::istreambuf_iterator<char>(outputStream)), std::istreambuf_iterator<char>());
        std::vector<char> compressed;
        if (LZMA::lzmaCompress(uncompressedReplayBytes, compressed))
        {
            std::string magic = "ScoreSaber Replay 👌🤠\r\n";
            compressed.insert(compressed.begin(), magic.begin(), magic.end());
            return compressed;
        }
        return {};
    }

    int WriteMetadata(std::shared_ptr<Metadata> metadata, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteString(metadata->Version.str(), outputStream);
        bytesWritten += WriteString(metadata->LevelID, outputStream);
        bytesWritten += WriteInt(metadata->Difficulty, outputStream);
        bytesWritten += WriteString(metadata->Characteristic, outputStream);
        bytesWritten += WriteString(metadata->Environment, outputStream);
        bytesWritten += WriteStringArray(metadata->Modifiers, outputStream);
        bytesWritten += WriteFloat(metadata->NoteSpawnOffset, outputStream);
        bytesWritten += WriteBool(metadata->LeftHanded, outputStream);
        bytesWritten += WriteFloat(metadata->InitialHeight, outputStream);
        bytesWritten += WriteFloat(metadata->RoomRotation, outputStream);
        bytesWritten += WriteVRPosition(metadata->RoomCenter, outputStream);
        bytesWritten += WriteFloat(metadata->FailTime, outputStream);
        bytesWritten += WriteString(metadata->GameVersion.value().str(), outputStream);
        bytesWritten += WriteString(metadata->PluginVersion.value().str(), outputStream);
        bytesWritten += WriteString(metadata->Platform.value(), outputStream);
        return bytesWritten;
    }

    int WriteVRPoseGroup(VRPoseGroup vrPoseGroup, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteVRPose(vrPoseGroup.Head, outputStream);
        bytesWritten += WriteVRPose(vrPoseGroup.Left, outputStream);
        bytesWritten += WriteVRPose(vrPoseGroup.Right, outputStream);
        bytesWritten += WriteInt(vrPoseGroup.FPS, outputStream);
        bytesWritten += WriteFloat(vrPoseGroup.Time, outputStream);
        return bytesWritten;
    }

    int WriteVRPose(VRPose vrPose, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteVRPosition(vrPose.Position, outputStream);
        bytesWritten += WriteVRRotation(vrPose.Rotation, outputStream);
        return bytesWritten;
    }

    int WriteHeightEvent(HeightEvent heightEvent, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteFloat(heightEvent.Height, outputStream);
        bytesWritten += WriteFloat(heightEvent.Time, outputStream);
        return bytesWritten;
    }

    int WriteNoteEvent(NoteEvent noteEvent, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteNoteID(noteEvent.TheNoteID, outputStream);
        bytesWritten += WriteInt((int)noteEvent.EventType, outputStream);
        bytesWritten += WriteVRPosition(noteEvent.CutPoint, outputStream);
        bytesWritten += WriteVRPosition(noteEvent.CutNormal, outputStream);
        bytesWritten += WriteVRPosition(noteEvent.SaberDirection, outputStream);
        bytesWritten += WriteInt(noteEvent.SaberType, outputStream);
        bytesWritten += WriteBool(noteEvent.DirectionOK, outputStream);
        bytesWritten += WriteFloat(noteEvent.SaberSpeed, outputStream);
        bytesWritten += WriteFloat(noteEvent.CutAngle, outputStream);
        bytesWritten += WriteFloat(noteEvent.CutDistanceToCenter, outputStream);
        bytesWritten += WriteFloat(noteEvent.CutDirectionDeviation, outputStream);
        bytesWritten += WriteFloat(noteEvent.BeforeCutRating, outputStream);
        bytesWritten += WriteFloat(noteEvent.AfterCutRating, outputStream);
        bytesWritten += WriteFloat(noteEvent.Time, outputStream);
        bytesWritten += WriteFloat(noteEvent.UnityTimescale, outputStream);
        bytesWritten += WriteFloat(noteEvent.TimeSyncTimescale, outputStream);

        bytesWritten += WriteFloat(noteEvent.TimeDeviation.value(), outputStream);
        bytesWritten += WriteVRRotation(noteEvent.WorldRotation.value(), outputStream);
        bytesWritten += WriteVRRotation(noteEvent.InverseWorldRotation.value(), outputStream);
        bytesWritten += WriteVRRotation(noteEvent.NoteRotation.value(), outputStream);
        bytesWritten += WriteVRPosition(noteEvent.NotePosition.value(), outputStream);
        return bytesWritten;
    }

    int WriteNoteID(NoteID noteID, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteFloat(noteID.Time, outputStream);
        bytesWritten += WriteInt(noteID.LineLayer, outputStream);
        bytesWritten += WriteInt(noteID.LineIndex, outputStream);
        bytesWritten += WriteInt(noteID.ColorType, outputStream);
        bytesWritten += WriteInt(noteID.CutDirection, outputStream);
        bytesWritten += WriteInt(noteID.GameplayType.value(), outputStream);
        bytesWritten += WriteInt(noteID.ScoringType.value(), outputStream);
        bytesWritten += WriteFloat(noteID.CutDirectionAngleOffset.value(), outputStream);
        return bytesWritten;
    }

    int WriteScoreEvent(ScoreEvent scoreEvent, stringstream& outputStream)
    {

        int bytesWritten = 0;
        bytesWritten += WriteInt(scoreEvent.Score, outputStream);
        bytesWritten += WriteFloat(scoreEvent.Time, outputStream);
        bytesWritten += WriteInt(scoreEvent.ImmediateMaxPossibleScore.value(), outputStream);
        return bytesWritten;
    }

    int WriteComboEvent(ComboEvent scoreEvent, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(scoreEvent.Combo, outputStream);
        bytesWritten += WriteFloat(scoreEvent.Time, outputStream);
        return bytesWritten;
    }

    int WriteMultiplierEvent(MultiplierEvent multiplierEvent, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(multiplierEvent.Multiplier, outputStream);
        bytesWritten += WriteFloat(multiplierEvent.NextMultiplierProgress, outputStream);
        bytesWritten += WriteFloat(multiplierEvent.Time, outputStream);
        return bytesWritten;
    }

    int WriteEnergyEvent(EnergyEvent energyEvent, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteFloat(energyEvent.Energy, outputStream);
        bytesWritten += WriteFloat(energyEvent.Time, outputStream);
        return bytesWritten;
    }

    // Lists
    int WriteStringArray(vector<string> values, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(values.size(), outputStream);
        for (int i = 0; i < values.size(); i++)
        {
            bytesWritten += WriteString(values[i], outputStream);
        }
        return bytesWritten;
    }

    int WriteVRPoseGroupList(vector<VRPoseGroup> values, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(values.size(), outputStream);
        for (int i = 0; i < values.size(); i++)
        {
            bytesWritten += WriteVRPoseGroup(values[i], outputStream);
        }
        return bytesWritten;
    }

    int WriteHeightChangeList(vector<HeightEvent> values, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(values.size(), outputStream);
        for (int i = 0; i < values.size(); i++)
        {
            bytesWritten += WriteHeightEvent(values[i], outputStream);
        }
        return bytesWritten;
    }

    int WriteNoteEventList(vector<NoteEvent> values, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(values.size(), outputStream);
        for (int i = 0; i < values.size(); i++)
        {
            bytesWritten += WriteNoteEvent(values[i], outputStream);
        }
        return bytesWritten;
    }

    int WriteScoreEventList(vector<ScoreEvent> values, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(values.size(), outputStream);
        for (int i = 0; i < values.size(); i++)
        {
            bytesWritten += WriteScoreEvent(values[i], outputStream);
        }
        return bytesWritten;
    }

    int WriteComboEventList(vector<ComboEvent> values, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(values.size(), outputStream);
        for (int i = 0; i < values.size(); i++)
        {
            bytesWritten += WriteComboEvent(values[i], outputStream);
        }
        return bytesWritten;
    }

    int WriteMultiplierEventList(vector<MultiplierEvent> values, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(values.size(), outputStream);
        for (int i = 0; i < values.size(); i++)
        {
            bytesWritten += WriteMultiplierEvent(values[i], outputStream);
        }
        return bytesWritten;
    }

    int WriteEnergyEventList(vector<EnergyEvent> values, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteInt(values.size(), outputStream);
        for (int i = 0; i < values.size(); i++)
        {
            bytesWritten += WriteEnergyEvent(values[i], outputStream);
        }
        return bytesWritten;
    }

    // Primitives
    int WriteString(string value, stringstream& outputStream)
    {
        int bytesWritten = 0;
        const char* cString = value.c_str();
        size_t stringLength = strlen(cString);

        bytesWritten += WriteInt((int)stringLength, outputStream);
        outputStream.write(cString, stringLength);
        bytesWritten += stringLength;
        return bytesWritten;
    }

    int WriteRawString(string value, stringstream& outputStream)
    {
        int bytesWritten = 0;
        const char* cString = value.c_str();
        size_t stringLength = strlen(cString);

        outputStream.write(cString, stringLength);
        bytesWritten += stringLength;
        return bytesWritten;
    }

    int WriteInt(int value, stringstream& outputStream)
    {
        outputStream.write(reinterpret_cast<const char*>(&value), sizeof(int));
        return 4;
    }

    int WriteFloat(float value, stringstream& outputStream)
    {
        outputStream.write(reinterpret_cast<const char*>(&value), sizeof(float));
        return 4;
    }

    int WriteBool(bool value, stringstream& outputStream)
    {
        outputStream.write(reinterpret_cast<const char*>(&value), sizeof(bool));
        return 1;
    }

    int WriteVRPosition(VRPosition position, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteFloat(position.X, outputStream);
        bytesWritten += WriteFloat(position.Y, outputStream);
        bytesWritten += WriteFloat(position.Z, outputStream);
        return bytesWritten;
    }

    int WriteVRRotation(VRRotation rotation, stringstream& outputStream)
    {
        int bytesWritten = 0;
        bytesWritten += WriteFloat(rotation.X, outputStream);
        bytesWritten += WriteFloat(rotation.Y, outputStream);
        bytesWritten += WriteFloat(rotation.Z, outputStream);
        bytesWritten += WriteFloat(rotation.W, outputStream);
        return bytesWritten;
    }

} // namespace ScoreSaber::Data::Private::ReplayWriter
