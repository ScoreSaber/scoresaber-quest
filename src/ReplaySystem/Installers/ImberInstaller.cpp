#include "ReplaySystem/Installers/ImberInstaller.hpp"

#include "ReplaySystem/UI/ImberManager.hpp"
#include "ReplaySystem/UI/ImberScrubber.hpp"
#include "ReplaySystem/UI/ImberSpecsReporter.hpp"
#include "ReplaySystem/UI/ImberUIPositionController.hpp"
#include "ReplaySystem/UI/MainImberPanelView.hpp"
#include "ReplaySystem/UI/SpectateAreaController.hpp"
#include "ReplaySystem/UI/VRControllerAccessor.hpp"
#include "Zenject/ConcreteBinderGeneric_1.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "lapiz/shared/utilities/ZenjectExtensions.hpp"
#include "logging.hpp"

DEFINE_TYPE(ScoreSaber::ReplaySystem::Installers, ImberInstaller);

using namespace Lapiz::Zenject::ZenjectExtensions;

namespace ScoreSaber::ReplaySystem::Installers
{
    void ImberInstaller::InstallBindings()
    {
        auto container = get_Container();
        INFO("Installing ImberInstaller");
        INFO("Installing VRControllerAccessor");
        container->Bind<UI::VRControllerAccessor*>()->AsSingle();
        INFO("Installing ImberManager");
        container->BindInterfacesTo<UI::ImberManager*>()->AsSingle();
        INFO("Installing ImberScrubber");
        container->BindInterfacesAndSelfTo<UI::ImberScrubber*>()->AsSingle();
        INFO("Installing ImberSpecsReporter");
        container->BindInterfacesAndSelfTo<UI::ImberSpecsReporter*>()->AsSingle();
        INFO("Installing ImberUIPositionController");
        container->BindInterfacesAndSelfTo<UI::ImberUIPositionController*>()->AsSingle();
        INFO("Installing MainImberPanelView");
        FromNewComponentAsViewController(container->Bind<UI::MainImberPanelView*>())->AsSingle();
        INFO("Installing SpectateAreaController");
        container->BindInterfacesAndSelfTo<UI::SpectateAreaController*>()->AsSingle();
    }
} // namespace ScoreSaber::ReplaySystem::Installers
