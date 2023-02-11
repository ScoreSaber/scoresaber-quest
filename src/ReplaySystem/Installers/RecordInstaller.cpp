#include "ReplaySystem/Installers/RecordInstaller.hpp"

#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "ReplaySystem/Recorders/MainRecorder.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "ReplaySystem/UI/GameReplayUI.hpp"
#include "Zenject/ConcreteBinderGeneric_1.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "lapiz/shared/utilities/ZenjectExtensions.hpp"
#include "logging.hpp"

DEFINE_TYPE(ScoreSaber::ReplaySystem::Installers, RecordInstaller);

using namespace Lapiz::Zenject::ZenjectExtensions;

namespace ScoreSaber::ReplaySystem::Installers
{
    void RecordInstaller::InstallBindings()
    {
        if (!ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying) {
            auto container = get_Container();
            container->BindInterfacesAndSelfTo<Recorders::MetadataRecorder*>()->AsSingle();
            container->BindInterfacesAndSelfTo<Recorders::HeightEventRecorder*>()->AsSingle();
            container->BindInterfacesAndSelfTo<Recorders::NoteEventRecorder*>()->AsSingle();
            container->BindInterfacesAndSelfTo<Recorders::PoseRecorder*>()->AsSingle();
            container->BindInterfacesAndSelfTo<Recorders::ScoreEventRecorder*>()->AsSingle();
            container->BindInterfacesAndSelfTo<Recorders::EnergyEventRecorder*>()->AsSingle();
            container->BindInterfacesAndSelfTo<Recorders::MainRecorder*>()->AsSingle();
        }
    }
} // namespace ScoreSaber::ReplaySystem::Installers
