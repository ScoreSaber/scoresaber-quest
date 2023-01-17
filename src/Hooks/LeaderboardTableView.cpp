#include "hooks.hpp"

#include "GlobalNamespace/LeaderboardTableCell.hpp"
#include "GlobalNamespace/LeaderboardTableView.hpp"
#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/CanvasUpdate.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

using namespace GlobalNamespace;

MAKE_AUTO_HOOK_MATCH(LeaderboardTableView_CellForIdx,
                     &::GlobalNamespace::LeaderboardTableView::CellForIdx,
                     HMUI::TableCell*, GlobalNamespace::LeaderboardTableView* self,
                     HMUI::TableView* tableView, int row)
{
    HMUI::TableCell* tableCell =
        LeaderboardTableView_CellForIdx(self, tableView, row);
    LeaderboardTableCell* cell =
        reinterpret_cast<LeaderboardTableCell*>(tableCell);

    cell->playerNameText->set_richText(true);
    cell->playerNameText->Rebuild(UnityEngine::UI::CanvasUpdate::PreRender);

    return cell;
}