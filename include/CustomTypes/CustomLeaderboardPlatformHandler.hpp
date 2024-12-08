// #pragma once

// #include "CustomTypes/Components/LeaderboardScoreInfoButtonHandler.hpp"
// #include "UI/Other/Banner.hpp"

// #include <GlobalNamespace/HMAsyncRequest.hpp>
// #include <GlobalNamespace/IDifficultyBeatmap.hpp>
// #include <GlobalNamespace/LeaderboardScoreUploader_ScoreData.hpp>
// #include <GlobalNamespace/PlatformLeaderboardsHandler.hpp>
// #include <GlobalNamespace/PlatformLeaderboardsModel.hpp>
// #include <GlobalNamespace/PlatformLeaderboardsModel_GetScoresCompletionHandler.hpp>
// #include <GlobalNamespace/PlatformLeaderboardsModel_ScoresScope.hpp>
// #include <GlobalNamespace/PlatformLeaderboardsModel_UploadScoreCompletionHandler.hpp>
// #include <TMPro/TextMeshProUGUI.hpp>
// #include <custom-types/shared/macros.hpp>
// #include <custom-types/shared/register.hpp>
// #include <custom-types/shared/types.hpp>
// #include <custom-types/shared/util.hpp>
// #define GET_FIND_METHOD(mPtr) \
//     il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

// DECLARE_CLASS_CODEGEN(
//     ScoreSaber::CustomTypes, CustomLeaderboardPlatformHandler,
//     GlobalNamespace::PlatformLeaderboardsHandler,
//     DECLARE_OVERRIDE_METHOD(
//         GlobalNamespace::HMAsyncRequest*, GetScores,
//         GET_FIND_METHOD(
//             &GlobalNamespace::PlatformLeaderboardsHandler::GetScores),
//         GlobalNamespace::IDifficultyBeatmap* beatmap, int count, int fromRank,
//         GlobalNamespace::PlatformLeaderboardsModel::ScoresScope scope,
//         ::Il2CppString* referencePlayerId,
//         GlobalNamespace::PlatformLeaderboardsModel::GetScoresCompletionHandler*
//             completionHandler);
//     DECLARE_OVERRIDE_METHOD(
//         GlobalNamespace::HMAsyncRequest*, UploadScore,
//         GET_FIND_METHOD(
//             &GlobalNamespace::PlatformLeaderboardsHandler::UploadScore),
//         GlobalNamespace::LeaderboardScoreUploader::ScoreData* scoreData,
//         GlobalNamespace::PlatformLeaderboardsModel::
//             UploadScoreCompletionHandler* completionHandler);
//     DECLARE_INSTANCE_FIELD_DEFAULT(int, page, 1);
//     DECLARE_INSTANCE_FIELD_DEFAULT(int, lastCell, 0);
//     DECLARE_INSTANCE_FIELD_DEFAULT(bool, aroundCountry, true);
//     DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, ranked);
//     DECLARE_INSTANCE_FIELD_DEFAULT(bool, mapRanked, true);
//     DECLARE_INSTANCE_FIELD(ScoreSaber::UI::Other::Banner*, scoreSaberBanner);
//     DECLARE_INSTANCE_FIELD(ScoreSaber::CustomTypes::Components::LeaderboardScoreInfoButtonHandler*, leaderboardScoreInfoButtonHandler);
//     public
//     : void changePage(bool inc);)