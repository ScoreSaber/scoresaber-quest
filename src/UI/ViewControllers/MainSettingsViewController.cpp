#include "UI/ViewControllers/MainSettingsViewController.hpp"

#include "Data/Private/Settings.hpp"
#include <bsml/shared/BSML.hpp>
#include "assets.hpp"
#include "logging.hpp"

using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::UI::ViewControllers, MainSettingsViewController);

namespace ScoreSaber::UI::ViewControllers {
    bool MainSettingsViewController::showScorePP {
        return Settings::showScorePP;
    }

    void MainSettingsViewController::showScorePP = bool value {
        Settings::showScorePP = value;
    }

    bool MainSettingsViewController::showLocalPlayerRank {
        return Settings::showLocalPlayerRank;
    }

    void MainSettingsViewController::showLocalPlayerRank = bool value {
        Settings::showLocalPlayerRank = value;
    }
    
    bool MainSettingsViewController::hideNAScores {
        return Settings::hideNAScoresFromLeaderboard;
    }

    void MainSettingsViewController::hideNAScores = bool value {
        Settings::hideNAScoresFromLeaderboard = value;
    }
    
    StringW MainSettingsViewController::locationFilterMode {
        return Settings::locationFilterMode;
    }

    void MainSettingsViewController::locationFilterMode = StringW value {
        Settings::locationFilterMode = (std::string)value;
    }
    
    bool MainSettingsViewController::enableCountryLeaderboards {
        return Settings::enableCountryLeaderboards;
    }

    void MainSettingsViewController::enableCountryLeaderboards = bool value {
        Settings::enableCountryLeaderboards = value;
    }
    
    bool MainSettingsViewController::saveLocalReplays {
        INFO("read");
        return Settings::saveLocalReplays;
    }

    void MainSettingsViewController::saveLocalReplays = bool value {
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