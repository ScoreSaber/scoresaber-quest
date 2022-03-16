#pragma once

#include "CustomTypes/Components/ClickableImage.hpp"
#include "UI/Other/ScoreInfoModal.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

#include "Data/Score.hpp"
#include <vector>

DECLARE_CLASS_CODEGEN(ScoreSaber::CustomTypes::Components, LeaderboardScoreInfoButtonHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(Array<ScoreSaber::CustomTypes::Components::ClickableImage*>*, buttons);

                      DECLARE_INSTANCE_FIELD(ScoreSaber::UI::Other::ScoreInfoModal*, scoreInfoModal);
                      public
                      :

                      void Setup();
                      void set_buttonCount(int count);
                      void set_scoreCollection(std::vector<ScoreSaber::Data::Score> scores);
                      private
                      :

                      std::vector<ScoreSaber::Data::Score>
                          scores;
                      void ShowScoreInfoModal(int buttonIdx);

)