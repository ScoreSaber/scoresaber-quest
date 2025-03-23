#include "CustomTypes/Components/LeaderboardScoreInfoButtonHandler.hpp"
#include "Utils/UIUtils.hpp"

#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Transform.hpp>

#include "Sprites.hpp"
#include "logging.hpp"

DEFINE_TYPE(ScoreSaber::CustomTypes::Components, LeaderboardScoreInfoButtonHandler);

using namespace UnityEngine;
using namespace UnityEngine::UI;

namespace ScoreSaber::CustomTypes::Components
{
    void LeaderboardScoreInfoButtonHandler::Setup()
    {
        scoreInfoModal = ScoreSaber::UI::Other::ScoreInfoModal::Create(transform);
    }

    void LeaderboardScoreInfoButtonHandler::set_scoreCollection(std::vector<ScoreSaber::Data::Score> _scores, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, int leaderboardId, int maxScore)
    {
        this->scores = _scores;
        this->leaderboardId = leaderboardId;
        this->beatmapLevel = beatmapLevel;
        this->beatmapKey = beatmapKey;
        this->maxScore = maxScore;
    }

    void LeaderboardScoreInfoButtonHandler::ShowScoreInfoModal(int buttonIdx)
    {
        if (scoreInfoModal && buttonIdx < scores.size())
        {
            scoreInfoModal->Show(scores.at(buttonIdx), beatmapLevel, beatmapKey, leaderboardId, maxScore);
        }
    }
}