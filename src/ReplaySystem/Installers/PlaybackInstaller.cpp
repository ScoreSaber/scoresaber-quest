#include "ReplaySystem/Installers/PlaybackInstaller.hpp"

#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "ReplaySystem/Playback/ComboPlayer.hpp"
#include "ReplaySystem/Playback/EnergyPlayer.hpp"
#include "ReplaySystem/Playback/HeightPlayer.hpp"
#include "ReplaySystem/Playback/MultiplierPlayer.hpp"
#include "ReplaySystem/Playback/NotePlayer.hpp"
#include "ReplaySystem/Playback/PosePlayer.hpp"
#include "ReplaySystem/Playback/ReplayTimeSyncController.hpp"
#include "ReplaySystem/Playback/ScorePlayer.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "ReplaySystem/UI/GameReplayUI.hpp"
#include "Zenject/ConcreteBinderGeneric_1.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "lapiz/shared/utilities/ZenjectExtensions.hpp"
#include "logging.hpp"

DEFINE_TYPE(ScoreSaber::ReplaySystem::Installers, PlaybackInstaller);

using namespace Lapiz::Zenject::ZenjectExtensions;

namespace ScoreSaber::ReplaySystem::Installers
{
    void PlaybackInstaller::ctor(GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData)
    {
        _gameplayCoreSceneSetupData = gameplayCoreSceneSetupData;
    }

    void PlaybackInstaller::InstallBindings()
    {
        if (ScoreSaber::ReplaySystem::ReplayLoader::IsPlaying) {
            auto container = get_Container();
            container->BindInterfacesAndSelfTo<Playback::PosePlayer*>()->AsSingle();
            container->BindInterfacesAndSelfTo<Playback::NotePlayer*>()->AsSingle();
            container->BindInterfacesAndSelfTo<Playback::ScorePlayer*>()->AsSingle();
            if(_gameplayCoreSceneSetupData->playerSpecificSettings->automaticPlayerHeight)
                container->BindInterfacesAndSelfTo<Playback::HeightPlayer*>()->AsSingle();

            container->Bind<Playback::ComboPlayer*>()->AsSingle();
            container->Bind<Playback::EnergyPlayer*>()->AsSingle();
            container->Bind<Playback::MultiplierPlayer*>()->AsSingle();
            container->BindInterfacesAndSelfTo<Playback::ReplayTimeSyncController*>()->AsSingle();
            FromNewComponentOnNewGameObject(container->Bind<UI::GameReplayUI*>())->AsSingle()->NonLazy();
        }
    }
} // namespace ScoreSaber::ReplaySystem::Installers
