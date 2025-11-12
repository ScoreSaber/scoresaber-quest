#pragma once

#include <UnityEngine/GameObject.hpp>
#include <Zenject/InjectContext.hpp>
#include <Zenject/Installer.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(ScoreSaber::ReplaySystem::Installers, RecordInstaller, ::Zenject::Installer) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, InstallBindings, &::Zenject::Installer::InstallBindings);
    DECLARE_DEFAULT_CTOR();
};