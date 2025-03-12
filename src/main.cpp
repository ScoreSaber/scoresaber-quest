#include "main.hpp"
#include "Utils/SafePtr.hpp"
#include "hooks.hpp"
#include "logging.hpp"

#include <GlobalNamespace/GameScenesManager.hpp>
#include <GlobalNamespace/HealthWarningFlowCoordinator.hpp>
#include <GlobalNamespace/MultiplayerLocalActivePlayerInstaller.hpp>
#include <GlobalNamespace/StandardGameplayInstaller.hpp>
#include "ReplaySystem/ReplayLoader.hpp"

#include "Data/Private/ReplayReader.hpp"
#include "Data/Private/Settings.hpp"
#include "MainInstaller.hpp"
#include "ReplaySystem/Installers/ImberInstaller.hpp"
#include "ReplaySystem/Installers/PlaybackInstaller.hpp"
#include "ReplaySystem/Installers/RecordInstaller.hpp"
#include "Services/FileService.hpp"
#include "Services/PlayerService.hpp"
#include "Services/ReplayService.hpp"
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
    return config;
}

// Called at the early stages of game loading
extern "C" __attribute((visibility("default"))) void setup(CModInfo *info) noexcept
{
    *info = modInfo.to_c();

    getConfig().Load();
    getConfig().Reload();
    getConfig().Write();
    INFO("Completed setup!");
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
    reserve_safe_ptrs(5000);
    BSML::Init();
    custom_types::Register::AutoRegister();
    Hooks::InstallHooks();
    ScoreSaber::Data::Private::Settings::LoadSettings();
    TeamUtils::Download();
    
    ScoreSaber::Services::FileService::EnsurePaths();0

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