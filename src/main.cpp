#include "main.hpp"
#include "hooks.hpp"
#include "logging.hpp"

#include "GlobalNamespace/GameScenesManager.hpp"
#include "GlobalNamespace/HealthWarningFlowCoordinator.hpp"
#include "GlobalNamespace/HealthWarningFlowCoordinator_InitData.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "questui/shared/QuestUI.hpp"

#include "Data/Private/ReplayReader.hpp"
#include "ReplaySystem/Installers/ImberInstaller.hpp"
#include "ReplaySystem/Installers/PlaybackInstaller.hpp"
#include "Services/FileService.hpp"
#include "UI/Other/Banner.hpp"
#include "Utils/TeamUtils.hpp"
#include "lapiz/shared/zenject/Zenjector.hpp"
#include "static.hpp"

ModInfo modInfo = {MOD_ID, VERSION};

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig()
{
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger()
{
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" __attribute((visibility("default"))) void setup(ModInfo& info)
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
    // il2cpp_functions::Class_Init(classof(HMUI::ImageView*));
    // il2cpp_functions::Class_Init(classof(HMUI::CurvedTextMeshPro*));
    QuestUI::Init();
    custom_types::Register::AutoRegister();
    Hooks::InstallHooks(ScoreSaber::Logging::getLogger());
    TeamUtils::Download();
    ScoreSaber::Services::FileService::EnsurePaths();

    ScoreSaber::ReplaySystem::ReplayLoader::LoadReplay();
    ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying = true;
    auto zenjector = Lapiz::Zenject::Zenjector::Get();
    //   zenjector->Install<TrickSaber::Installers::GameInstaller*>(Lapiz::Zenject::Location::StandardPlayer);
    zenjector->Install<ScoreSaber::ReplaySystem::Installers::ImberInstaller*>(Lapiz::Zenject::Location::StandardPlayer);
    zenjector->Install<ScoreSaber::ReplaySystem::Installers::PlaybackInstaller*>(Lapiz::Zenject::Location::StandardPlayer);
}