#include "ReplaySystem/Installers/MainInstaller.hpp"

#include "ReplaySystem/UI/ResultsViewReplayButtonController.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "logging.hpp"

DEFINE_TYPE(ScoreSaber::ReplaySystem::Installers, MainInstaller);

namespace ScoreSaber::ReplaySystem::Installers
{
    void MainInstaller::InstallBindings()
    {
        auto container = get_Container();
        container->BindInterfacesTo<UI::ResultsViewReplayButtonController*>()->AsSingle();
    }
} // namespace ScoreSaber::ReplaySystem::Installers
