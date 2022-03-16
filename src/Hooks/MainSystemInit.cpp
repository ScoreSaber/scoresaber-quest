// #include "hooks.hpp"

// #include "CustomTypes/CustomLeaderboardPlatformHandler.hpp"
// #include "GlobalNamespace/LeaderboardScoreUploader.hpp"
// #include "GlobalNamespace/MainSystemInit.hpp"
// #include "GlobalNamespace/PlatformLeaderboardsHandler.hpp"
// #include "GlobalNamespace/PlatformLeaderboardsModel.hpp"
// #include "GlobalNamespace/ScreenCaptureCache.hpp"

// #include "VRUIControls/PhysicsRaycasterWithCache.hpp"
// #include "Zenject/ConcreteIdBinderGeneric_1.hpp"
// #include "Zenject/DiContainer.hpp"
// #include "Zenject/FromBinderNonGeneric.hpp"
// #include "Zenject/NameTransformScopeConcreteIdArgConditionCopyNonLazyBinder.hpp"
// #include "beatsaber-hook/shared/utils/hooking.hpp"
// #include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

// using namespace GlobalNamespace;
// using namespace VRUIControls;
// using namespace Zenject;

// ScoreSaber::CustomTypes::CustomLeaderboardPlatformHandler* leaderboardsHandler = nullptr;
// MAKE_AUTO_HOOK_MATCH(
//     MainSystemInit_InstallPlatformLeaderboardsModel,
//     &GlobalNamespace::MainSystemInit::InstallPlatformLeaderboardsModel, void,
//     GlobalNamespace::MainSystemInit* self, Zenject::DiContainer* container)
// {
//     using namespace GlobalNamespace;
//     using namespace VRUIControls;
//     using namespace Zenject;

//     leaderboardsHandler = CRASH_UNLESS(
//         il2cpp_utils::New<
//             ScoreSaber::CustomTypes::CustomLeaderboardPlatformHandler*>());
//     leaderboardsHandler->page = 1;
//     container->Bind<PlatformLeaderboardsHandler*>()
//         ->FromInstance(leaderboardsHandler)
//         ->AsSingle();
//     container->Bind<LeaderboardScoreUploader*>()
//         ->FromComponentInNewPrefab(self->leaderboardScoreUploader)
//         ->AsSingle();
//     container->BindInterfacesAndSelfTo<PlatformLeaderboardsModel*>()
//         ->FromComponentInNewPrefab(self->platformLeaderboardsModel)
//         ->AsSingle();

//     container->Bind<ScreenCaptureCache*>()->AsSingle();
//     container->Bind<PhysicsRaycasterWithCache*>()->AsSingle();
// }