#include "ReplaySystem/ReplayLoader.hpp"
#include "ReplaySystem/UI/ResultsViewReplayButtonController.hpp"
#include "Services/PlayerService.hpp"
#include "Services/ReplayService.hpp"
#include "Services/UploadService.hpp"

#include <bsml/shared/BSML.hpp>
#include <custom-types/shared/delegate.hpp>
#include <bsml/shared/BSML/MainThreadScheduler.hpp>
#include "Utils/OperatorOverloads.hpp"
#include "Utils/SafePtr.hpp"
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
        didActivateDelegate = { &ResultsViewReplayButtonController::ResultsViewController_didActivateEvent, this };
        continueButtonPressedDelegate = { &ResultsViewReplayButtonController::ResultsViewController_continueButtonPressedEvent, this };
        restartButtonPressedDelegate = { &ResultsViewReplayButtonController::ResultsViewController_restartButtonPressedEvent, this };
        
        _resultsViewController->___didActivateEvent += didActivateDelegate;
        _resultsViewController->___continueButtonPressedEvent += continueButtonPressedDelegate;
        _resultsViewController->___restartButtonPressedEvent += restartButtonPressedDelegate;

        FixedSafePtr<ResultsViewReplayButtonController> self(this);
        Services::ReplayService::ReplaySerialized = [self](const std::vector<char> &v) { self->UploadDaemon_ReplaySerialized(v); };
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

    void ResultsViewReplayButtonController::ResultsViewController_restartButtonPressedEvent(UnityW<GlobalNamespace::ResultsViewController> obj)
    {
        _serializedReplay.clear();
        _replayReady = false;
    }

    void ResultsViewReplayButtonController::ResultsViewController_continueButtonPressedEvent(UnityW<GlobalNamespace::ResultsViewController> obj)
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
        FixedSafePtr<ResultsViewReplayButtonController> self(this);

        il2cpp_utils::il2cpp_aware_thread([self](){
            while(!self->_replayReady) {
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
            }
            MainThreadScheduler::Schedule([self]() {
                self->watchReplayButton->interactable = true;
            });
        }).detach();
    }

    void ResultsViewReplayButtonController::Dispose()
    {
        if(_resultsViewController) {
            _resultsViewController->___didActivateEvent -= didActivateDelegate;
            _resultsViewController->___continueButtonPressedEvent -= continueButtonPressedDelegate;
            _resultsViewController->___restartButtonPressedEvent -= restartButtonPressedDelegate;
        }
        Services::ReplayService::ReplaySerialized = nullptr;
    }

    void ResultsViewReplayButtonController::ClickedReplayButton()
    {
        FixedSafePtr<ResultsViewReplayButtonController> self(this);
        il2cpp_utils::il2cpp_aware_thread([self](){
            std::vector<std::string> modifiersVec = Services::UploadService::GetModifierList(self->_levelCompletionResults->gameplayModifiers, self->_levelCompletionResults->energy);
            std::string modifiers;
            for(int i = 0; i < modifiersVec.size(); ++i) {
                if(i > 0) modifiers += ',';
                modifiers += modifiersVec[i];
            }
            ReplayLoader::Load(self->_serializedReplay, self->_beatmapLevel, self->_beatmapKey, modifiers, ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.name);
        }).detach();
        watchReplayButton->interactable = false;
    }
}