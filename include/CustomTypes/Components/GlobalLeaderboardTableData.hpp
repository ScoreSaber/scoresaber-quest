#pragma once
#include <beatsaber-hook/shared/config/rapidjson-utils.hpp>

#include <HMUI/TableCell.hpp>
#include <HMUI/TableView.hpp>
#include <HMUI/ViewController.hpp>
#include "UI/Other/PlayerProfileModal.hpp"
#include <UnityEngine/MonoBehaviour.hpp>
#include <custom-types/shared/coroutine.hpp>
#include <custom-types/shared/macros.hpp>


DECLARE_CLASS_CODEGEN_INTERFACES(
        ScoreSaber::CustomTypes::Components,
        GlobalLeaderboardTableData,
        UnityEngine::MonoBehaviour,
        HMUI::TableView::IDataSource*) {
    DECLARE_INSTANCE_FIELD(StringW, reuseIdentifier);
    DECLARE_INSTANCE_FIELD(float, cellSize);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::TableView>, tableView);
    DECLARE_INSTANCE_FIELD(bool, clickableCells);
    DECLARE_INSTANCE_FIELD(bool, initialized);
    DECLARE_INSTANCE_FIELD(bool, isLoading);
    DECLARE_INSTANCE_FIELD(int, page);
    DECLARE_INSTANCE_FIELD(StringW, cellTemplate);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::ViewController>, globalViewController);
    DECLARE_INSTANCE_FIELD(UnityW<ScoreSaber::UI::Other::PlayerProfileModal>, playerProfileModal);

    DECLARE_CTOR(ctor);
    DECLARE_INSTANCE_METHOD(void, DownButtonWasPressed);
    DECLARE_INSTANCE_METHOD(void, UpButtonWasPressed);

    DECLARE_OVERRIDE_METHOD_MATCH(
        HMUI::TableCell*, CellForIdx,
        &HMUI::TableView::IDataSource::CellForIdx,
        HMUI::TableView* tableView, int idx);
    DECLARE_OVERRIDE_METHOD_MATCH(
        float, CellSize,
        &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(
        int, NumberOfCells,
        &HMUI::TableView::IDataSource::NumberOfCells);

public:

    enum LeaderboardType{
        Global,
        AroundYou,
        Friends,
        Country};

    LeaderboardType leaderboardType = Global;
    void set_LeaderboardType(LeaderboardType type);
    std::string get_LeaderboardURL();
    void StartRefresh();
    custom_types::Helpers::Coroutine Refresh();
};