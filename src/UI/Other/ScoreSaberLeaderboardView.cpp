
#include "UI/Other/ScoreSaberLeaderboardView.hpp"

#include "Data/InternalLeaderboard.hpp"
#include "Data/Score.hpp"
#include "Data/Private/Settings.hpp"

#include <GlobalNamespace/HMTask.hpp>
#include <GlobalNamespace/IDifficultyBeatmap.hpp>
#include <GlobalNamespace/IPreviewBeatmapLevel.hpp>
#include <GlobalNamespace/LoadingControl.hpp>
#include <GlobalNamespace/PlatformLeaderboardViewController.hpp>
#include <GlobalNamespace/PlatformLeaderboardsHandler.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel_GetScoresCompletionHandler.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel_GetScoresResult.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel_LeaderboardScore.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel_ScoresScope.hpp>
#include <GlobalNamespace/SharedCoroutineStarter.hpp>
#include <GlobalNamespace/StandardLevelDetailView.hpp>
#include <GlobalNamespace/StandardLevelDetailViewController.hpp>
#include "Utils/BeatmapUtils.hpp"

#include "Services/UploadService.hpp"

#include <HMUI/CurvedCanvasSettingsHelper.hpp>
#include <HMUI/CurvedTextMeshPro.hpp>
#include <HMUI/IconSegmentedControl.hpp>
#include <HMUI/IconSegmentedControl_DataItem.hpp>
#include <HMUI/ImageView.hpp>
#include <HMUI/Screen.hpp>
#include <HMUI/StackLayoutGroup.hpp>
#include <HMUI/ViewController_AnimationDirection.hpp>
#include <HMUI/ViewController_AnimationType.hpp>

#include "Services/LeaderboardService.hpp"
#include "Services/PlayerService.hpp"

#include "Sprites.hpp"
#include <TMPro/TextMeshProUGUI.hpp>
#include "UI/FlowCoordinators/ScoreSaberFlowCoordinator.hpp"

#include "UI/Other/PanelView.hpp"
#include "UI/Other/ProfilePictureView.hpp"
#include <System/Action.hpp>
#include <System/Threading/CancellationTokenSource.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Rect.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/SpriteRenderer.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/UI/Button.hpp>
#include "Utils/StringUtils.hpp"
#include "Utils/UIUtils.hpp"
#include <beatsaber-hook/shared/utils/hooking.hpp>
#include <bsml/shared/Helpers/utilities.hpp>
#include <custom-types/shared/delegate.hpp>
#include "hooks.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "questui/QuestUI.hpp"
#include <chrono>

using namespace HMUI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace GlobalNamespace;
using namespace ScoreSaber;
using namespace StringUtils;
using namespace ScoreSaber::CustomTypes;
using namespace ScoreSaber::UI::FlowCoordinators;
using namespace ScoreSaber::Services;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::UI::Other::ScoreSaberLeaderboardView
{
    ScoreSaber::UI::Other::Banner* ScoreSaberBanner;

    ScoreSaber::UI::Other::PanelView* view;
    ScoreSaber::CustomTypes::Components::LeaderboardScoreInfoButtonHandler* leaderboardScoreInfoButtonHandler;

    PlatformLeaderboardViewController* _platformLeaderboardViewController;

    UnityEngine::UI::Button* _pageUpButton;
    UnityEngine::UI::Button* _pageDownButton;

    std::vector<ProfilePictureView> _ImageHolders;

    std::vector<HMUI::ImageView*> _cellClickingImages;

    SafePtr<System::Threading::CancellationTokenSource> cancellationToken;

    bool _activated = false;

    int _lastCell = 0;
    int _leaderboardPage = 1;
    bool _filterAroundCountry = false;
    std::string _currentLeaderboardRefreshId;
    
    bool _allowReplayWatching = true;

    void OnSoftRestart()
    {
        _activated = false;
        int _lastCell = 0;
        int _leaderboardPage = 1;
        _filterAroundCountry = false;
        _currentLeaderboardRefreshId.clear();
        _allowReplayWatching = true;

        ScoreSaberBanner = nullptr;
        view = nullptr;
        leaderboardScoreInfoButtonHandler = nullptr;
        _platformLeaderboardViewController = nullptr;
        _pageUpButton = nullptr;
        _pageDownButton = nullptr;

        _ImageHolders.clear();
        _cellClickingImages.clear();
    }

    void EarlyDidActivate(PlatformLeaderboardViewController* self, bool firstActivation, bool addedToHeirarchy,
                     bool screenSystemEnabling)
    {
        _leaderboardPage = 1;
    }

    void DidActivate(PlatformLeaderboardViewController* self, bool firstActivation, bool addedToHeirarchy,
                     bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            INFO("PlatformLeaderboardViewController firstActivation");

            StandardLevelDetailViewController* _standardLevelDetailViewController = ArrayUtil::First(Resources::FindObjectsOfTypeAll<StandardLevelDetailViewController*>());

            _platformLeaderboardViewController = self;

            // Page Buttons

            if (!_pageUpButton)
            {
                _pageUpButton = QuestUI::BeatSaberUI::CreateUIButton(self->transform, "", "SettingsButton", Vector2(-40.0f, 20.0f), Vector2(5.0f, 5.0f),
                                                                     [=]() {
                                                                         DirectionalButtonClicked(PageDirection::Up);
                                                                     });

                QuestUI::BeatSaberUI::SetButtonSprites(_pageUpButton, Base64ToSprite(carat_up_inactive_base64),
                                                       Base64ToSprite(carat_up_base64));

                RectTransform* rectTransform = reinterpret_cast<RectTransform*>(_pageUpButton->transform->GetChild(0));
                rectTransform->sizeDelta = {10.0f, 10.0f};
            }

            if (!_pageDownButton)
            {
                _pageDownButton = QuestUI::BeatSaberUI::CreateUIButton(self->transform, "", "SettingsButton", Vector2(-40.0f, -20.0f), Vector2(5.0f, 5.0f),
                                                                       [=]() {
                                                                           DirectionalButtonClicked(PageDirection::Down);
                                                                       });

                QuestUI::BeatSaberUI::SetButtonSprites(_pageDownButton, Base64ToSprite(carat_down_inactive_base64),
                                                       Base64ToSprite(carat_down_base64));
                RectTransform* rectTransform = reinterpret_cast<RectTransform*>(_pageDownButton->transform->GetChild(0));
                rectTransform->sizeDelta = {10.0f, 10.0f};
            }

            // RedBrumbler top panel

            ScoreSaberBanner = ::ScoreSaber::UI::Other::Banner::Create(self->transform);
            auto playerProfileModal = ::ScoreSaber::UI::Other::PlayerProfileModal::Create(self->transform);
            ScoreSaberBanner->playerProfileModal = playerProfileModal;

            ScoreSaberBanner->Prompt("Signing into ScoreSaber...", false, 5.0f, nullptr);
            auto newGo = GameObject::New_ctor();
            auto t = newGo->transform;
            t->transform->SetParent(self->transform, false);
            t->localScale = {1, 1, 1};

            leaderboardScoreInfoButtonHandler = newGo->AddComponent<ScoreSaber::CustomTypes::Components::LeaderboardScoreInfoButtonHandler*>();
            leaderboardScoreInfoButtonHandler->Setup();
            leaderboardScoreInfoButtonHandler->scoreInfoModal->playerProfileModal = playerProfileModal;

            // profile pictures
            auto pfpVertical = CreateVerticalLayoutGroup(self->transform);
            pfpVertical->rectTransform->anchoredPosition = Vector2(-21, -1);
            pfpVertical->spacing = -20.15;

            auto nullSprite = BSML::Utilities::ImageResources::GetBlankSprite();

            _ImageHolders.clear();
            for (int i = 0; i < 10; ++i) {
                auto rowHorizontal = CreateHorizontalLayoutGroup(pfpVertical->transform);
                rowHorizontal->childForceExpandHeight = true;
                rowHorizontal->childAlignment = TextAnchor::MiddleCenter;

                auto rowStack = UIUtils::CreateStackLayoutGroup(rowHorizontal->transform);
                
                auto image = CreateImage(rowStack->transform, nullSprite, {0.0f, 0.0f}, {4.75f, 4.75f});
                image->preserveAspect = true;
                auto imageLayout = image->gameObject->GetComponent<UnityEngine::UI::LayoutElement*>();
                imageLayout->preferredHeight = 4.75f;
                imageLayout->preferredWidth = 4.75f;

                auto loadingIndicator = UIUtils::CreateLoadingIndicator(rowStack->transform);
                auto loadingIndicatorLayout = loadingIndicator->gameObject->GetComponent<UnityEngine::UI::LayoutElement*>();
                loadingIndicatorLayout->preferredWidth = 3.75f;
                loadingIndicatorLayout->preferredHeight = 3.75f;
                // missing: set preserveAspect to true, but not sure where to set this (or if it is even needed, but the PC plugin does it)
                loadingIndicator->SetActive(false);

                _ImageHolders.emplace_back(image, loadingIndicator);
                _ImageHolders.back().Parsed();
            }

            // cell clickers
            auto clickVertical = CreateVerticalLayoutGroup(self->transform);
            clickVertical->rectTransform->anchoredPosition = Vector2(5, -1);
            clickVertical->spacing = -20.25;

            auto mat_UINoGlowRoundEdge = ArrayUtil::First(Resources::FindObjectsOfTypeAll<Material*>(), [](Material* x) { return Paper::StringConvert::from_utf16(x->name) == "UINoGlowRoundEdge"; });

            _cellClickingImages.clear();
            for (int i = 0; i < 10; ++i) {
                auto rowHorizontal = CreateHorizontalLayoutGroup(clickVertical->transform);
                rowHorizontal->childForceExpandHeight = true;
                rowHorizontal->childAlignment = TextAnchor::MiddleCenter;

                auto rowStack = UIUtils::CreateStackLayoutGroup(rowHorizontal->transform);
                
                auto image = CreateImage(rowStack->transform, nullSprite, {0.0f, 0.0f}, {72.0f, 5.75f});
                image->material = mat_UINoGlowRoundEdge;
                image->preserveAspect = true;
                auto imageLayout = image->gameObject->GetComponent<UnityEngine::UI::LayoutElement*>();
                imageLayout->preferredWidth = 72.0f;
                imageLayout->preferredHeight = 5.75f;

                _cellClickingImages.emplace_back(image);
            }

            PlayerService::AuthenticateUser([&](PlayerService::LoginStatus loginStatus) {
                switch (loginStatus)
                {
                    case PlayerService::LoginStatus::Success: {
                        ScoreSaberBanner->Prompt("<color=#89fc81>Successfully signed in to ScoreSaber</color>", false, 5.0f,
                                                 nullptr);
                                                 INFO("Refresh 1");
                        _platformLeaderboardViewController->Refresh(true, true);
                        break;
                    }
                    case PlayerService::LoginStatus::Error: {
                        ScoreSaberBanner->Prompt("<color=#fc8181>Authentication failed</color>", false, 5.0f, nullptr);
                        break;
                    }
                }
            });
        }

        // we have to set this up again, because locationFilterMode could have changed
        Sprite* globalLeaderboardIcon = self->globalLeaderboardIcon;
        Sprite* friendsLeaderboardIcon = self->friendsLeaderboardIcon;
        Sprite* aroundPlayerLeaderboardIcon = self->aroundPlayerLeaderboardIcon;
        Sprite* countryLeaderboardIcon = Base64ToSprite(country_base64);
        countryLeaderboardIcon->textureRect.size = {64.0f, 64.0f};

        IconSegmentedControl* scopeSegmentedControl = self->scopeSegmentedControl;

        std::string mode = Settings::locationFilterMode;
        for (auto& c : mode)
        {
            c = tolower(c);
        }

        ::Array<IconSegmentedControl::DataItem*>* array = ::Array<IconSegmentedControl::DataItem*>::New({
            IconSegmentedControl::DataItem::New_ctor(globalLeaderboardIcon, "Global"),
            IconSegmentedControl::DataItem::New_ctor(aroundPlayerLeaderboardIcon, "Around You"),
            IconSegmentedControl::DataItem::New_ctor(friendsLeaderboardIcon, "Friends"),
            IconSegmentedControl::DataItem::New_ctor(countryLeaderboardIcon, mode == "region" ? "Region" : "Country"),
        });

        scopeSegmentedControl->SetData(array);

        _activated = true;
    }

    void DidDeactivate()
    {
        ScoreSaberBanner->playerProfileModal->Hide();
        leaderboardScoreInfoButtonHandler->scoreInfoModal->Hide();
    }

    void ByeImages()
    {
        for (auto &holder : _ImageHolders) {
            holder.ClearSprite();
        }
    }

    void RefreshLeaderboard(BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, LeaderboardTableView* tableView,
                            PlatformLeaderboardsModel::ScoresScope scope, LoadingControl* loadingControl,
                            std::string refreshId)
    {
        if (ScoreSaber::Services::UploadService::uploading)
        {
            return;
        }

        if (!_activated)
        {
            return;
        }

        if (scope == PlatformLeaderboardsModel::ScoresScope::AroundPlayer && !_filterAroundCountry) {
            _pageUpButton->interactable = false;
            _pageDownButton->interactable = false;
        } else {
            _pageUpButton->interactable = true;
            _pageDownButton->interactable = true;
        }

        ByeImages();

        if (cancellationToken) {
            cancellationToken->Cancel();
            cancellationToken->Dispose();
        }
        cancellationToken = System::Threading::CancellationTokenSource::New_ctor();

        if (PlayerService::playerInfo.loginStatus == PlayerService::LoginStatus::Error)
        {
            SetErrorState(loadingControl, "ScoreSaber authentication failed, please restart Beat Saber", false);
            ByeImages();
            return;
        }

        if (PlayerService::playerInfo.loginStatus != PlayerService::LoginStatus::Success)
        {
            return;
        }

        _currentLeaderboardRefreshId = refreshId;

        HMTask::New_ctor(custom_types::MakeDelegate<System::Action*>((std::function<void()>)[difficultyBeatmap, scope, loadingControl, tableView, refreshId]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (_currentLeaderboardRefreshId == refreshId)
            {
                LeaderboardService::GetLeaderboardData(
                    difficultyBeatmap, scope, _leaderboardPage,
                    [=](Data::InternalLeaderboard internalLeaderboard) {
                        QuestUI::MainThreadScheduler::Schedule([=]() {
                            if (_currentLeaderboardRefreshId != refreshId) {
                                return; // we need to check this again, since some time may have passed due to waiting for leaderboard data
                            }
                            if (internalLeaderboard.leaderboard.has_value())
                            {
                                SetRankedStatus(internalLeaderboard.leaderboard->leaderboardInfo);
                                int playerScoreIndex = GetPlayerScoreIndex(internalLeaderboard.leaderboard.value().scores);
                                if (internalLeaderboard.leaderboardItems->Count != 0)
                                {
                                    if (scope == PlatformLeaderboardsModel::ScoresScope::AroundPlayer && playerScoreIndex == -1 && !_filterAroundCountry)
                                    {
                                        SetErrorState(loadingControl, "You haven't set a score on this leaderboard", true);
                                    }
                                    else
                                    {
                                        tableView->SetScores(internalLeaderboard.leaderboardItems, playerScoreIndex);
                                        for (int i = 0; i < internalLeaderboard.profilePictures.size(); ++i) {
                                            _ImageHolders[i].SetProfileImage(internalLeaderboard.profilePictures[i], i, cancellationToken->Token);
                                        }
                                        loadingControl->ShowText(System::String::_Empty, false);
                                        loadingControl->Hide();
                                        leaderboardScoreInfoButtonHandler->scoreCollection = internalLeaderboard.leaderboard.value().scores, internalLeaderboard.leaderboard->leaderboardInfo.id;
                                    }
                                }
                                else
                                {
                                    if (_leaderboardPage > 1)
                                    {
                                        SetErrorState(loadingControl, "No scores on this page");
                                    }
                                    else
                                    {
                                        SetErrorState(loadingControl, "No scores on this leaderboard, be the first!");
                                    }
                                    ByeImages();
                                }
                            }
                            else
                            {
                                if (internalLeaderboard.leaderboardItems->get_Item(0) != nullptr)
                                {
                                    SetErrorState(loadingControl, internalLeaderboard.leaderboardItems->get_Item(0)->playerName, false);
                                }
                                else
                                {
                                    SetErrorState(loadingControl, "No scores on this leaderboard, be the first! 0x1");
                                }
                                ByeImages();
                            }
                        });
                    },
                    _filterAroundCountry);
            }
        }), nullptr)->Run();
    }

    void SetRankedStatus(Data::LeaderboardInfo leaderboardInfo)
    {
        if (leaderboardInfo.ranked)
        {
            if (leaderboardInfo.positiveModifiers)
            {
                ScoreSaberBanner->status = "Ranked (DA = +0.02, GN +0.04)", leaderboardInfo.id;
            }
            else
            {
                ScoreSaberBanner->status = "Ranked (modifiers disabled)", leaderboardInfo.id;
            }
            return;
        }

        if (leaderboardInfo.qualified)
        {
            ScoreSaberBanner->status = "Qualified", leaderboardInfo.id;
            return;
        }

        if (leaderboardInfo.loved)
        {
            ScoreSaberBanner->status = "Loved", leaderboardInfo.id;
            return;
        }
        ScoreSaberBanner->status = "Unranked", leaderboardInfo.id;
    }

    int GetPlayerScoreIndex(std::vector<Data::Score> scores)
    {
        for (int i = 0; i < scores.size(); i++)
        {
            if (scores[i].leaderboardPlayerInfo.id == PlayerService::playerInfo.localPlayerData.id)
            {
                return i;
            }
        }
        return -1;
    }

    void SetErrorState(LoadingControl* loadingControl, std::string errorText, bool showRefreshButton)
    {
        loadingControl->Hide();
        loadingControl->ShowText(errorText, showRefreshButton);
    }
    
    void RefreshLeaderboard()
    {
        if (_activated)
        {
            _platformLeaderboardViewController->Refresh(true, true);
        }
    }

    void ChangeScope(bool filterAroundCountry)
    {
        if (_activated)
        {
            _leaderboardPage = 1;
            _filterAroundCountry = filterAroundCountry;
            _platformLeaderboardViewController->Refresh(true, true);
            CheckPage();
        }
    }

    void CheckPage()
    {
        if (_leaderboardPage > 1)
        {
            _pageUpButton->interactable = true;
        }
        else
        {
            _pageUpButton->interactable = false;
        }
    }

    void DirectionalButtonClicked(PageDirection direction)
    {
        switch (direction)
        {
            case PageDirection::Up: {
                _leaderboardPage--;
                break;
            }
            case PageDirection::Down: {
                _leaderboardPage++;
                break;
            }
        }
        _platformLeaderboardViewController->Refresh(true, true);
        CheckPage();
    }

    void SetUploadState(bool state, bool success, std::string errorMessage)
    {
        QuestUI::MainThreadScheduler::Schedule([=]() {
            if (state)
            {
                _platformLeaderboardViewController->loadingControl->ShowLoading(System::String::_Empty);
                ScoreSaberBanner->loading = true;
                ScoreSaberBanner->prompt = "Uploading score...", -1;
                // ScoreSaberBanner->Prompt("Uploading Score", true, 5.0f, nullptr);
            }
            else
            {
                _platformLeaderboardViewController->Refresh(true, true);

                if (success)
                {
                    ScoreSaberBanner->prompt = "<color=#89fc81>Score uploaded successfully</color>", 5;
                    PlayerService::UpdatePlayerInfo(true);
                }
                else
                {
                    ScoreSaberBanner->prompt = errorMessage, 5;
                }
            }
        });
    }

    void AllowReplayWatching(bool value)
    {
        _allowReplayWatching = value;
    }

    bool IsReplayWatchingAllowed()
    {
        return _allowReplayWatching;
    }

} // namespace ScoreSaber::UI::Other::ScoreSaberLeaderboardView
