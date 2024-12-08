#pragma once

#include "CustomTypes/Components/ClickableImage.hpp"
#include "UI/Other/ScoreInfoModal.hpp"
#include <UnityEngine/MonoBehaviour.hpp>
#include <custom-types/shared/macros.hpp>

#include "Data/Score.hpp"
#include <vector>

DECLARE_CLASS_CODEGEN(ScoreSaber::CustomTypes::Components, LeaderboardScoreInfoButtonHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(ScoreSaber::UI::Other::ScoreInfoModal*, scoreInfoModal);

                      public:
                      void Setup();
                      void set_scoreCollection(std::vector<ScoreSaber::Data::Score> scores, int leaderboardId);
                      void ShowScoreInfoModal(int buttonIdx);

                      private:
                      int leaderboardId;
                      std::vector<ScoreSaber::Data::Score> scores;

)