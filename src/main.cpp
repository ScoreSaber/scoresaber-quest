#include "main.hpp"
#include "hooks.hpp"
#include "logging.hpp"

#include <GlobalNamespace/GameScenesManager.hpp>
#include <GlobalNamespace/HealthWarningFlowCoordinator.hpp>
#include <GlobalNamespace/HealthWarningFlowCoordinator_InitData.hpp>
#include <GlobalNamespace/MultiplayerLocalActivePlayerInstaller.hpp>
#include <GlobalNamespace/StandardGameplayInstaller.hpp>
#include "ReplaySystem/ReplayLoader.hpp"
#include "questui/QuestUI.hpp"

#include "Data/Private/ReplayReader.hpp"
#include "Data/Private/Settings.hpp"
#include "MainInstaller.hpp"
#include "ReplaySystem/Installers/ImberInstaller.hpp"
#include "ReplaySystem/Installers/PlaybackInstaller.hpp"
#include "ReplaySystem/Installers/RecordInstaller.hpp"
#include "Services/FileService.hpp"
#include "Services/PlayerService.hpp"
#include "Services/ReplayService.hpp"
#include "UI/Other/Banner.hpp"
#include "UI/Other/ProfilePictureView.hpp"
#include "UI/Other/ScoreSaberLeaderboardView.hpp"
#include "Utils/TeamUtils.hpp"
#include <bsml/shared/BSML.hpp>
#include <bsml/shared/BSMLDataCache.hpp>
#include <lapiz/shared/zenject/Zenjector.hpp>
#include "assets.hpp"
#include "static.hpp"

static modloader::ModInfo modInfo = {MOD_ID, VERSION, 0};

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
extern "C" __attribute((visibility("default"))) void setup(CModInfo& info) noexcept
{
    info = modInfo;

    getConfig().Load();
    getConfig().Reload();
    getConfig().Write();
    getLogger().info("Completed setup!");
}

void soft_restart()
{
    ScoreSaber::ReplaySystem::ReplayLoader::OnSoftRestart();
    ScoreSaber::Services::PlayerService::OnSoftRestart();
    ScoreSaber::Services::ReplayService::OnSoftRestart();
    ScoreSaber::UI::Other::ScoreSaberLeaderboardView::OnSoftRestart();
    ScoreSaber::UI::Other::ProfilePictureView::OnSoftRestart();
}

// Called later on in the game loading - a good time to install function hooks
extern "C" __attribute((visibility("default"))) void late_load() noexcept
{
    il2cpp_functions::Init();
    // il2cpp_functions::Class_Init(classof(HMUI::ImageView*));
    // il2cpp_functions::Class_Init(classof(HMUI::CurvedTextMeshPro*));
    BSML::Init();
    QuestUI::Init();
    custom_types::Register::AutoRegister();
    Hooks::InstallHooks(ScoreSaber::Logging::getLogger());
    ScoreSaber::Data::Private::Settings::LoadSettings();
    TeamUtils::Download();
    
    ScoreSaber::Services::FileService::EnsurePaths();

    auto zenjector = Lapiz::Zenject::Zenjector::Get();
    zenjector->Install<ScoreSaber::MainInstaller*>(Lapiz::Zenject::Location::Menu);
    zenjector->Install<ScoreSaber::ReplaySystem::Installers::ImberInstaller*>(Lapiz::Zenject::Location::StandardPlayer);
    zenjector->Install<ScoreSaber::ReplaySystem::Installers::PlaybackInstaller*>(Lapiz::Zenject::Location::StandardPlayer);
    zenjector->Install<ScoreSaber::ReplaySystem::Installers::RecordInstaller*, GlobalNamespace::StandardGameplayInstaller*>();
    zenjector->Install<ScoreSaber::ReplaySystem::Installers::RecordInstaller*, GlobalNamespace::MultiplayerLocalActivePlayerInstaller*>();

    BSML::Events::onGameDidRestart.addCallback(soft_restart);
}

BSML_DATACACHE(replay_png) {
    return IncludedAssets::replay_png;
}