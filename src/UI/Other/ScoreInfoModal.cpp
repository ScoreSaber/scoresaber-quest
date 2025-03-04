#include "UI/Other/ScoreInfoModal.hpp"

#include <GlobalNamespace/BeatmapCharacteristicSO.hpp>
#include <GlobalNamespace/BeatmapDifficulty.hpp>
#include <GlobalNamespace/BeatmapDifficultySerializedMethods.hpp>
#include <GlobalNamespace/PlatformLeaderboardViewController.hpp>
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
#include <UnityEngine/UI/ContentSizeFitter.hpp>
#include <UnityEngine/UI/LayoutElement.hpp>
#include <UnityEngine/UI/VerticalLayoutGroup.hpp>
#include <bsml/shared/BSML/MainThreadScheduler.hpp>
#include "Utils/StringUtils.hpp"
#include "static.hpp"
#include <sstream>

DEFINE_TYPE(ScoreSaber::UI::Other, ScoreInfoModal);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace GlobalNamespace;
using namespace TMPro;
using namespace System;
using namespace StringUtils;
using namespace BSML;
using namespace BSML::Lite;

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
    identifier = CreateText(textVertical->transform, "", FontStyles::Normal); \
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

    DateTime scoreDate;
    // manual ctor call, ugly af, but apparently the only way now with cordl :(
    scoreDate._ctor(scoreYear, scoreMonth, scoreDay, scoreHour, scoreMin, scoreSec, scoreMillisec);
    int daysInMo = DateTime::DaysInMonth(scoreYear, scoreMonth);

    DateTime now = DateTime::get_UtcNow();

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

    void ScoreInfoModal::Show(ScoreSaber::Data::Score& score, BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, int leaderboardId, int maxScore)
    {
        this->leaderboardId = leaderboardId;
        if (score.leaderboardPlayerInfo.name.has_value())
        {
            set_player(score.leaderboardPlayerInfo.name.value());
        }

        if (score.deviceHmd) {
            set_device_hmd(*score.deviceHmd);
        } else {
            set_device_hmd(GetDevice(score.hmd));
        }

        // Not sure if this is the best way to get the beatmap
        // but it works
        PlatformLeaderboardViewController* lb = Resources::FindObjectsOfTypeAll<PlatformLeaderboardViewController*>()->First();

        currentScore = score;
        currentBeatmapLevel = beatmapLevel;
        currentBeatmapKey = beatmapKey;

        set_score(score.modifiedScore, ((double)score.baseScore / (double)maxScore) * 100.0);
        set_pp(score.pp);
        if (score.maxCombo != 0) {
            set_combo(score.maxCombo);
            set_fullCombo(score.fullCombo);
            set_badCuts(score.badCuts);
            set_missedNotes(score.missedNotes);
        } else {
            set_combo(nullopt);
            set_fullCombo(nullopt);
            set_badCuts(nullopt);
            set_missedNotes(nullopt);
        }
        set_modifiers(score.modifiers);
        set_timeSet(GetDate(score.timeSet));

        if (score.leaderboardPlayerInfo.id.has_value())
        {
            playerId = score.leaderboardPlayerInfo.id.value();
        }

        std::string levelHash = StringUtils::GetFormattedHash(currentBeatmapKey.levelId);
        std::string characteristic = currentBeatmapKey.beatmapCharacteristic->serializedName;
        std::string songName = currentBeatmapLevel->songName;
        std::string difficultyName = GlobalNamespace::BeatmapDifficultySerializedMethods::SerializedName(currentBeatmapKey.difficulty);
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

        player = CreateText(nameVertical->transform, "", FontStyles::Normal);
        player->overflowMode = TextOverflowModes::Ellipsis;
        player->alignment = TextAlignmentOptions::Left;
        player->fontSize = 4.0f;

        auto buttonHorizontal = CreateHorizontalLayoutGroup(headerHorizontal->transform);
        SetPreferredSize(buttonHorizontal, 12.0f * 0.9f, 5.5f * 0.9f);
        buttonHorizontal->spacing = 0.2f;

        auto userSprite = Base64ToSprite(user_base64);
        auto userImage = CreateClickableImage(buttonHorizontal->transform, userSprite, std::bind(&ScoreInfoModal::ShowPlayerProfileModal, this), {0, 0}, {0, 0});
        userImage->preserveAspect = true;

        auto replaySprite = Base64ToSprite(replay_base64);
        replayImage = CreateClickableImage(buttonHorizontal->transform, replaySprite, std::bind(&ScoreInfoModal::PlayReplay, this), {0, 0}, {0, 0});
        replayImage->preserveAspect = true;

        auto seperatorHorizontal = CreateHorizontalLayoutGroup(mainVertical->transform);
        auto seperatorLayout = seperatorHorizontal->gameObject->GetComponent<LayoutElement*>();
        if (seperatorLayout)
            seperatorLayout = seperatorHorizontal->gameObject->AddComponent<LayoutElement*>();

        seperatorLayout->preferredHeight = 0.4f;

        auto texture = Texture2D::get_whiteTexture();
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

        set_player(u"placeholder");
        set_device_hmd("Unknown");
        set_score(0, 0);
        set_pp(0);
        set_combo(0);
        set_fullCombo(0);
        set_badCuts(0);
        set_missedNotes(0);
        set_modifiers("");
        set_timeSet("");
    }

    void ScoreInfoModal::set_player(std::u16string player)
    {
        this->player->text = player + u"'s Score";
    }

    void ScoreInfoModal::set_device_hmd(std::string_view device)
    {
        this->deviceHmd->text = fmt::format("<color=#6F6F6F>HMD:</color> {:s}", device.data());
    }

    void ScoreInfoModal::set_score(long score, double percent)
    {
        this->score->text = fmt::format("<color=#6F6F6F>Score:</color> {:s} (<color=#ffd42a>{:.2f}{:s}</color>)", FormatNumber((int)score).c_str(), percent, "%");
    }

    void ScoreInfoModal::set_pp(double pp)
    {
        this->pp->text = fmt::format("<color=#6F6F6F>Performance Points:</color> <color=#6872e5>{:.2f}pp</color>", pp);
    }

    void ScoreInfoModal::set_combo(std::optional<int> combo)
    {
        if (combo)
            this->combo->text = fmt::format("<color=#6F6F6F>Combo:</color> {:s}", FormatNumber(*combo).c_str());
        else
            this->missedNotes->text = "<color=#6F6F6F>Combo:</color> N/A";
    }

    void ScoreInfoModal::set_fullCombo(std::optional<bool> value)
    {
        if (value)
            this->fullCombo->text = fmt::format("<color=#6F6F6F>Full Combo:</color> {:s}", *value ? "<color=#13fd81>Yes</color>" : "<color=\"red\">No</color>");
        else
            this->missedNotes->text = "<color=#6F6F6F>Full Combo:</color> N/A";
    }

    void ScoreInfoModal::set_badCuts(std::optional<int> badCuts)
    {
        if (badCuts)
            this->badCuts->text = fmt::format("<color=#6F6F6F>Bad Cuts:</color> {:s}", FormatNumber(*badCuts).c_str());
        else
            this->missedNotes->text = "<color=#6F6F6F>Bad Cuts:</color> N/A";
    }

    void ScoreInfoModal::set_missedNotes(std::optional<int> missedNotes)
    {
        if (missedNotes)
            this->missedNotes->text = fmt::format("<color=#6F6F6F>Missed Notes:</color> {:s}", FormatNumber(*missedNotes).c_str());
        else
            this->missedNotes->text = "<color=#6F6F6F>Missed Notes:</color> N/A";
    }

    void ScoreInfoModal::set_modifiers(std::string_view modifiers)
    {
        this->modifiers->text = fmt::format("<color=#6F6F6F>Modifiers:</color> {:s}", modifiers.data());
    }

    void ScoreInfoModal::set_timeSet(std::string_view timeSet)
    {
        this->timeSet->text = fmt::format("<color=#6F6F6F>Time Set:</color> {:s}", timeSet.data());
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
            ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->set_prompt("Loading Replay...", -1);
            ScoreSaber::ReplaySystem::ReplayLoader::GetReplayData(currentBeatmapLevel, currentBeatmapKey, leaderboardId, replayFileName, currentScore, [=, this](bool result) {
                MainThreadScheduler::Schedule([=, this]() {
                    if (result)
                    {
                        ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->set_prompt("Replay loaded!", 3);
                        ScoreSaber::ReplaySystem::ReplayLoader::StartReplay(currentBeatmapLevel, currentBeatmapKey);
                    }
                    else
                    {
                        ScoreSaber::UI::Other::ScoreSaberLeaderboardView::ScoreSaberBanner->set_prompt("Failed to load replay", 3);
                    }
                    SetReplayButtonState(true);
                    replayEnabled = true;
                });
            });
        }
    }
} // namespace ScoreSaber::UI::Other