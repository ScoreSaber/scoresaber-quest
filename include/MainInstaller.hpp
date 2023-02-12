#pragma once

#include "UnityEngine/GameObject.hpp"
#include "Zenject/InjectContext.hpp"
#include "Zenject/Installer.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(ScoreSaber, MainInstaller, ::Zenject::Installer,
                      DECLARE_OVERRIDE_METHOD(void, InstallBindings, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::Installer::InstallBindings>::get());
                      DECLARE_DEFAULT_CTOR();)