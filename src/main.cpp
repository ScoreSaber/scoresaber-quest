#include "main.hpp"
#include "hooks.hpp"
#include "logging.hpp"

#include "GlobalNamespace/GameScenesManager.hpp"
#include "GlobalNamespace/HealthWarningFlowCoordinator.hpp"
#include "GlobalNamespace/HealthWarningFlowCoordinator_InitData.hpp"
#include "questui/shared/QuestUI.hpp"

#include "Services/FileService.hpp"
#include "UI/Other/Banner.hpp"
#include "Utils/TeamUtils.hpp"

ModInfo modInfo = {ID, VERSION};

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig()
{
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// HACK: Move to the next scene upon health and safety loading
// just here to make loading faster, can be removed in the final version
// MAKE_AUTO_HOOK_MATCH(HealthWarningFlowCoordinator_DidActivate, &GlobalNamespace::HealthWarningFlowCoordinator::DidActivate, void, GlobalNamespace::HealthWarningFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
// {
//     self->gameScenesManager->ReplaceScenes(self->initData->nextScenesTransitionSetupData, 0.0f, nullptr, nullptr);
// }

// Returns a logger, useful for printing debug messages
Logger& getLogger()
{
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info)
{
    info = modInfo;

    getConfig().Load();
    getConfig().Reload();
    getConfig().Write();
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" __attribute((visibility("default"))) void load()
{
    il2cpp_functions::Init();
    il2cpp_functions::Class_Init(classof(HMUI::ImageView*));
    il2cpp_functions::Class_Init(classof(HMUI::CurvedTextMeshPro*));
    QuestUI::Init();
    custom_types::Register::AutoRegister();
    Hooks::InstallHooks(ScoreSaber::Logging::getLogger());
    TeamUtils::Download();
    ScoreSaber::Services::FileService::EnsurePaths();
}