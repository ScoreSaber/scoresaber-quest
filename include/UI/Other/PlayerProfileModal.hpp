#pragma once

#include <HMUI/ImageView.hpp>
#include <HMUI/ModalView.hpp>
#include <TMPro/TextMeshProUGUI.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/Transform.hpp>
#include <UnityEngine/UI/GridLayoutGroup.hpp>
#include <custom-types/shared/coroutine.hpp>
#include <custom-types/shared/macros.hpp>
#include <string_view>

#include <UnityEngine/Coroutine.hpp>

#include "Data/Badge.hpp"
#include "Data/Player.hpp"

DECLARE_CLASS_CODEGEN(ScoreSaber::UI::Other, PlayerProfileModal, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(UnityW<HMUI::ModalView>, modal);
                      DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, pfpImage);
                      DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::GridLayoutGroup>, badgeParent);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, headerText);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, globalRanking);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, performancePoints);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, averageRankedAccuracy);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, totalScore);
                      DECLARE_INSTANCE_FIELD(UnityEngine::Coroutine*, profileRoutine);
                      DECLARE_INSTANCE_FIELD(ListW<UnityEngine::Coroutine*>, badgeRoutines);

                      public
                      :

                      std::string playerId;
                      static ScoreSaber::UI::Other::PlayerProfileModal * Create(UnityEngine::Transform * parent);
                      void Show(std::string playerId);

                      void Hide();
                      void Setup();
                      void set_player(std::u16string_view playername);
                      void set_header(std::u16string_view header);
                      void set_globalRanking(int globalRanking);
                      void set_performancePoints(float performancePoints);
                      void set_averageRankedAccuracy(float averageRankedAccuracy);
                      void set_totalScore(long totalScore);
                      void set_pfp(UnityEngine::Sprite* pfp);
                      void ClearBadges();
                      void AddBadge(ScoreSaber::Data::Badge& badge, int index);

                      custom_types::Helpers::Coroutine FetchPlayerData(std::string playerId);
                      void SetPlayerData(ScoreSaber::Data::Player& player);
                      void stopProfileRoutine();
                      void stopBadgeRoutines();

                      void OpenPlayerUrl();)