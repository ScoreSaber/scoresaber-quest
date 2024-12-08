#include "UI/Other/ScoreInfoModal.hpp"

#include <GlobalNamespace/BeatmapCharacteristicSO.hpp>
#include <GlobalNamespace/BeatmapDifficulty.hpp>
#include <GlobalNamespace/BeatmapDifficultySerializedMethods.hpp>
#include <GlobalNamespace/IDifficultyBeatmapSet.hpp>
#include <GlobalNamespace/PlatformLeaderboardViewController.hpp>
#include <GlobalNamespace/SharedCoroutineStarter.hpp>
#include "ReplaySystem/ReplayLoader.hpp"
#include "Services/FileService.hpp"
#include "Sprites.hpp"
#include <System/DateTime.hpp>
#include <System/DayOfWeek.hpp>
#include <System/Globalization/CultureInfo.hpp>
#include <System/Int32.hpp>
#include <System/String.hpp>
#include <System/TimeSpan.hpp>
#include "UI/Other/ScoreSaberLeaderboardView.hpp"
#include <UnityEngine/RectOffset.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/SystemInfo.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/UI/HorizontalLayoutGroup.hpp>
#include <UnityEngine/UI/LayoutElement.hpp>
#include <UnityEngine/UI/VerticalLayoutGroup.hpp>
#include "Utils/BeatmapUtils.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "logging.hpp"
#include "main.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "static.hpp"
#include <map>
#include <sstream>

DEFINE_TYPE(ScoreSaber::UI::Other, ScoreInfoModal);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace GlobalNamespace;
using namespace QuestUI::BeatSaberUI;
using namespace TMPro;
using namespace System;
using namespace StringUtils;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->gameObject->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->gameObject->AddComponent<LayoutElement*>();  \
    layout##identifier->preferredWidth = width;                                          \
    layout##identifier->preferredHeight = height

#define SetFitMode(identifier, horizontal, vertical)                                            \
    auto fitter##identifier = identifier->gameObject->GetComponent<ContentSizeFitter*>(); \
    fitter##identifier->verticalFit = vertical;                                              \
    fitter##identifier->horizontalFit = horizontal

#define CreateDefaultTextAndSetSize(identifier, size)                  \
    identifier = CreateText(textVertical->transform, "", false); \
    identifier->fontSize = size;

std::string FormatNumber(int number)
{
    std::string s = std::to_string(number);

    int insertIndex = s.length() % 3;
    int count = s.length() / 3;

    if (insertIndex == 0)
    {
        insertIndex = insertIndex + 3;
        count--;
    }
    while (count > 0)
    {
        s.insert(insertIndex, ",");
        insertIndex = insertIndex + 4;

        count--;
    }

    return s;
}

// Not the ideal way to get the devices
// but ill have to make do for now
std::string GetDevice(int id)
{
    if (id == 0) { return "Unknown"; }
    if (id == 1) { return "Oculus Rift CV1"; }
    if (id == 2) { return "HTC VIVE"; }
    if (id == 4) { return "HTC VIVE Pro"; }
    if (id == 8) { return "Windows Mixed Reality"; }
    if (id == 16) { return "Oculus Rift S"; }
    if (id == 32) { return "Oculus Quest"; }
    if (id == 64) { return "Valve Index"; }
    if (id == 128) { return "HTC VIVE Cosmos"; }
    return "Unknown";
}

std::string GetUnit(std::string unit, int amount)
{
    std::string s = std::to_string(amount);
    return amount == 1 ? s + " " + unit : s + " " + unit + "s";
}

std::string GetDate(std::string date)
{
    using namespace std;

    // Time format example from scoresaber: 2021-09-18T12:48:07.000Z
    int scoreYear, scoreMonth, scoreDay, scoreHour, scoreMin, scoreSec, scoreMillisec;
    sscanf(date.c_str(), "%d-%d-%dT%d:%d:%d.%dZ", &scoreYear, &scoreMonth, &scoreDay, &scoreHour, &scoreMin, &scoreSec, &scoreMillisec);

    DateTime scoreDate = DateTime(scoreYear, scoreMonth, scoreDay, scoreHour, scoreMin, scoreSec, scoreMillisec);
    int daysInMo = DateTime::DaysInMonth(scoreYear, scoreMonth);

    DateTime now = DateTime::UtcNow;

    TimeSpan diff = TimeSpan(now.Ticks - scoreDate.Ticks);

    std::vector<std::pair<std::string, int>> times = {};

    long ticks = diff.Ticks;
    long seconds = ticks / 10000000;
    constexpr const unsigned int secondsInYear = 31536000;
    constexpr const unsigned int secondsInWeek = 604800;
    constexpr const unsigned int secondsInDay = 86400;
    constexpr const unsigned int secondsInHour = 3600;
    constexpr const unsigned int secondsInMinute = 60;
    int year = seconds / secondsInYear;
    if (year)
        times.push_back({"year", year});
    int yRemain = seconds % secondsInYear;

    int month = yRemain / (daysInMo * secondsInDay);
    if (month)
        times.push_back({"month", month});
    int mRemain = yRemain % (daysInMo * secondsInDay);

    int week = mRemain / secondsInWeek;
    if (week)
        times.push_back({"week", week});
    int wRemain = mRemain % secondsInWeek;

    int day = wRemain / secondsInDay;
    if (day)
        times.push_back({"day", day});
    int dRemain = wRemain % secondsInDay;

    int hour = dRemain / secondsInHour;
    if (hour)
        times.push_back({"hour", hour});
    int hRemain = dRemain % secondsInHour;

    int minute = hRemain / secondsInMinute;
    if (minute)
        times.push_back({"minute", minute});

    int second = hRemain % secondsInMinute;
    times.push_back({"second", second});

    stringstream s;

    for (size_t i = 0; i < min<size_t>(2, times.size()); ++i)
    {
        if (i > 0)
            s << " and ";
        s << GetUnit(times[i].first, times[i].second);
    }
    s << " ago";

    return s.str();
}

namespace ScoreSaber::UI::Other
{
    void ScoreInfoModal::Hide()
    {
        modal->Hide(true, nullptr);
    }

    void ScoreInfoModal::Show(ScoreSaber::Data::Score& score, int leaderboardId)
    {
        this->leaderboardId = leaderboardId;
        if (score.leaderboardPlayerInfo.name.has_value())
        {
            player = score.leaderboardPlayerInfo.name.value();
        }

        if (score.deviceHmd) {
            device_hmd = *score.deviceHmd;
        } else {
            device_hmd = GetDevice(score.hmd);
        }

        // Not sure if this is the best way to get the beatmap
        // but it works
        PlatformLeaderboardViewController* lb = ArrayUtil::First(Resources::FindObjectsOfTypeAll<PlatformLeaderboardViewController*>());

        currentBeatmap = lb->difficultyBeatmap;
        currentScore = score;

        SharedCoroutineStarter::instance
            ->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(BeatmapUtils::getMaxScoreCoroutine(currentBeatmap, [&](int maxScore) {
                score = score.modifiedScore, ((double)score.modifiedScore / (double)maxScore) * 100.0;
                pp = score.pp;
                if (score.maxCombo != 0) {
                    combo = score.maxCombo;
                    fullCombo = score.fullCombo;
                    badCuts = score.badCuts;
                    missedNotes = score.missedNotes;
                } else {
                    combo = nullopt;
                    fullCombo = nullopt;
                    badCuts = nullopt;
                    missedNotes = nullopt;
                }
                modifiers = score.modifiers;
                timeSet = GetDate(score.timeSet);

                if (score.leaderboardPlayerInfo.id.has_value())
                {
                    playerId = score.leaderboardPlayerInfo.id.value();
                }

                auto previewBeatmapLevel = reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(currentBeatmap->level);
                std::string levelHash = StringUtils::GetFormattedHash(previewBeatmapLevel->levelID);
                std::string characteristic = currentBeatmap->parentDifficultyBeatmapSet->beatmapCharacteristic->serializedName;
                std::string songName = previewBeatmapLevel->songName;
                std::string difficultyName = GlobalNamespace::BeatmapDifficultySerializedMethods::SerializedName(currentBeatmap->difficulty);
                replayFileName = ScoreSaber::Services::FileService::GetReplayFileName(levelHash, difficultyName, characteristic, score.leaderboardPlayerInfo.id.value(), songName);
                replayEnabled = score.hasReplay;
                modal->Show(true, true, nullptr);
                if (fileexists(ScoreSaber::Static::REPLAY_DIR + "/" + replayFileName + ".dat"))
                {
                    replayEnabled = true;
                }
                
                if(!ScoreSaberLeaderboardView::IsReplayWatchingAllowed())
                    replayEnabled = false;

                SetReplayButtonState(replayEnabled);
            })));
    }

    ScoreInfoModal* ScoreInfoModal::Create(UnityEngine::Transform* parent)
    {
        auto modal = CreateModal(parent, Vector2(55, 50), nullptr);
        auto ppmodal = modal->gameObject->AddComponent<ScoreInfoModal*>();
        ppmodal->modal = modal;
        ppmodal->Setup();
        return ppmodal;
    }

    void ScoreInfoModal::Setup()
    {
        ContentSizeFitter::FitMode pref = ContentSizeFitter::FitMode::PreferredSize;

        auto mainVertical = CreateVerticalLayoutGroup(transform);
        SetPreferredSize(mainVertical, 55.0f, 50);
        SetFitMode(mainVertical, pref, pref);
        mainVertical->padding = RectOffset::New_ctor(3, 3, 3, 3);
        mainVertical->spacing = 0.8f;

        auto headerHorizontal = CreateHorizontalLayoutGroup(mainVertical->transform);
        SetPreferredSize(headerHorizontal, 50.0f, 5.0f);
        SetFitMode(headerHorizontal, pref, pref);

        auto nameVertical = CreateVerticalLayoutGroup(headerHorizontal->transform);
        // SetPreferredSize(nameVertical, 38.0f, 5.5f);
        SetFitMode(nameVertical, pref, pref);

        auto nameHorizontal = CreateHorizontalLayoutGroup(nameVertical->transform);
        SetPreferredSize(nameHorizontal, 38.0f, 0.5f);
        SetFitMode(nameHorizontal, pref, pref);
        nameHorizontal->childAlignment = TextAnchor::MiddleLeft;
        nameHorizontal->childForceExpandWidth = false;
        nameHorizontal->spacing = 1.0f;

        player = CreateText(nameVertical->transform, "", false);
        player->overflowMode = TextOverflowModes::Ellipsis;
        player->alignment = TextAlignmentOptions::Left;
        player->fontSize = 4.0f;

        auto buttonHorizontal = CreateHorizontalLayoutGroup(headerHorizontal->transform);
        SetPreferredSize(buttonHorizontal, 12.0f * 0.9f, 5.5f * 0.9f);
        buttonHorizontal->spacing = 0.2f;

        auto userSprite = Base64ToSprite(user_base64);
        auto userImage = UIUtils::CreateClickableImage(buttonHorizontal->transform, userSprite, {0, 0}, {0, 0}, std::bind(&ScoreInfoModal::ShowPlayerProfileModal, this));
        userImage->preserveAspect = true;

        auto replaySprite = Base64ToSprite(replay_base64);
        replayImage = UIUtils::CreateClickableImage(buttonHorizontal->transform, replaySprite, {0, 0}, {0, 0}, std::bind(&ScoreInfoModal::PlayReplay, this));
        replayImage->preserveAspect = true;

        auto seperatorHorizontal = CreateHorizontalLayoutGroup(mainVertical->transform);
        auto seperatorLayout = seperatorHorizontal->gameObject->GetComponent<LayoutElement*>();
        if (seperatorLayout)
            seperatorLayout = seperatorHorizontal->gameObject->AddComponent<LayoutElement*>();

        seperatorLayout->preferredHeight = 0.4f;

        auto texture = Texture2D::whiteTexture;
        auto whiteSprite = Sprite::Create(texture, Rect(0.0f, 0.0f, (float)texture->height, (float)texture->height), Vector2(0.5f, 0.5f), 1024.0f, 1u, SpriteMeshType::FullRect, Vector4(0.0f, 0.0f, 0.0f, 0.0f), false);
        auto seperatorImage = CreateImage(seperatorHorizontal->transform, whiteSprite, {0, 0}, {0, 0});
        seperatorImage->rectTransform->sizeDelta = {48.0f, 0.4f};

        auto textVertical = CreateVerticalLayoutGroup(mainVertical->transform);
        SetPreferredSize(textVertical, 50.0f, 40.0f);
        textVertical->spacing = 0.1f;

        CreateDefaultTextAndSetSize(deviceHmd, 3.5f);
        CreateDefaultTextAndSetSize(score, 3.5f);
        CreateDefaultTextAndSetSize(pp, 3.5f);
        CreateDefaultTextAndSetSize(combo, 3.5f);
        CreateDefaultTextAndSetSize(fullCombo, 3.5f);
        CreateDefaultTextAndSetSize(badCuts, 3.5f);
        CreateDefaultTextAndSetSize(missedNotes, 3.5f);
        CreateDefaultTextAndSetSize(modifiers, 3.5f);
        CreateDefaultTextAndSetSize(timeSet, 3.5f);

        player = u"placeholder";
        device_hmd = "Unknown";
        score = 0, 0;
        pp = 0;
        combo = 0;
        fullCombo = 0;
        badCuts = 0;
        missedNotes = 0;
        modifiers = "";
        timeSet = "";
    }

    void ScoreInfoModal::player = std::u16string player
    {
        this->player->text = player + u"'s Score";
    }

    void ScoreInfoModal::device_hmd = std::string_view device
    {
        this->deviceHmd->text = string_format("<color=#6F6F6F>HMD:</color> %s", device.data());
    }

    void ScoreInfoModal::score = long score, double percent
    {
        this->score->text = string_format("<color=#6F6F6F>Score:</color> %s (<color=#ffd42a>%.2f%s</color>)", FormatNumber((int)score).c_str(), percent, "%");
    }

    void ScoreInfoModal::pp = double pp
    {
        this->pp->text = string_format("<color=#6F6F6F>Performance Points:</color> <color=#6872e5>%.2fpp</color>", pp);
    }

    void ScoreInfoModal::combo = std::optional<int> combo
    {
        if (combo)
            this->combo->text = string_format("<color=#6F6F6F>Combo:</color> %s", FormatNumber(*combo).c_str());
        else
            this->missedNotes->text = "<color=#6F6F6F>Combo:</color> N/A";
    }

    void ScoreInfoModal::fullCombo = std::optional<bool> value
    {
        if (value)
            this->fullCombo->text = string_format("<color=#6F6F6F>Full Combo:</color> %s", *value ? "<color=#13fd81>Yes</color>" : "<color=\"red\">No</color>");
        else
            this->missedNotes->text = "<color=#6F6F6F>Full Combo:</color> N/A";
    }

    void ScoreInfoModal::badCuts = std::optional<int> badCuts
    {
        if (badCuts)
            this->badCuts->text = string_format("<color=#6F6F6F>Bad Cuts:</color> %s", FormatNumber(*badCuts).c_str());
        else
            this->missedNotes->text = "<color=#6F6F6F>Bad Cuts:</color> N/A";
    }

    void ScoreInfoModal::missedNotes = std::optional<int> missedNotes
    {
        if (missedNotes)
            this->missedNotes->text = string_format("<color=#6F6F6F>Missed Notes:</color> %s", FormatNumber(*missedNotes).c_str());
        else
            this->missedNotes->text = "<color=#6F6F6F>Missed Notes:</color> N/A";
    }

    void ScoreInfoModal::modifiers = std::string_view modifiers
    {
        this->modifiers->text = string_format("<color=#6F6F6F>Modifiers:</color> %s", modifiers.data());
    }

    void ScoreInfoModal::timeSet = std::string_view timeSet
    {
        this->timeSet->text = string_format("<color=#6F6F6F>Time Set:</color> %s", timeSet.data());
    }

    void ScoreInfoModal::ShowPlayerProfileModal()
    {
        if (playerProfileModal)
        {
            Hide();
            playerProfileModal->Show(playerId);
        }
    }

    void ScoreInfoModal::SetReplayButtonState(bool enabled)
    {
        if (replayImage)
        {
            if (enabled)
            {
                // setting color doesn't seem to do anything, so I swapped the highlight colors around so that change only happens if enabled
                //replayImage->color = {1.0f, 1.0f, 1.0f, 0.8f};
                replayImage->highlightColor = {1.0f, 1.0f, 1.0f, 0.2f};
            }
            else
            {
                //replayImage->color = {1.0f, 1.0f, 1.0f, 0.2f};
                replayImage->highlightColor = {1.0f, 1.0f, 1.0f, 1.0f};
            }
        }
    }

    void ScoreInfoModal::PlayReplay()
    {
        if (replayEnabled)
        {
            SetReplayButtonState(false);
            replayEnabled = false;
            Hide();
            ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->prompt = "Loading Replay...", -1;
            ScoreSaber::ReplaySystem::ReplayLoader::GetReplayData(currentBeatmap, leaderboardId, replayFileName, currentScore, [=](bool result) {
                QuestUI::MainThreadScheduler::Schedule([=]() {
                    if (result)
                    {
                        ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->prompt = "Replay loaded!", 3;
                        ScoreSaber::ReplaySystem::ReplayLoader::StartReplay(currentBeatmap);
                    }
                    else
                    {
                        ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->prompt = "Failed to load replay", 3;
                    }
                    SetReplayButtonState(true);
                    replayEnabled = true;
                });
            });
        }
    }
} // namespace ScoreSaber::UI::Other