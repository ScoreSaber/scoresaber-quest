#include "ReplaySystem/ReplayLoader.hpp"
#include "ReplaySystem/UI/ResultsViewReplayButtonController.hpp"
#include "Services/PlayerService.hpp"
#include "Services/ReplayService.hpp"
#include "Services/UploadService.hpp"

#include <bsml/shared/BSML.hpp>
#include <custom-types/shared/delegate.hpp>
#include <bsml/shared/BSML/MainThreadScheduler.hpp>
#include "Utils/OperatorOverloads.hpp"
#include "logging.hpp"

using namespace BSML;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, ResultsViewReplayButtonController);

namespace ScoreSaber::ReplaySystem::UI
{
    void ResultsViewReplayButtonController::ctor(GlobalNamespace::ResultsViewController* resultsViewController)
    {
        INVOKE_CTOR();
        _resultsViewController = resultsViewController;
    }

    void ResultsViewReplayButtonController::Initialize()
    {
        didActivateDelegate = custom_types::MakeDelegate<HMUI::ViewController::DidActivateDelegate*>(classof(HMUI::ViewController::DidActivateDelegate*), (std::function<void(bool, bool, bool)>)[&](bool a, bool b, bool c){ ResultsViewController_didActivateEvent(a, b, c);});
        continueButtonPressedDelegate = custom_types::MakeDelegate<System::Action_1<UnityW<GlobalNamespace::ResultsViewController>>*>(classof(System::Action_1<UnityW<GlobalNamespace::ResultsViewController>>*), (std::function<void(UnityW<GlobalNamespace::ResultsViewController>)>)[&](UnityW<GlobalNamespace::ResultsViewController> o){ ResultsViewController_continueButtonPressedEvent(o);});
        restartButtonPressedDelegate = custom_types::MakeDelegate<System::Action_1<UnityW<GlobalNamespace::ResultsViewController>>*>(classof(System::Action_1<UnityW<GlobalNamespace::ResultsViewController>>*), (std::function<void(UnityW<GlobalNamespace::ResultsViewController>)>)[&](UnityW<GlobalNamespace::ResultsViewController> o){ ResultsViewController_restartButtonPressedEvent(o);});
        
        _resultsViewController->add_didActivateEvent(didActivateDelegate);
        _resultsViewController->add_continueButtonPressedEvent(continueButtonPressedDelegate);
        _resultsViewController->add_restartButtonPressedEvent(restartButtonPressedDelegate);
        Services::ReplayService::ReplaySerialized = [&](const std::vector<char> &v) { UploadDaemon_ReplaySerialized(v); };
    }

    void ResultsViewReplayButtonController::ResultsViewController_didActivateEvent(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if(firstActivation) {
            BSML::parse_and_construct(
                "<button-with-icon id=\"watchReplayButton\" icon=\"ScoreSaber_replay_png\" hover-hint=\"Watch Replay\" pref-width=\"15\" pref-height=\"13\" interactable=\"false\" on-click=\"ClickedReplayButton\" />",
                _resultsViewController->gameObject->transform,
                this
            );
            watchReplayButton->transform->localScale = watchReplayButton->transform->localScale * 0.4f;
            watchReplayButton->transform->localPosition = {42.5f, 27.0f, 0.0f};
        }
        _replayReady = _serializedReplay.size() > 0;
        watchReplayButton->interactable = _replayReady;
        _beatmapLevel = _resultsViewController->_beatmapLevel;
        _beatmapKey = _resultsViewController->_beatmapKey;
        _levelCompletionResults = _resultsViewController->_levelCompletionResults;
        WaitForReplay();
    }

    void ResultsViewReplayButtonController::ResultsViewController_restartButtonPressedEvent(GlobalNamespace::ResultsViewController* obj)
    {
        _serializedReplay.clear();
        _replayReady = false;
    }

    void ResultsViewReplayButtonController::ResultsViewController_continueButtonPressedEvent(GlobalNamespace::ResultsViewController* obj)
    {
        _serializedReplay.clear();
        _replayReady = false;
    }
    
    void ResultsViewReplayButtonController::UploadDaemon_ReplaySerialized(const std::vector<char> &serializedReplay)
    {
        _serializedReplay = serializedReplay;
        _replayReady = true;
    }

    void ResultsViewReplayButtonController::WaitForReplay()
    {
        il2cpp_utils::il2cpp_aware_thread([&](){
            while(!_replayReady) {
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
            }
            MainThreadScheduler::Schedule([&]() {
                watchReplayButton->interactable = true;
            });
        }).detach();
    }

    void ResultsViewReplayButtonController::Dispose()
    {
        _resultsViewController->remove_didActivateEvent(didActivateDelegate);
        _resultsViewController->remove_continueButtonPressedEvent(continueButtonPressedDelegate);
        _resultsViewController->remove_restartButtonPressedEvent(restartButtonPressedDelegate);
        Services::ReplayService::ReplaySerialized = nullptr;
    }

    void ResultsViewReplayButtonController::ClickedReplayButton()
    {
        il2cpp_utils::il2cpp_aware_thread([=, this](){
            std::vector<std::string> modifiersVec = Services::UploadService::GetModifierList(_levelCompletionResults->gameplayModifiers, _levelCompletionResults->energy);
            std::string modifiers;
            for(int i = 0; i < modifiersVec.size(); ++i) {
                if(i > 0) modifiers += ',';
                modifiers += modifiersVec[i];
            }
            ReplayLoader::Load(_serializedReplay, _beatmapLevel, _beatmapKey, modifiers, ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.name);
        }).detach();
        watchReplayButton->interactable = false;
    }
}