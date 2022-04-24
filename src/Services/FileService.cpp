#include "System/IO/Directory.hpp"
#include "Utils/StringUtils.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "logging.hpp"
#include "static.hpp"

using namespace ScoreSaber::Static;

namespace ScoreSaber::Services::FileService
{
    void EnsurePaths()
    {
        if (!direxists(DATA_DIR))
        {
            System::IO::Directory::CreateDirectory(il2cpp_utils::newcsstr(DATA_DIR));
        }
        if (!direxists(REPLAY_DIR))
        {
            System::IO::Directory::CreateDirectory(il2cpp_utils::newcsstr(REPLAY_DIR));
        }
        if (!direxists(REPLAY_TMP_DIR))
        {
            System::IO::Directory::CreateDirectory(il2cpp_utils::newcsstr(REPLAY_TMP_DIR));
        }
    }
} // namespace ScoreSaber::Services::FileService
