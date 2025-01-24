#include "CustomTypes/Components/GlobalLeaderboardTableData.hpp"
#include "CustomTypes/Components/GlobalLeaderboardTableCell.hpp"
#include <HMUI/ScrollView.hpp>
#include <HMUI/Touchable.hpp>
#include <System/Action_1.hpp>
#include <GlobalNamespace/ICoroutineStarter.hpp>
#include <UnityEngine/Networking/DownloadHandler.hpp>
#include <UnityEngine/Networking/UnityWebRequest.hpp>
#include <UnityEngine/RectOffset.hpp>
#include <UnityEngine/WaitForSeconds.hpp>
#include <bsml/shared/Helpers/getters.hpp>
#include "Utils/WebUtils.hpp"
#include "logging.hpp"
#include "static.hpp"

#include "Data/PlayerCollection.hpp"
#include "UI/ViewControllers/GlobalViewController.hpp"
#include "Utils/StrippedMethods.hpp"

DEFINE_TYPE(ScoreSaber::CustomTypes::Components, GlobalLeaderboardTableData);

using namespace ScoreSaber::CustomTypes::Components;
using namespace UnityEngine::UI;
using namespace UnityEngine::Networking;
using namespace TMPro;
using namespace ScoreSaber;

#define BeginCoroutine(method) \
    BSML::Helpers::GetDiContainer()->Resolve<GlobalNamespace::ICoroutineStarter*>()->StartCoroutine( \
        custom_types::Helpers::CoroutineHelper::New(method));

Data::PlayerCollection playerCollection;

custom_types::Helpers::Coroutine GetDocument(ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData* self)
{
    std::string url = self->GetLeaderboardURL();
    UnityWebRequest* webRequest = UnityWebRequest::Get(url);
    StrippedMethods::UnityEngine::Networking::UnityWebRequest::SetRequestHeader(webRequest, "Cookie", WebUtils::cookie);
    co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(webRequest->SendWebRequest()));
    if (webRequest->result != UnityWebRequest::Result::ProtocolError || webRequest->result != UnityWebRequest::Result::ConnectionError)
    {
        // Some of the players have utf16 characters in their names, so parse this as a utf16 document

        auto s = std::u16string(webRequest->downloadHandler->text);

        // implicit constructor poggers
        playerCollection = webRequest->downloadHandler->text;

        self->initialized = true;
    }
    co_return;
}

namespace ScoreSaber::CustomTypes::Components
{
    void GlobalLeaderboardTableData::ctor()
    {
        page = 1;
        reuseIdentifier = "CustomPlayerCellList";
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

    void GlobalLeaderboardTableData::SetLeaderboardType(LeaderboardType type)
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

    std::string GlobalLeaderboardTableData::GetLeaderboardURL()
    {
        std::string playersUrl = ScoreSaber::Static::BASE_URL + "/api/game/players";
        switch (leaderboardType)
        {
            default:
                [[fallthrough]];
            case LeaderboardType::Global:
                // Global leaderboard
                return fmt::format("{:s}?page={:d}", playersUrl.c_str(), page);
                break;
            case LeaderboardType::AroundYou:
                return fmt::format("{:s}/around-player", playersUrl.c_str());
                break;
            case LeaderboardType::Friends:
                return fmt::format("{:s}/around-friends?page={:d}", playersUrl.c_str(), page);
                break;
            case LeaderboardType::Country:
                return fmt::format("{:s}/around-country?page={:d}", playersUrl.c_str(), page);
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
        {
            return;
        }
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(Refresh()));
    }

    /// really just here because of the way it reloads twice by doing ReloadData and then RefreshCells(true, true), now it's combined
    void ReloadTableViewData(HMUI::TableView* self)
    {
        if (!self->_isInitialized)
        {
            self->LazyInit();
        }
        auto visibleCells = self->_visibleCells;
        int length = visibleCells->Count;
        for (int i = 0; i < length; i++)
        {
            auto tableCell = visibleCells->Item[i];
            tableCell->gameObject->SetActive(false);
            self->AddCellToReusableCells(tableCell);
        }
        self->_visibleCells->Clear();
        if (self->dataSource != nullptr)
        {
            self->_numberOfCells = self->dataSource->NumberOfCells();
            self->_cellSize = self->dataSource->CellSize();
        }
        else
        {
            self->_numberOfCells = 0;
            self->_cellSize = 1.0f;
        }

        self->scrollView->_fixedCellSize = self->cellSize;
        self->RefreshContentSize();
        if (!self->gameObject->activeInHierarchy)
        {
            self->_refreshCellsOnEnable = true;
        }
        else
        {
            self->RefreshCells(true, true);
        }

        if (self->didReloadDataEvent)
        {
            self->didReloadDataEvent->Invoke(self);
        }
    }

    custom_types::Helpers::Coroutine GlobalLeaderboardTableData::Refresh()
    {
        isLoading = true;
        playerCollection.clear();
        ReloadTableViewData(tableView);
        reinterpret_cast<ScoreSaber::UI::ViewControllers::GlobalViewController*>(globalViewController)->set_loading(true);
        co_yield custom_types::Helpers::CoroutineHelper::New(GetDocument(this));
        ReloadTableViewData(tableView);
        isLoading = false;
        reinterpret_cast<ScoreSaber::UI::ViewControllers::GlobalViewController*>(globalViewController)->set_loading(false);
        co_return;
    }

    HMUI::TableCell* GlobalLeaderboardTableData::CellForIdx(HMUI::TableView* tableView, int idx)
    {
        UnityW<GlobalLeaderboardTableCell> playerCell = tableView->DequeueReusableCellForIdentifier(reuseIdentifier).cast<GlobalLeaderboardTableCell>();

        if (!playerCell)
        {
            playerCell = GlobalLeaderboardTableCell::CreateCell();
            playerCell->playerProfileModal = playerProfileModal;
            // playerCell->transform->SetParent(tableView->transform->GetChild(0)->GetChild(0), false);
        }

        playerCell->reuseIdentifier = reuseIdentifier;
        if (initialized)
        {
            playerCell->Refresh(playerCollection[idx], leaderboardType);
        }
        return playerCell;
    }
} // namespace ScoreSaber::CustomTypes::Components