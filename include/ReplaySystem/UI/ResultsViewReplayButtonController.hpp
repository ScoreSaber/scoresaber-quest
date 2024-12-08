#pragma once

#include <GlobalNamespace/BeatmapKey.hpp>
#include <GlobalNamespace/BeatmapLevel.hpp>
#include <GlobalNamespace/ResultsViewController.hpp>
#include <GlobalNamespace/LevelCompletionResults.hpp>
#include <HMUI/ViewController.hpp>
#include <System/IDisposable.hpp>
#include <System/Action_1.hpp>
#include <UnityEngine/UI/Button.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include <vector>

#define INTERFACES                                                         \
    {                                                                      \
        classof(Zenject::IInitializable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::UI, ResultsViewReplayButtonController, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::UI::ResultsViewReplayButtonController", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ResultsViewController*, _resultsViewController);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapLevel*, _beatmapLevel);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapKey*, _beatmapKey);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::LevelCompletionResults*, _levelCompletionResults);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, watchReplayButton);
                                    DECLARE_CTOR(ctor, GlobalNamespace::ResultsViewController* resultsViewController);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
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
