#include "UI/ViewControllers/MainSettingsViewController.hpp"

#include "Data/Private/Settings.hpp"
#include <bsml/shared/BSML.hpp>
#include "assets.hpp"
#include "logging.hpp"

using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::UI::ViewControllers, MainSettingsViewController);

namespace ScoreSaber::UI::ViewControllers {
    bool MainSettingsViewController::get_showScorePP() {
        return Settings::showScorePP;
    }

    void MainSettingsViewController::set_showScorePP(bool value) {
        Settings::showScorePP = value;
    }

    bool MainSettingsViewController::get_showLocalPlayerRank() {
        return Settings::showLocalPlayerRank;
    }

    void MainSettingsViewController::set_showLocalPlayerRank(bool value) {
        Settings::showLocalPlayerRank = value;
    }
    
    bool MainSettingsViewController::get_hideNAScores() {
        return Settings::hideNAScoresFromLeaderboard;
    }

    void MainSettingsViewController::set_hideNAScores(bool value) {
        Settings::hideNAScoresFromLeaderboard = value;
    }
    
    StringW MainSettingsViewController::get_locationFilterMode() {
        return Settings::locationFilterMode;
    }

    void MainSettingsViewController::set_locationFilterMode(StringW value) {
        Settings::locationFilterMode = (std::string)value;
    }
    
    bool MainSettingsViewController::get_enableCountryLeaderboards() {
        return Settings::enableCountryLeaderboards;
    }

    void MainSettingsViewController::set_enableCountryLeaderboards(bool value) {
        Settings::enableCountryLeaderboards = value;
    }
    
    bool MainSettingsViewController::get_saveLocalReplays() {
        INFO("read");
        return Settings::saveLocalReplays;
    }

    void MainSettingsViewController::set_saveLocalReplays(bool value) {
        INFO("written");
        Settings::saveLocalReplays = value;
    }
    
    void MainSettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation) {
            BSML::parse_and_construct(IncludedAssets::MainSettingsViewController_bsml, transform, this);
        }
    }

    void MainSettingsViewController::ctor() {
        INVOKE_CTOR();
        locationFilerOptions->Add(StringW("Country"));
        locationFilerOptions->Add(StringW("Region"));
    }
}