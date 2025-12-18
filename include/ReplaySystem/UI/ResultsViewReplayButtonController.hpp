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
#include "Utils/DelegateUtils.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::ReplaySystem::UI,
        ResultsViewReplayButtonController,
        System::Object,
        Zenject::IInitializable*,
        System::IDisposable*) {
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::ResultsViewController>, _resultsViewController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapLevel*, _beatmapLevel);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapKey, _beatmapKey);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::LevelCompletionResults*, _levelCompletionResults);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<UnityEngine::UI::Button>, watchReplayButton);
    DECLARE_CTOR(ctor, GlobalNamespace::ResultsViewController* resultsViewController);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_INSTANCE_METHOD(void, ResultsViewController_didActivateEvent, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_METHOD(void, ResultsViewController_restartButtonPressedEvent, UnityW<GlobalNamespace::ResultsViewController> obj);
    DECLARE_INSTANCE_METHOD(void, ResultsViewController_continueButtonPressedEvent, UnityW<GlobalNamespace::ResultsViewController> obj);
    DECLARE_INSTANCE_METHOD(void, WaitForReplay);
    DECLARE_INSTANCE_METHOD(void, ClickedReplayButton);
    std::vector<char> _serializedReplay;
    volatile bool _replayReady = false;
    DelegateUtils::DelegateW<HMUI::ViewController::DidActivateDelegate> didActivateDelegate;
    DelegateUtils::DelegateW<System::Action_1<UnityW<GlobalNamespace::ResultsViewController>>> continueButtonPressedDelegate;
    DelegateUtils::DelegateW<System::Action_1<UnityW<GlobalNamespace::ResultsViewController>>> restartButtonPressedDelegate;
    void UploadDaemon_ReplaySerialized(const std::vector<char> &serializedReplay);
};