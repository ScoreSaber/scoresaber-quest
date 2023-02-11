#include "Data/Private/ReplayFile.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Vector3.hpp"

namespace ScoreSaber::Data::Private
{

    UnityEngine::Vector3 VRVector3(Data::Private::VRPosition pose)
    {
        return UnityEngine::Vector3(pose.X, pose.Y, pose.Z);
    }

    UnityEngine::Quaternion VRQuaternion(Data::Private::VRRotation rotation)
    {
        return UnityEngine::Quaternion(rotation.X, rotation.Y, rotation.Z, rotation.W);
    }

    Pointers::Pointers(int _metadata, int _poseKeyframes, int _heightKeyframes, int _noteKeyframes, int _scoreKeyframes, int _comboKeyframes, int _multiplierKeyframes, int _energyKeyframes, int _fpsKeyframes)
    {
        metadata = _metadata;
        poseKeyframes = _poseKeyframes;
        heightKeyframes = _heightKeyframes;
        noteKeyframes = _noteKeyframes;
        scoreKeyframes = _scoreKeyframes;
        comboKeyframes = _comboKeyframes;
        multiplierKeyframes = _multiplierKeyframes;
        energyKeyframes = _energyKeyframes;
        fpsKeyframes = _fpsKeyframes;
    }

    Pointers::Pointers(){};

    VRPosition::VRPosition(float _X, float _Y, float _Z)
    {
        X = _X;
        Y = _Y;
        Z = _Z;
    }

    VRPosition::VRPosition(UnityEngine::Vector3 vector)
    {
        X = vector.x;
        Y = vector.y;
        Z = vector.z;
    }

    VRPosition::VRPosition(){};

    VRRotation::VRRotation(float _X, float _Y, float _Z, float _W)
    {
        X = _X;
        Y = _Y;
        Z = _Z;
        W = _W;
    }

    VRRotation::VRRotation(UnityEngine::Quaternion quaternion)
    {
        X = quaternion.x;
        Y = quaternion.y;
        Z = quaternion.z;
        W = quaternion.w;
    }

    VRRotation::VRRotation(){};

    VRPose::VRPose(VRPosition _Position, VRRotation _Rotation)
    {
        Position = _Position;
        Rotation = _Rotation;
    }

    VRPose::VRPose(){};

    ScoreEvent::ScoreEvent(int _Score, float _Time)
    {
        Score = _Score;
        Time = _Time;
    }

    ScoreEvent::ScoreEvent(){};

    ComboEvent::ComboEvent(int _Combo, float _Time)
    {
        Combo = _Combo;
        Time = _Time;
    }

    ComboEvent::ComboEvent(){};

    NoteID::NoteID(float _Time, int _LineLayer, int _LineIndex, int _ColorType, int _CutDirection)
    {
        Time = _Time;
        LineLayer = _LineLayer;
        LineIndex = _LineIndex;
        ColorType = _ColorType;
        CutDirection = _CutDirection;
    }

    NoteID::NoteID(){};

    EnergyEvent::EnergyEvent(float _Energy, float _Time)
    {
        Energy = _Energy;
        Time = _Time;
    }

    EnergyEvent::EnergyEvent(){};

    HeightEvent::HeightEvent(float _Height, float _Time)
    {
        Height = _Height;
        Time = _Time;
    }

    HeightEvent::HeightEvent(){};

    MultiplierEvent::MultiplierEvent(int _Multiplier, float _NextMultiplierProgress, float _Time)
    {
        Multiplier = _Multiplier;
        NextMultiplierProgress = _NextMultiplierProgress;
        Time = _Time;
    }

    MultiplierEvent::MultiplierEvent(){};

    VRPoseGroup::VRPoseGroup(VRPose _Head, VRPose _Left, VRPose _Right, int _FPS, float _Time)
    {
        Head = _Head;
        Left = _Left;
        Right = _Right;
        FPS = _FPS;
        Time = _Time;
    }

    VRPoseGroup::VRPoseGroup(){};

    Metadata::Metadata(string _Version, string _LevelID, int _Difficulty, string _Characteristic, string _Environment, vector<string> _Modifiers,
                       float _NoteSpawnOffset, bool _LeftHanded, float _InitialHeight, float _RoomRotation, VRPosition _RoomCenter, float _FailTime)
    {
        Version = _Version;
        LevelID = _LevelID;
        Difficulty = _Difficulty;
        Characteristic = _Characteristic;
        Environment = _Environment;
        Modifiers = _Modifiers;
        NoteSpawnOffset = _NoteSpawnOffset;
        LeftHanded = _LeftHanded;
        InitialHeight = _InitialHeight;
        RoomRotation = _RoomRotation;
        RoomCenter = _RoomCenter;
        FailTime = _FailTime;
    }

    Metadata::Metadata(){};

    NoteEvent::NoteEvent(NoteID _TheNoteID, NoteEventType _EventType, VRPosition _CutPoint, VRPosition _CutNormal, VRPosition _SaberDirection,
                         int _SaberType, bool _DirectionOK, float _SaberSpeed, float _CutAngle, float _CutDistanceToCenter, float _CutDirectionDeviation,
                         float _BeforeCutRating, float _AfterCutRating, float _Time, float _UnityTimescale, float _TimeSyncTimescale)
    {
        TheNoteID = _TheNoteID;
        EventType = _EventType;
        CutPoint = _CutPoint;
        CutNormal = _CutNormal;
        SaberDirection = _SaberDirection;
        SaberType = _SaberType;
        DirectionOK = _DirectionOK;
        SaberSpeed = _SaberSpeed;
        CutAngle = _CutAngle;
        CutDistanceToCenter = _CutDistanceToCenter;
        CutDirectionDeviation = _CutDirectionDeviation;
        BeforeCutRating = _BeforeCutRating;
        AfterCutRating = _AfterCutRating;
        Time = _Time;
        UnityTimescale = _UnityTimescale;
        TimeSyncTimescale = _TimeSyncTimescale;
    }

    NoteEvent::NoteEvent(){};

    ReplayFile::ReplayFile(Metadata* _metadata, vector<VRPoseGroup> _poseKeyframes, vector<HeightEvent> _heightKeyframes, vector<NoteEvent> _noteKeyframes,
                           vector<ScoreEvent> _scoreKeyframes, vector<ComboEvent> _comboKeyframes, vector<MultiplierEvent> _multiplierKeyframes,
                           vector<EnergyEvent> _energyKeyframes)
    {
        metadata = _metadata;
        poseKeyframes = _poseKeyframes;
        heightKeyframes = _heightKeyframes;
        noteKeyframes = _noteKeyframes;
        scoreKeyframes = _scoreKeyframes;
        comboKeyframes = _comboKeyframes;
        multiplierKeyframes = _multiplierKeyframes;
        energyKeyframes = _energyKeyframes;
    }

    ReplayFile::ReplayFile(){};

    ReplayFile::~ReplayFile(){
        delete metadata;
    };
} // namespace ScoreSaber::Data::Private