#include "Utils/obfuscation.hpp"
#include <string>
namespace ScoreSaber::Static
{

    // https://scoresaber.com
    static const std::string BASE_URL = ENCRYPT_STRING_AUTO_A(encoder, "https://scoresaber.com");

    static const std::string DATA_DIR = ENCRYPT_STRING_AUTO_A(encoder, "/sdcard/ModData/com.beatgames.beatsaber/Mods/ScoreSaber");
    static const std::string STEAM_KEY_PATH = DATA_DIR + ENCRYPT_STRING_AUTO_A(encoder, "/scoresaber_DO_NOT_SHARE.scary");
    static const std::string FRIENDS_PATH = DATA_DIR + ENCRYPT_STRING_AUTO_A(encoder, "/friends.txt");

    static const std::string MOD_DIR = ENCRYPT_STRING_AUTO_A(encoder, "/sdcard/Android/data/com.beatgames.beatsaber/files/mods");

    static const std::string REPLAY_DIR = DATA_DIR + ENCRYPT_STRING_AUTO_A(encoder, "/replays");
    static const std::string REPLAY_TMP_DIR = REPLAY_DIR + ENCRYPT_STRING_AUTO_A(encoder, "/tmp");
} // namespace ScoreSaber::Static
