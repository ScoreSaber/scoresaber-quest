#include "MainInstaller.hpp"

#include "ReplaySystem/UI/ResultsViewReplayButtonController.hpp"
#include "UI/Multiplayer/ScoreSaberMultiplayerInitializer.hpp"
#include "UI/Multiplayer/ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager.hpp"
#include "UI/Multiplayer/ScoreSaberMultiplayerResultsLeaderboardFlowManager.hpp"
#include "Utils/MaxScoreCache.hpp"
#include <Zenject/DiContainer.hpp>
#include <Zenject/FromBinderNonGeneric.hpp>
#include "logging.hpp"

DEFINE_TYPE(ScoreSaber, MainInstaller);

namespace ScoreSaber
{
    void MainInstaller::InstallBindings()
    {
        auto container = Container;
        container->BindInterfacesTo<ReplaySystem::UI::ResultsViewReplayButtonController*>()->AsSingle();
        container->BindInterfacesTo<UI::Multiplayer::ScoreSaberMultiplayerInitializer*>()->AsSingle();
        container->BindInterfacesTo<UI::Multiplayer::ScoreSaberMultiplayerLevelSelectionLeaderboardFlowManager*>()->AsSingle();
        container->BindInterfacesTo<UI::Multiplayer::ScoreSaberMultiplayerResultsLeaderboardFlowManager*>()->AsSingle();
        container->Bind<Utils::MaxScoreCache*>()->AsSingle();
    }
} // namespace ScoreSaber
