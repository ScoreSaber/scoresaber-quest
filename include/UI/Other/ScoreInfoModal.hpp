#pragma once
#include "Data/Score.hpp"

#include "HMUI/ModalView.hpp"
#include "UI/Other/PlayerProfileModal.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"
#include <string>

DECLARE_CLASS_CODEGEN(ScoreSaber::UI::Other, ScoreInfoModal, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(HMUI::ModalView*, modal);
                      DECLARE_INSTANCE_FIELD(PlayerProfileModal*, playerProfileModal);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, player);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, device);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, score);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, pp);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, combo);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, fullCombo);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, badCuts);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, missedNotes);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, modifiers);
                      DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, timeSet);

                      public
                      :

                      void Hide();
                      void Show(ScoreSaber::Data::Score& score);

                      static ScoreSaber::UI::Other::ScoreInfoModal * Create(UnityEngine::Transform * parent);
                      void Setup();

                      void set_player(std::u16string player);
                      void set_device(std::string_view device);
                      void set_score(long score, double percent);
                      void set_pp(double pp);
                      void set_combo(int combo);
                      void set_fullCombo(bool value);
                      void set_badCuts(int badCuts);
                      void set_missedNotes(int missedNotes);
                      void set_modifiers(std::string_view modifiers);
                      void set_timeSet(std::string_view timeSet);

                      private
                      :

                      std::string playerId;
                      void ShowPlayerProfileModal();

)