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
using namespace UnityEngine;

std::optional<Vector2> normalAnchor;

MAKE_AUTO_HOOK_MATCH(LeaderboardTableView_CellForIdx,
                     &::GlobalNamespace::LeaderboardTableView::CellForIdx,
                     HMUI::TableCell*, GlobalNamespace::LeaderboardTableView* self,
                     HMUI::TableView* tableView, int row)
{
    HMUI::TableCell* tableCell =
        LeaderboardTableView_CellForIdx(self, tableView, row);
    LeaderboardTableCell* cell =
        reinterpret_cast<LeaderboardTableCell*>(tableCell);

    if (!normalAnchor) {
        normalAnchor = cell->playerNameText->get_rectTransform()->get_anchoredPosition();
    }

    cell->playerNameText->set_richText(true);
    cell->playerNameText->get_rectTransform()->set_anchoredPosition(Vector2(normalAnchor->x + 2.5f, 0.0f));
    cell->playerNameText->Rebuild(UnityEngine::UI::CanvasUpdate::PreRender);

    return cell;
}