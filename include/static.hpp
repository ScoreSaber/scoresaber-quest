#include <string>
namespace ScoreSaber::Static
{
    // http://192.168.1.8:9999
    // https://scoresaber.com
    static const std::string BASE_URL = "http://192.168.1.8:9999";

    static const std::string DATA_DIR = "/sdcard/ModData/Mods/ScoreSaber";
    static const std::string STEAM_KEY_PATH = DATA_DIR + "/scoresaber_DO_NOT_SHARE.scary";

    static const std::string REPLAY_DIR = DATA_DIR + "/replays";
    static const std::string REPLAY_TMP_DIR = REPLAY_DIR + "/tmp";
} // namespace ScoreSaber::Static
