#include "hooks.hpp"

#include "CustomTypes/Components/CellClicker.hpp"
#include <GlobalNamespace/LeaderboardTableCell.hpp>
#include <GlobalNamespace/LeaderboardTableView.hpp>
#include <HMUI/TableCell.hpp>
#include <HMUI/TableView.hpp>
#include <TMPro/TextMeshPro.hpp>
#include <TMPro/TextMeshProUGUI.hpp>
#include "UI/Other/ScoreSaberLeaderboardView.hpp"
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/RectTransform.hpp>
#include <UnityEngine/UI/CanvasUpdate.hpp>

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace ScoreSaber::UI::Other;
using namespace ScoreSaber::CustomTypes::Components;

std::optional<Vector2> normalAnchor;

MAKE_AUTO_HOOK_MATCH(LeaderboardTableView_CellForIdx,
                     &::GlobalNamespace::LeaderboardTableView::CellForIdx, UnityW<HMUI::TableCell>, GlobalNamespace::LeaderboardTableView* self,
                     HMUI::TableView* tableView, int row)
{
    HMUI::TableCell* tableCell = LeaderboardTableView_CellForIdx(self, tableView, row);
    LeaderboardTableCell* cell = il2cpp_utils::cast<LeaderboardTableCell>(tableCell);

    CellClicker* cellClicker = ScoreSaberLeaderboardView::_cellClickingImages[row]->gameObject->AddComponent<CellClicker*>();
    cellClicker->onClick = std::bind(&LeaderboardScoreInfoButtonHandler::ShowScoreInfoModal, ScoreSaberLeaderboardView::leaderboardScoreInfoButtonHandler, std::placeholders::_1);
    cellClicker->index = row;
    cellClicker->seperator = cell->_separatorImage.cast<HMUI::ImageView>();

    if (!normalAnchor) {
        normalAnchor = cell->_playerNameText->rectTransform->anchoredPosition;
    }

    cell->_playerNameText->richText = true;
    cell->_playerNameText->rectTransform->anchoredPosition = Vector2(normalAnchor->x + 2.5f, 0.0f);
    cell->_playerNameText->Rebuild(UnityEngine::UI::CanvasUpdate::PreRender);
    cell->showSeparator = true;

    return cell;
}