#pragma once

#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "HMUI/ViewController_DidActivateDelegate.hpp"
#include "System/IDisposable.hpp"
#include "System/Action_1.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "Zenject/IInitializable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include <vector>

#define INTERFACES                                                         \
    {                                                                      \
        classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::UI, ResultsViewReplayButtonController, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::UI::ResultsViewReplayButtonController", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::ResultsViewController*, _resultsViewController);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::IDifficultyBeatmap*, _difficultyBeatmap);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::LevelCompletionResults*, _levelCompletionResults);
                                    DECLARE_PRIVATE_FIELD(UnityEngine::UI::Button*, watchReplayButton);
                                    DECLARE_CTOR(ctor, GlobalNamespace::ResultsViewController* resultsViewController);
                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    DECLARE_INSTANCE_METHOD(void, ResultsViewController_didActivateEvent, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                                    DECLARE_INSTANCE_METHOD(void, ResultsViewController_restartButtonPressedEvent, GlobalNamespace::ResultsViewController* obj);
                                    DECLARE_INSTANCE_METHOD(void, ResultsViewController_continueButtonPressedEvent, GlobalNamespace::ResultsViewController* obj);
                                    DECLARE_INSTANCE_METHOD(void, WaitForReplay);
                                    DECLARE_INSTANCE_METHOD(void, ClickedReplayButton);
                                    std::vector<char> _serializedReplay;
                                    volatile bool _replayReady = false;
                                    HMUI::ViewController::DidActivateDelegate* didActivateDelegate;
                                    System::Action_1<GlobalNamespace::ResultsViewController*>* continueButtonPressedDelegate;
                                    System::Action_1<GlobalNamespace::ResultsViewController*>* restartButtonPressedDelegate;
                                    void UploadDaemon_ReplaySerialized(const std::vector<char> &serializedReplay);

);

#undef INTERFACES
