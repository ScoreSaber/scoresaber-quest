#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableView_IDataSource.hpp"
#include "HMUI/ViewController.hpp"
#include "UI/Other/PlayerProfileModal.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#define GET_FIND_METHOD(mPtr) \
    il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

static std::vector<Il2CppClass*> GetInterfaces()
{
    return {classof(HMUI::TableView::IDataSource*)};
}

___DECLARE_TYPE_WRAPPER_INHERITANCE(
    ScoreSaber::CustomTypes::Components, GlobalLeaderboardTableData,
    Il2CppTypeEnum::IL2CPP_TYPE_CLASS, UnityEngine::MonoBehaviour, "ScoreSaber",
    GetInterfaces(), 0, nullptr,
    DECLARE_INSTANCE_FIELD(Il2CppString*, reuseIdentifier);
    DECLARE_INSTANCE_FIELD(float, cellSize);
    DECLARE_INSTANCE_FIELD(HMUI::TableView*, tableView);
    DECLARE_INSTANCE_FIELD(bool, clickableCells);
    DECLARE_INSTANCE_FIELD(bool, initialized);
    DECLARE_INSTANCE_FIELD(bool, isLoading);
    DECLARE_INSTANCE_FIELD(int, page);
    DECLARE_INSTANCE_FIELD(int, page2);
    DECLARE_INSTANCE_FIELD(Il2CppString*, cellTemplate);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, globalViewController);
    DECLARE_INSTANCE_FIELD(ScoreSaber::UI::Other::PlayerProfileModal*, playerProfileModal);

    DECLARE_CTOR(ctor);
    DECLARE_INSTANCE_METHOD(void, DownButtonWasPressed);
    DECLARE_INSTANCE_METHOD(void, UpButtonWasPressed);

    DECLARE_OVERRIDE_METHOD(
        HMUI::TableCell*, CellForIdx,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<
            &HMUI::TableView::IDataSource::CellForIdx>::get(),
        HMUI::TableView* tableView, int idx);
    DECLARE_OVERRIDE_METHOD(
        float, CellSize,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<
            &HMUI::TableView::IDataSource::CellSize>::get());
    DECLARE_OVERRIDE_METHOD(
        int, NumberOfCells,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<
            &HMUI::TableView::IDataSource::NumberOfCells>::get());

    public
    :

    enum LeaderboardType{
        Global,
        AroundYou,
        Friends,
        Country};

    std::string myCountry;
    LeaderboardType leaderboardType = Global;
    void set_leaderboardType(LeaderboardType type);
    std::string get_leaderboardURL();
    void StartRefresh(bool redownload = false);
    custom_types::Helpers::Coroutine Refresh(bool redownload = false);)