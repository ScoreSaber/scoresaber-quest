#pragma once
#include <paper2_scotland2/shared/logger.hpp>

namespace ScoreSaber
{
    namespace Logging {
        constexpr auto Logger = Paper::ConstLoggerContext("ScoreSaber");
    }
}
#define INFO(...) ::ScoreSaber::Logging::Logger.info(__VA_ARGS__)
#define ERROR(...) ::ScoreSaber::Logging::Logger.error(__VA_ARGS__)
#define CRITICAL(...) ::ScoreSaber::Logging::Logger.critical(__VA_ARGS__)