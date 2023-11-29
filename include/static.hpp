#include <string>
namespace ScoreSaber::Static
{

    // https://scoresaber.com
    static const std::string BASE_URL = "https://scoresaber.com";

    static const std::string DATA_DIR = "/sdcard/ModData/com.beatgames.beatsaber/Mods/ScoreSaber";
    static const std::string STEAM_KEY_PATH = DATA_DIR + "/scoresaber_DO_NOT_SHARE.scary";
    static const std::string FRIENDS_PATH = DATA_DIR + "/friends.txt";
    static const std::string SETTINGS_PATH = DATA_DIR + "/ScoreSaber.json";

    static const std::string MOD_DIR = "/sdcard/Android/data/com.beatgames.beatsaber/files/mods";

    static const std::string REPLAY_DIR = DATA_DIR + "/replays";
    static const std::string REPLAY_TMP_DIR = REPLAY_DIR + "/tmp";
} // namespace ScoreSaber::Static
