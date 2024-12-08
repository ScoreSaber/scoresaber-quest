#pragma once
#include "CustomTypes/Components/GlobalLeaderboardTableData.hpp"

#include <HMUI/ModalView.hpp>
#include <HMUI/ViewController.hpp>
#include "UI/Other/PlayerProfileModal.hpp"
#include <UnityEngine/GameObject.hpp>
#include <custom-types/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(
    ScoreSaber::UI::ViewControllers, GlobalViewController,
    HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate,
                            &HMUI::ViewController::DidActivate,
                            bool firstActivation, bool addedToHierarchy,
                            bool screenSystemEnabling);
    DECLARE_INSTANCE_FIELD(ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData*, leaderboardList);
    DECLARE_INSTANCE_FIELD(HMUI::ModalView*, moreInfoModal);
    DECLARE_INSTANCE_FIELD(ScoreSaber::UI::Other::PlayerProfileModal*, playerProfileModal);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, loadingIndicator);
    void set_loading(bool value);
    private
    :

    void OpenMoreInfoModal();
    void UpButtonWasPressed();
    void DownButtonWasPressed();
    void FilterWasClicked(ScoreSaber::CustomTypes::Components::GlobalLeaderboardTableData::LeaderboardType type);

);