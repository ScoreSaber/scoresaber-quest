#pragma once
#include "Utils/StringUtils.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "../extern/includes/beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "../extern/includes/beatsaber-hook/shared/rapidjson/include/rapidjson/stringbuffer.h"
#include "../extern/includes/beatsaber-hook/shared/rapidjson/include/rapidjson/writer.h"

using namespace std;
using namespace rapidjson;

namespace ScoreSaber::Data::Private
{
    class ScoreSaberUploadData
    {
      public:
        u16string playerName;
        string playerId;
        int score;
        string leaderboardId;
        string songName;
        string songSubName;
        string levelAuthorName;
        string songAuthorName;
        int bpm;
        int difficulty;
        string infoHash;
        vector<string> modifiers;
        string gameMode;
        int badCutsCount;
        int missedCount;
        int maxCombo;
        bool fullCombo;
        string deviceHmdIdentifier;
        string deviceControllerLeftIdentifier;
        string deviceControllerRightIdentifier;
        ScoreSaberUploadData();
        ~ScoreSaberUploadData();
        ScoreSaberUploadData(u16string playerName, string playerId, int score, string leaderboardId,
                                           string songName, string songSubName, string levelAuthorName, string songAuthorName, int bpm, int difficulty,
                                           string infoHash, vector<string> modifiers, string gameMode, int badCutsCount, int missedCount, int maxCombo,
                                           bool fullCombo, string deviceHmdIdentifier, string deviceControllerLeftIdentifier, string deviceControllerRightIdentifier);

        template <typename Writer>
        void Serialize(Writer& writer) const
        {
            writer.StartObject();
            writer.String("playerName");
            writer.String(to_utf8(playerName).c_str());
            writer.String("playerId");
            writer.String(playerId.c_str());
            writer.String("score");
            writer.Int(score);
            writer.String("leaderboardId");
            writer.String(leaderboardId.c_str());
            writer.String("songName");
            writer.String(songName.c_str());
            writer.String("songSubName");
            writer.String(songSubName.c_str());
            writer.String("levelAuthorName");
            writer.String(levelAuthorName.c_str());
            writer.String("songAuthorName");
            writer.String(songAuthorName.c_str());
            writer.String("bpm");
            writer.Int(bpm);
            writer.String("difficulty");
            writer.Int(difficulty);
            writer.String("infoHash");
            writer.String(infoHash.c_str());

            writer.String("modifiers");
            writer.StartArray();
            for (auto mod : modifiers)
            {
                writer.String(mod.c_str());
            }
            writer.EndArray();

            writer.String("gameMode");
            writer.String(gameMode.c_str());
            writer.String("badCutsCount");
            writer.Int(badCutsCount);
            writer.String("missedCount");
            writer.Int(missedCount);
            writer.String("maxCombo");
            writer.Int(maxCombo);
            writer.String("fullCombo");
            writer.Bool(fullCombo);
            writer.String("hmd");
            writer.Null(); // legacy field, can't be determined post unity upgrade
            writer.String("deviceHmdIdentifier");
            writer.String(deviceHmdIdentifier.c_str());
            writer.String("deviceControllerLeftIdentifier");
            writer.String(deviceControllerLeftIdentifier.c_str());
            writer.String("deviceControllerRightIdentifier");
            writer.String(deviceControllerRightIdentifier.c_str());
            writer.EndObject();
        }

        std::string serialize()
        {
            StringBuffer s;
            Writer<StringBuffer> writer(s);
            Serialize(writer);
            return s.GetString();
        }
    };

} // namespace ScoreSaber::Data::Private