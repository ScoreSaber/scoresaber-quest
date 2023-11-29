#include <string>
#include <vector>

namespace ScoreSaber::Data::Private {
    struct SpectatorPose {
        float x;
        float y;
        float z;

        SpectatorPose(float x, float y, float z);
    };

    struct SpectatorPoseRoot {
        std::string name;
        SpectatorPose spectatorPose;

        SpectatorPoseRoot(SpectatorPose spectatorPose, std::string name);
    };

    namespace Settings {
        extern int fileVersion;
        extern bool showLocalPlayerRank;
        extern bool showScorePP;
        extern bool showStatusText;
        extern bool saveLocalReplays;
        extern bool enableCountryLeaderboards; // Not yet implemented
        extern std::string locationFilterMode;
        extern bool hideNAScoresFromLeaderboard;
        extern bool hasClickedScoreSaberLogo; // Not yet implemented
        extern bool hasOpenedReplayUI;
        extern bool leftHandedReplayUI;
        extern bool lockedReplayUIMode;
        extern std::vector<SpectatorPoseRoot> spectatorPositions;

        void InitializeDefaults();
        void InitializeDefaultSpectatorPositions();
        void LoadSettings();
        void SaveSettings();
    }
}