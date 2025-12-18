#pragma once

#include <GlobalNamespace/GameplayCoreSceneSetupData.hpp>
#include <UnityEngine/GameObject.hpp>
#include <Zenject/InjectContext.hpp>
#include <Zenject/Installer.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(ScoreSaber::ReplaySystem::Installers, PlaybackInstaller, ::Zenject::Installer) {
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::GameplayCoreSceneSetupData*, _gameplayCoreSceneSetupData);
    DECLARE_OVERRIDE_METHOD_MATCH(void, InstallBindings, &::Zenject::Installer::InstallBindings);
    DECLARE_CTOR(ctor, GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData);
};