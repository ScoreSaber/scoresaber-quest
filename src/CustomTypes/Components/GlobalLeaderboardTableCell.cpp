#include "CustomTypes/Components/GlobalLeaderboardTableCell.hpp"

#include <HMUI/ImageView.hpp>
#include <HMUI/Touchable.hpp>

#include <UnityEngine/Networking/DownloadHandlerTexture.hpp>
#include <UnityEngine/Networking/UnityWebRequest.hpp>
#include <UnityEngine/Networking/UnityWebRequestTexture.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/UI/ContentSizeFitter.hpp>
#include <UnityEngine/UI/LayoutElement.hpp>
#include <bsml/shared/BSML-Lite.hpp>
#include <bsml/shared/BSML/SharedCoroutineStarter.hpp>

#include "Sprites.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/WebUtils.hpp"

#include "logging.hpp"
#include "main.hpp"

DEFINE_TYPE(ScoreSaber::CustomTypes::Components, GlobalLeaderboardTableCell);

using namespace ScoreSaber::CustomTypes::Components;
using namespace StringUtils;

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace TMPro;
using namespace BSML;
using namespace BSML::Lite;

using LeaderboardType = ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType;

#define BeginCoroutine(method) SharedCoroutineStarter::StartCoroutine(custom_types::Helpers::CoroutineHelper::New(method))

void GlobalLeaderboardTableCell::ctor()
{
}

VerticalLayoutGroup* CreateHost(Transform* parent, Vector2 anchoredPos,
                                Vector2 size)
{
    VerticalLayoutGroup* group = CreateVerticalLayoutGroup(parent);
    group->rectTransform->anchoredPosition = anchoredPos;

    LayoutElement* elem = group->GetComponent<LayoutElement*>();
    elem->preferredHeight = size.y;
    elem->preferredWidth = size.x;

    ContentSizeFitter* fitter = group->GetComponent<ContentSizeFitter*>();
    fitter->verticalFit = ContentSizeFitter::FitMode::PreferredSize;
    fitter->horizontalFit = ContentSizeFitter::FitMode::PreferredSize;
    return group;
}

std::string flag_url(std::string_view COUNTRY)
{
    auto country = std::string(COUNTRY);
    for (auto& c : country)
    {
        c = tolower(c);
    }
    auto url = fmt::format("https://cdn.scoresaber.com/flags/{:s}.png", country);
    return url;
}

void GlobalLeaderboardTableCell::Refresh(ScoreSaber::Data::Player& player, LeaderboardType leaderboardType)
{
    stopProfileRoutine();
    stopFlagRoutine();

    profile->sprite = Base64ToSprite(oculus_base64);

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

    this->name->text = player.name;

    if (leaderboardType == LeaderboardType::Global || leaderboardType == LeaderboardType::AroundYou)
    {
        this->rank->text = fmt::format("#{:d}", player.rank);
    }
    else
    {
        this->rank->text = fmt::format("#{:d} (#{:d})", player.countryRank, player.rank);
    }

    this->pp->text = fmt::format("<color=#6872e5>{:.0f}pp</color>", player.pp);
    flag->sprite = Base64ToSprite(country_base64);

    flagRoutine = BeginCoroutine(WebUtils::WaitForImageDownload(flag_url(player.country), flag));
    this->country->text = player.country;

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
        result = fmt::format("<color=green>+{:d}</color>", weeklyChange);
    }
    else if (weeklyChange < 0)
    {
        result = fmt::format("<color=red>{:d}</color>", weeklyChange);
    }
    else
    {
        result = fmt::format("{:d}", weeklyChange);
    }
    weekly->text = result;

    playerId = player.id;
}

GlobalLeaderboardTableCell* GlobalLeaderboardTableCell::CreateCell()
{
    auto cellGO = UnityEngine::GameObject::New_ctor();
    auto playerCell = cellGO->AddComponent<GlobalLeaderboardTableCell*>();
    cellGO->name = "GlobalLeaderboardTableCell";

    cellGO->AddComponent<HMUI::Touchable*>();
    playerCell->interactable = false;

    auto verticalLayoutGroup = CreateVerticalLayoutGroup(
        playerCell->transform);

    auto layout = verticalLayoutGroup->gameObject->GetComponent<UnityEngine::UI::LayoutElement*>();
    layout->preferredHeight = 11.0f;
    layout->preferredWidth = 200.0f;

    Transform* t = playerCell->transform;

    playerCell->profile = CreateClickableImage(
        CreateHost(t, {-45.0f, 0.0f}, {10.0f, 10.0f})->transform,
        Base64ToSprite(oculus_base64), std::bind(&GlobalLeaderboardTableCell::OpenPlayerProfileModal, playerCell),
        {0.0f, 0.0f}, {10.0f, 10.0f});

    playerCell->name = CreateClickableText(
        CreateHost(t, {-11.0f, 2.8f}, {55.0f, 8.0f})->transform,
        u"Username", {0.0f, 0.0f}, {0.0f, 0.0f}, std::bind(&GlobalLeaderboardTableCell::OpenPlayerProfileModal, playerCell));

    playerCell->name->overflowMode = TextOverflowModes::Ellipsis;
    playerCell->name->alignment = TextAlignmentOptions::Left;
    playerCell->name->fontSize = 5.0f;

    playerCell->rank = CreateText(
        CreateHost(t, {-18.0f, -2.0f}, {40.0f, 8.0f})->transform,
        "#---", false,
        {0.0f, 0.0f});

    playerCell->rank->alignment = TextAlignmentOptions::Left;

    playerCell->pp = CreateText(
        CreateHost(t, {27.0f, 0.0f}, {20.0f, 11.0f})->transform,
        "---<color=#6872e5>pp</color>", false, {0.0f, 0.0f});

    playerCell->pp->fontSize = 5.0f;
    playerCell->pp->overflowMode = TextOverflowModes::Ellipsis;

    playerCell->flag = CreateImage(
        CreateHost(t, {19.42f, -1.65f}, {4.0f, 3.0f})->transform,
        Base64ToSprite(country_base64), {0.0f, 0.0f},
        {4.0, 3.0f});

    playerCell->flag->preserveAspect = true;

    playerCell->country = CreateText(
        CreateHost(t, {31.0f, -2.0f}, {17.0f, 0.0f})->transform, "N/A", false, {0.0f, 0.0f});
    playerCell->country->alignment = TextAlignmentOptions::Left;
    playerCell->country->fontSize = 3.5f;

    playerCell->weekly = CreateText(
        CreateHost(t, {41.0f, -1.0f}, {15.0f, 0.0f})->transform, "0", false,
        {0.0f, 0.0f});

    playerCell->weekly->alignment = TextAlignmentOptions::Right;
    playerCell->weekly->fontSize = 5.0f;

    return playerCell;
}

void GlobalLeaderboardTableCell::stopProfileRoutine()
{
    if (profileRoutine)
    {
        StopCoroutine(profileRoutine);
    }
    profileRoutine = nullptr;
}

void GlobalLeaderboardTableCell::stopFlagRoutine()
{
    if (flagRoutine)
    {
        StopCoroutine(flagRoutine);
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
