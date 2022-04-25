#include "CustomTypes/Components/GlobalLeaderboardTableData.hpp"
#include "CustomTypes/Components/GlobalLeaderboardTableCell.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "HMUI/ScrollView.hpp"
#include "HMUI/Touchable.hpp"
#include "System/Action_1.hpp"
#include "UnityEngine/Networking/DownloadHandler.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/WebUtils.hpp"
#include "logging.hpp"
#include "main.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "questui/shared/QuestUI.hpp"
#include "static.hpp"

#include "Data/PlayerCollection.hpp"
#include "UI/ViewControllers/GlobalViewController.hpp"

DEFINE_TYPE(ScoreSaber::CustomTypes::Components, GlobalLeaderboardTableData);

using namespace ScoreSaber::CustomTypes::Components;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace TMPro;
using namespace ScoreSaber;

#define BeginCoroutine(method)                                               \
    GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine( \
        reinterpret_cast<System::Collections::IEnumerator*>(                 \
            custom_types::Helpers::CoroutineHelper::New(method)));

Data::PlayerCollection playerCollection;

custom_types::Helpers::Coroutine GetDocument(ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData* self)
{
    std::string url = self->get_leaderboardURL();
    INFO("Getting player data from url %s", url.c_str());
    UnityEngine::Networking::UnityWebRequest* webRequest = UnityEngine::Networking::UnityWebRequest::Get(StringUtils::StrToIl2cppStr(url));
    webRequest->SetRequestHeader(il2cpp_utils::newcsstr("Cookie"), il2cpp_utils::newcsstr(WebUtils::cookie));
    co_yield reinterpret_cast<System::Collections::IEnumerator*>(
        CRASH_UNLESS(webRequest->SendWebRequest()));
    if (!webRequest->get_isNetworkError())
    {
        // Some of the players have utf16 characters in their names, so parse this as a utf16 document

        auto s = std::u16string(csstrtostr(webRequest->get_downloadHandler()->get_text()));

        // implicit constructor poggers
        playerCollection = webRequest->get_downloadHandler()->get_text();

        self->initialized = true;
    }
    co_return;
}

namespace ScoreSaber::CustomTypes::Components
{
    void GlobalLeaderboardTableData::ctor()
    {
        page = 1;
        reuseIdentifier = il2cpp_utils::newcsstr("CustomPlayerCellList");
        leaderboardType = Global;
    }

    float GlobalLeaderboardTableData::CellSize()
    {
        return 12.0f;
    }

    int GlobalLeaderboardTableData::NumberOfCells()
    {
        // if we have less than 50 players in the playerCollection for SOME reason, this will make sure that if we reach the end of the list it won't overextend
        int size = playerCollection.size();
        return size < 5 ? size : 5;
    }

    void GlobalLeaderboardTableData::set_leaderboardType(LeaderboardType type)
    {
        // if the new type is the same as the old type
        bool sameType = leaderboardType == type;
        bool wasPage1 = page == 1;
        leaderboardType = type;
        // scroll back to top always
        page = 1;
        // refresh the content only if we were not on page one, or if the new type is different from the old one
        StartRefresh();
    }

    std::string GlobalLeaderboardTableData::get_leaderboardURL()
    {
        std::string playersUrl = ScoreSaber::Static::BASE_URL + "/api/game/players";
        switch (leaderboardType)
        {
            default:
                [[fallthrough]];
            case LeaderboardType::Global:
                // Global leaderboard
                return string_format("%s?page=%d", playersUrl.c_str(), page);
                break;
            case LeaderboardType::AroundYou:
                return string_format("%s/around-player", playersUrl.c_str());
                break;
            case LeaderboardType::Friends:
                return string_format("%s/around-friends?page=%d", playersUrl.c_str(), page);
                break;
            case LeaderboardType::Country:
                return string_format("%s/around-country?page=%d", playersUrl.c_str(), page);
                break;
        }
    }

    void GlobalLeaderboardTableData::DownButtonWasPressed()
    {
        if (isLoading)
        {
            return;
        }

        page++;
        StartRefresh();
    }

    void GlobalLeaderboardTableData::UpButtonWasPressed()
    {
        if (isLoading)
        {
            return;
        }

        page--;
        StartRefresh();
    }

    void GlobalLeaderboardTableData::StartRefresh()
    {
        if (isLoading)
            return;
        GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(reinterpret_cast<System::Collections::IEnumerator*>(custom_types::Helpers::CoroutineHelper::New(Refresh())));
    }

    /// really just here because of the way it reloads twice by doing ReloadData and then RefreshCells(true, true), now it's combined
    void ReloadTableViewData(HMUI::TableView* self)
    {
        if (!self->dyn__isInitialized())
        {
            self->LazyInit();
        }
        auto visibleCells = self->dyn__visibleCells();
        int length = visibleCells->get_Count();
        for (int i = 0; i < length; i++)
        {
            auto tableCell = visibleCells->get_Item(i);
            tableCell->get_gameObject()->SetActive(false);
            self->AddCellToReusableCells(tableCell);
        }
        self->visibleCells->Clear();
        if (self->dyn__dataSource() != nullptr)
        {
            self->dyn__numberOfCells() = self->dyn__dataSource()->NumberOfCells();
            self->dyn__cellSize() = self->dyn__dataSource()->CellSize();
        }
        else
        {
            self->dyn__numberOfCells() = 0;
            self->dyn__cellSize() = 1.0f;
        }

        self->dyn__scrollView()->dyn__fixedCellSize() = self->dyn__cellSize();
        self->RefreshContentSize();
        if (!self->get_gameObject()->get_activeInHierarchy())
        {
            self->dyn__refreshCellsOnEnable() = true;
        }
        else
        {
            self->RefreshCells(true, true);
        }

        if (self->dyn_didReloadDataEvent())
        {
            self->dyn_didReloadDataEvent()->Invoke(self);
        }
    }

    custom_types::Helpers::Coroutine GlobalLeaderboardTableData::Refresh()
    {
        isLoading = true;
        playerCollection.clear();
        ReloadTableViewData(tableView);
        reinterpret_cast<ScoreSaber::UI::ViewControllers::GlobalViewController*>(globalViewController)->set_loading(true);
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(custom_types::Helpers::CoroutineHelper::New(GetDocument(this)));
        ReloadTableViewData(tableView);
        isLoading = false;
        reinterpret_cast<ScoreSaber::UI::ViewControllers::GlobalViewController*>(globalViewController)->set_loading(false);
        co_return;
    }

    HMUI::TableCell* GlobalLeaderboardTableData::CellForIdx(HMUI::TableView* tableView, int idx)
    {
        GlobalLeaderboardTableCell* playerCell = reinterpret_cast<GlobalLeaderboardTableCell*>(tableView->DequeueReusableCellForIdentifier(reuseIdentifier));

        if (!playerCell)
        {
            playerCell = GlobalLeaderboardTableCell::CreateCell();
            playerCell->playerProfileModal = playerProfileModal;
            // playerCell->get_transform()->SetParent(tableView->get_transform()->GetChild(0)->GetChild(0), false);
        }

        playerCell->set_reuseIdentifier(reuseIdentifier);
        if (initialized)
        {
            playerCell->Refresh(playerCollection[idx], leaderboardType);
        }
        return playerCell;
    }
} // namespace ScoreSaber::CustomTypes::Components