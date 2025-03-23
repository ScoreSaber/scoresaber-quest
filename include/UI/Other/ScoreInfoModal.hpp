#pragma once
#include "Data/Score.hpp"
#include <GlobalNamespace/BeatmapKey.hpp>
#include <GlobalNamespace/BeatmapLevel.hpp>
#include <HMUI/ModalView.hpp>
#include "UI/Other/PlayerProfileModal.hpp"
#include <UnityEngine/MonoBehaviour.hpp>
#include <custom-types/shared/macros.hpp>
#include <bsml/shared/BSML-Lite.hpp>
#include <optional>
#include <string>

DECLARE_CLASS_CODEGEN(ScoreSaber::UI::Other, ScoreInfoModal, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(UnityW<HMUI::ModalView>, modal);
                      DECLARE_INSTANCE_FIELD(UnityW<PlayerProfileModal>, playerProfileModal);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, player);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, deviceHmd);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, score);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, pp);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, combo);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, fullCombo);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, badCuts);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, missedNotes);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, modifiers);
                      DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, timeSet);
                      DECLARE_INSTANCE_FIELD(UnityW<BSML::ClickableImage>, replayImage);
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapLevel*, currentBeatmapLevel);
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapKey, currentBeatmapKey);

                      public
                      :

                      void Hide();
                      void Show(ScoreSaber::Data::Score& score, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, int leaderboardId, int maxScore);

                      static ScoreSaber::UI::Other::ScoreInfoModal * Create(UnityEngine::Transform * parent);

                      void Setup();

                      void set_player(std::u16string player);
                      void set_device_hmd(std::string_view device);
                      void set_score(long score, double percent);
                      void set_pp(double pp);
                      void set_combo(std::optional<int> combo);
                      void set_fullCombo(std::optional<bool> value);
                      void set_badCuts(std::optional<int> badCuts);
                      void set_missedNotes(std::optional<int> missedNotes);
                      void set_modifiers(std::string_view modifiers);
                      void set_timeSet(std::string_view timeSet);

                      private
                      : int leaderboardId;
                      ScoreSaber::Data::Score currentScore;
                      std::string playerId;
                      std::string replayFileName;
                      bool replayEnabled;

                      void ShowPlayerProfileModal();
                      void PlayReplay();
                      void SetReplayButtonState(bool enabled);

)