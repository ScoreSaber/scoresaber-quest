#include "ReplaySystem/ReplayLoader.hpp"
#include "ReplaySystem/UI/ResultsViewReplayButtonController.hpp"
#include "Services/PlayerService.hpp"
#include "Services/ReplayService.hpp"
#include "Services/UploadService.hpp"
#include "bsml/shared/BSML.hpp"
#include "custom-types/shared/delegate.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"


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
        continueButtonPressedDelegate = custom_types::MakeDelegate<System::Action_1<GlobalNamespace::ResultsViewController*>*>(classof(System::Action_1<GlobalNamespace::ResultsViewController*>*), (std::function<void(GlobalNamespace::ResultsViewController*)>)[&](GlobalNamespace::ResultsViewController* o){ ResultsViewController_continueButtonPressedEvent(o);});
        restartButtonPressedDelegate = custom_types::MakeDelegate<System::Action_1<GlobalNamespace::ResultsViewController*>*>(classof(System::Action_1<GlobalNamespace::ResultsViewController*>*), (std::function<void(GlobalNamespace::ResultsViewController*)>)[&](GlobalNamespace::ResultsViewController* o){ ResultsViewController_restartButtonPressedEvent(o);});
        
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
                _resultsViewController->get_gameObject()->get_transform(),
                this
            );
            watchReplayButton->get_transform()->set_localScale(watchReplayButton->get_transform()->get_localScale() * 0.4f);
            watchReplayButton->get_transform()->set_localPosition({42.5f, 27.0f, 0.0f});
        }
        watchReplayButton->set_interactable(_serializedReplay.size() > 0);
        _difficultyBeatmap = _resultsViewController->difficultyBeatmap;
        _levelCompletionResults = _resultsViewController->levelCompletionResults;
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
        std::thread([&](){
            while(!_replayReady) {
                std::this_thread::sleep_for(std::chrono::seconds(25));
            }
            QuestUI::MainThreadScheduler::Schedule([&]() {
                watchReplayButton->set_interactable(true);
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
        std::thread([=](){
            std::vector<std::string> modifiersVec = Services::UploadService::GetModifierList(_levelCompletionResults->gameplayModifiers, _levelCompletionResults->energy);
            std::string modifiers;
            for(int i = 0; i < modifiersVec.size(); ++i) {
                if(i > 0) modifiers += ',';
                modifiers += modifiersVec[i];
            }
            ReplayLoader::Load(_serializedReplay, _difficultyBeatmap, modifiers, ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.name);
        }).detach();
        watchReplayButton->set_interactable(false);
    }
}