#include "CustomTypes/Components/GlobalLeaderboardTableCell.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/Touchable.hpp"

#include "UnityEngine/Networking/DownloadHandlerTexture.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestTexture.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/Texture2D.hpp"

#include "Sprites.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/WebUtils.hpp"

#include "logging.hpp"
#include "main.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "questui/shared/QuestUI.hpp"

DEFINE_TYPE(ScoreSaber::CustomTypes::Components, GlobalLeaderboardTableCell);

using namespace ScoreSaber::CustomTypes::Components;
using namespace StringUtils;

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace TMPro;

using LeaderboardType = ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType;

#define BeginCoroutine(method)                                               \
    GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine( \
        custom_types::Helpers::CoroutineHelper::New(method));

void GlobalLeaderboardTableCell::ctor()
{
}

VerticalLayoutGroup* CreateHost(Transform* parent, Vector2 anchoredPos,
                                Vector2 size)
{
    VerticalLayoutGroup* group = BeatSaberUI::CreateVerticalLayoutGroup(parent);
    group->get_rectTransform()->set_anchoredPosition(anchoredPos);

    LayoutElement* elem = group->GetComponent<LayoutElement*>();
    elem->set_preferredHeight(size.y);
    elem->set_preferredWidth(size.x);

    ContentSizeFitter* fitter = group->GetComponent<ContentSizeFitter*>();
    fitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
    fitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
    return group;
}

std::string flag_url(std::string_view COUNTRY)
{
    auto country = std::string(COUNTRY);
    for (auto& c : country)
    {
        c = tolower(c);
    }
    auto url = string_format("https://github.com/hampusborgos/country-flags/raw/main/png250px/%s.png", country.c_str());
    return url;
}

void GlobalLeaderboardTableCell::Refresh(ScoreSaber::Data::Player& player, LeaderboardType leaderboardType)
{
    stopProfileRoutine();
    stopFlagRoutine();

    profile->set_sprite(Base64ToSprite(oculus_base64));

    // if it ends with oculus.png then there is no reason to redownload the image, so let's not redownload it :)
    if (!player.profilePicture.ends_with("oculus.png"))
    {
        if (player.profilePicture.ends_with(".gif"))
        {
            profileRoutine = BeginCoroutine(WebUtils::WaitForGifDownload(player.profilePicture, profile));
        }
        else
        {
            profileRoutine = BeginCoroutine(WebUtils::WaitForImageDownload(player.profilePicture, profile));
        }
    }

    this->name->set_text(player.name);

    if (leaderboardType == LeaderboardType::Global || leaderboardType == LeaderboardType::AroundYou)
    {
        this->rank->set_text(string_format("#%d", player.rank));
    }
    else
    {
        this->rank->set_text(string_format("#%d (#%d)", player.countryRank, player.rank));
    }

    this->pp->set_text(string_format("<color=#6872e5>%.0fpp</color>", player.pp));
    flag->set_sprite(Base64ToSprite(country_base64));
    flagRoutine = BeginCoroutine(WebUtils::WaitForImageDownload(flag_url(player.country), flag));
    this->country->set_text(player.country);

    auto& histories = player.histories;
    auto length = histories.size();
    int weeklyChange;
    if (length == 0)
    {
        weeklyChange = 0;
    }
    else if (length < 8)
    {
        // if we have less than a week of history, the history is just first minus last
        weeklyChange = histories[0] - histories[length - 1];
    }
    else
    {
        weeklyChange = histories[length - 8] - histories[length - 1];
    }

    std::string result;
    if (weeklyChange > 0)
    {
        result = string_format("<color=green>+%d</color>", weeklyChange);
    }
    else if (weeklyChange < 0)
    {
        result = string_format("<color=red>%d</color>", weeklyChange);
    }
    else
    {
        result = string_format("%d", weeklyChange);
    }
    weekly->set_text(result);

    playerId = player.id;
}

GlobalLeaderboardTableCell* GlobalLeaderboardTableCell::CreateCell()
{
    auto cellGO = UnityEngine::GameObject::New_ctor();
    auto playerCell = cellGO->AddComponent<GlobalLeaderboardTableCell*>();
    cellGO->set_name("GlobalLeaderboardTableCell");

    cellGO->AddComponent<HMUI::Touchable*>();
    playerCell->set_interactable(false);

    auto verticalLayoutGroup = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(
        playerCell->get_transform());

    auto layout = verticalLayoutGroup->get_gameObject()->GetComponent<UnityEngine::UI::LayoutElement*>();
    layout->set_preferredHeight(11.0f);
    layout->set_preferredWidth(200.0f);

    Transform* t = playerCell->get_transform();

    playerCell->profile = UIUtils::CreateClickableImage(
        CreateHost(t, {-45.0f, 0.0f}, {10.0f, 10.0f})->get_transform(),
        Base64ToSprite(oculus_base64), {0.0f, 0.0f},
        {10.0f, 10.0f}, std::bind(&GlobalLeaderboardTableCell::OpenPlayerProfileModal, playerCell));

    playerCell->name = UIUtils::CreateClickableText(
        CreateHost(t, {-11.0f, 2.8f}, {55.0f, 8.0f})->get_transform(),
        u"Username", {0.0f, 0.0f}, {0.0f, 0.0f}, std::bind(&GlobalLeaderboardTableCell::OpenPlayerProfileModal, playerCell));

    playerCell->name->set_overflowMode(TextOverflowModes::Ellipsis);
    playerCell->name->set_alignment(TextAlignmentOptions::Left);
    playerCell->name->set_fontSize(5.0f);

    playerCell->rank = BeatSaberUI::CreateText(
        CreateHost(t, {-18.0f, -2.0f}, {40.0f, 8.0f})->get_transform(),
        "#---", false,
        {0.0f, 0.0f});

    playerCell->rank->set_alignment(TextAlignmentOptions::Left);

    playerCell->pp = BeatSaberUI::CreateText(
        CreateHost(t, {27.0f, 0.0f}, {20.0f, 11.0f})->get_transform(),
        "---<color=#6872e5>pp</color>", false, {0.0f, 0.0f});

    playerCell->pp->set_fontSize(5.0f);
    playerCell->pp->set_overflowMode(TextOverflowModes::Ellipsis);

    playerCell->flag = BeatSaberUI::CreateImage(
        CreateHost(t, {19.42f, -1.65f}, {4.0f, 3.0f})->get_transform(),
        Base64ToSprite(country_base64), {0.0f, 0.0f},
        {4.0, 3.0f});

    playerCell->flag->set_preserveAspect(true);

    playerCell->country = BeatSaberUI::CreateText(
        CreateHost(t, {31.0f, -2.0f}, {17.0f, 0.0f})->get_transform(), "N/A", false, {0.0f, 0.0f});
    playerCell->country->set_alignment(TextAlignmentOptions::Left);
    playerCell->country->set_fontSize(3.5f);

    playerCell->weekly = BeatSaberUI::CreateText(
        CreateHost(t, {41.0f, -1.0f}, {15.0f, 0.0f})->get_transform(), "0", false,
        {0.0f, 0.0f});

    playerCell->weekly->set_alignment(TextAlignmentOptions::Right);
    playerCell->weekly->set_fontSize(5.0f);

    return playerCell;
}

void GlobalLeaderboardTableCell::stopProfileRoutine()
{
    if (profileRoutine)
    {
        GlobalNamespace::SharedCoroutineStarter::get_instance()->StopCoroutine(profileRoutine);
    }
    profileRoutine = nullptr;
}

void GlobalLeaderboardTableCell::stopFlagRoutine()
{
    if (flagRoutine)
    {
        GlobalNamespace::SharedCoroutineStarter::get_instance()->StopCoroutine(flagRoutine);
    }
    flagRoutine = nullptr;
}

void GlobalLeaderboardTableCell::OpenPlayerProfileModal()
{
    if (playerProfileModal)
    {
        playerProfileModal->Show(playerId);
    }
}
