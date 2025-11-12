#pragma once

#include "UI/Other/ScoreInfoModal.hpp"
#include <UnityEngine/MonoBehaviour.hpp>
#include <custom-types/shared/macros.hpp>

#include "Data/Score.hpp"
#include <vector>

DECLARE_CLASS_CODEGEN(ScoreSaber::CustomTypes::Components, LeaderboardScoreInfoButtonHandler, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_FIELD(UnityW<ScoreSaber::UI::Other::ScoreInfoModal>, scoreInfoModal);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapLevel*, beatmapLevel);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapKey, beatmapKey);

    public:
    void Setup();
    void set_scoreCollection(std::vector<ScoreSaber::Data::Score> scores, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, int leaderboardId, int maxScore);
    void ShowScoreInfoModal(int buttonIdx);

    private:
    int leaderboardId;
    int maxScore;
    std::vector<ScoreSaber::Data::Score> scores;
};