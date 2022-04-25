#include "System/IO/Directory.hpp"
#include "Utils/StringUtils.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "logging.hpp"
#include "static.hpp"

namespace ScoreSaber::Services::FileService
{
    void EnsurePaths()
    {
        if (!direxists(ScoreSaber::Static::DATA_DIR))
        {
            System::IO::Directory::CreateDirectory(il2cpp_utils::newcsstr(ScoreSaber::Static::DATA_DIR));
        }
        if (!direxists(ScoreSaber::Static::REPLAY_DIR))
        {
            System::IO::Directory::CreateDirectory(il2cpp_utils::newcsstr(ScoreSaber::Static::REPLAY_DIR));
        }
        if (!direxists(ScoreSaber::Static::REPLAY_TMP_DIR))
        {
            System::IO::Directory::CreateDirectory(il2cpp_utils::newcsstr(ScoreSaber::Static::REPLAY_TMP_DIR));
        }
    }

    std::string GetReplayFileName(std::string levelId, std::string difficultyName, std::string characteristic, std::string playerId, std::string songName)
    {
        songName = StringUtils::ReplaceInvalidChars(StringUtils::Truncate(songName, 155, false));
        std::string replayPath = playerId + "-" + songName + "-" + difficultyName + "-" + characteristic + "-" + levelId;
        return replayPath;
    }
} // namespace ScoreSaber::Services::FileService
