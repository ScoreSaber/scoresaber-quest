#include "Data/Private/Settings.hpp"

#include "static.hpp"
#include "logging.hpp"

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/stringbuffer.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/writer.h"
#include "beatsaber-hook/shared/utils/utils-functions.h"

namespace ScoreSaber::Data::Private {

    namespace Settings {
        constexpr int currentVersion = 1;

        int fileVersion;
        bool showLocalPlayerRank;
        bool showScorePP;
        bool showStatusText;
        bool saveLocalReplays;
        bool enableCountryLeaderboards;
        std::string locationFilterMode;
        bool hideNAScoresFromLeaderboard;
        bool hasClickedScoreSaberLogo;
        bool hasOpenedReplayUI;
        bool leftHandedReplayUI;
        bool lockedReplayUIMode;
        std::vector<SpectatorPoseRoot> spectatorPositions;


        void InitializeDefaults() {
            fileVersion = currentVersion;
            showLocalPlayerRank = true;
            showScorePP = true;
            showStatusText = true;
            saveLocalReplays = true;
            enableCountryLeaderboards = true;
            locationFilterMode = "Country";
            hideNAScoresFromLeaderboard = false;
            hasClickedScoreSaberLogo = false;
            hasOpenedReplayUI = false;
            leftHandedReplayUI = false;
            lockedReplayUIMode = false;
            InitializeDefaultSpectatorPositions();
        }

        void InitializeDefaultSpectatorPositions() {
            spectatorPositions = {
                SpectatorPoseRoot(SpectatorPose(0, 0, -2), "Main"),
                SpectatorPoseRoot(SpectatorPose(0, 4, 0), "Bird's Eye"),
                SpectatorPoseRoot(SpectatorPose(-3, 0, 0), "Left"),
                SpectatorPoseRoot(SpectatorPose(3, 0, 0), "Right"),
            };
        }

        void LoadSettings() {
            InitializeDefaults();

            if (!fileexists(Static::SETTINGS_PATH)) {
                SaveSettings();
                return;
            }

            rapidjson::Document doc;
            auto settings_file = readfile(Static::SETTINGS_PATH);
            doc.Parse(settings_file.data());

            if (doc.HasMember("fileVersion"))
                fileVersion = doc["fileVersion"].GetInt();

            if (doc.HasMember("showLocalPlayerRank"))
                showLocalPlayerRank = doc["showLocalPlayerRank"].GetBool();

            if (doc.HasMember("showScorePP"))
                showScorePP = doc["showScorePP"].GetBool();

            if (doc.HasMember("showStatusText"))
                showStatusText = doc["showStatusText"].GetBool();

            if (doc.HasMember("saveLocalReplays"))
                saveLocalReplays = doc["saveLocalReplays"].GetBool();

            if (doc.HasMember("enableCountryLeaderboards"))
                enableCountryLeaderboards = doc["enableCountryLeaderboards"].GetBool();

            if (doc.HasMember("locationFilterMode"))
                locationFilterMode = doc["locationFilterMode"].GetString();

            if (doc.HasMember("hideNAScoresFromLeaderboard"))
                hideNAScoresFromLeaderboard = doc["hideNAScoresFromLeaderboard"].GetBool();

            if (doc.HasMember("hasClickedScoreSaberLogo"))
                hasClickedScoreSaberLogo = doc["hasClickedScoreSaberLogo"].GetBool();

            if (doc.HasMember("hasOpenedReplayUI"))
                hasOpenedReplayUI = doc["hasOpenedReplayUI"].GetBool();

            if (doc.HasMember("leftHandedReplayUI"))
                leftHandedReplayUI = doc["leftHandedReplayUI"].GetBool();

            if (doc.HasMember("lockedReplayUIMode"))
                lockedReplayUIMode = doc["lockedReplayUIMode"].GetBool();

            if (doc.HasMember("spectatorPositions")) {
                spectatorPositions.clear();
                for (auto &&position : doc["spectatorPositions"].GetArray()) {
                    auto &&pose = position["spectatorPose"].GetObject();
                    spectatorPositions.emplace_back(SpectatorPose(pose["x"].GetFloat(), pose["y"].GetFloat(), pose["z"].GetFloat()), position["name"].GetString());
                }
            }

            if (fileVersion < currentVersion) {
                // add settings upgrade code here once needed
                SaveSettings();
            }
        }

        void SaveSettings() {
            INFO("saving settings");

            fileVersion = currentVersion;

            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);

            writer.StartObject();
            writer.String("fileVersion");
            writer.Int(fileVersion);
            writer.String("showLocalPlayerRank");
            writer.Bool(showLocalPlayerRank);
            writer.String("showScorePP");
            writer.Bool(showScorePP);
            writer.String("showStatusText");
            writer.Bool(showStatusText);
            writer.String("saveLocalReplays");
            writer.Bool(saveLocalReplays);
            writer.String("enableCountryLeaderboards");
            writer.Bool(enableCountryLeaderboards);
            writer.String("locationFilterMode");
            writer.String(locationFilterMode);
            writer.String("hideNAScoresFromLeaderboard");
            writer.Bool(hideNAScoresFromLeaderboard);
            writer.String("hasClickedScoreSaberLogo");
            writer.Bool(hasClickedScoreSaberLogo);
            writer.String("hasOpenedReplayUI");
            writer.Bool(hasOpenedReplayUI);
            writer.String("leftHandedReplayUI");
            writer.Bool(leftHandedReplayUI);
            writer.String("spectatorPositions");
            writer.Bool(lockedReplayUIMode);
            writer.String("lockedReplayUIMode");
            writer.StartArray();
            for (auto position : spectatorPositions) {
                writer.StartObject();
                writer.String("name");
                writer.String(position.name);

                writer.String("spectatorPose");
                writer.StartObject();
                writer.String("x");
                writer.Double(position.spectatorPose.x);
                writer.String("y");
                writer.Double(position.spectatorPose.y);
                writer.String("z");
                writer.Double(position.spectatorPose.z);
                writer.EndObject();

                writer.EndObject();
            }
            writer.EndArray();
            writer.EndObject();
            
            assert(writer.IsComplete());

            writefile(Static::SETTINGS_PATH, s.GetString());
        }
    }

    SpectatorPose::SpectatorPose(float x, float y, float z) : x(x), y(y), z(z) { }
    SpectatorPoseRoot::SpectatorPoseRoot(SpectatorPose spectatorPose, std::string name) : name(name), spectatorPose(spectatorPose) { }
}