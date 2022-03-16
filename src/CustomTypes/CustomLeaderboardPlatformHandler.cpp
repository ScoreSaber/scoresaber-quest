// #include "Utils/StringUtils.hpp"

// #include "CustomTypes/CustomLeaderboardPlatformHandler.hpp"
// #include "Utils/BeatmapUtils.hpp"

// #include <chrono>
// #include <codecvt>

// #include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
// #include "GlobalNamespace/BeatmapData.hpp"
// #include "GlobalNamespace/DifficultyBeatmapSet.hpp"
// #include "GlobalNamespace/GameplayModifierParamsSO.hpp"
// #include "GlobalNamespace/GameplayModifiers.hpp"
// #include "GlobalNamespace/GameplayModifiersModelSO.hpp"
// #include "GlobalNamespace/IBeatmapLevel.hpp"
// #include "GlobalNamespace/IDifficultyBeatmap.hpp"
// #include "GlobalNamespace/IGameEnergyCounter.hpp"
// #include "GlobalNamespace/IPlatformUserModel.hpp"
// #include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
// #include "GlobalNamespace/LeaderboardTableView_ScoreData.hpp"
// #include "GlobalNamespace/LoadingControl.hpp"
// #include "GlobalNamespace/MainMenuViewController.hpp"
// #include "GlobalNamespace/OculusPlatformLeaderboardsHandler.hpp"
// #include "GlobalNamespace/OculusPlatformLeaderboardsHandler_--c__DisplayClass5_0.hpp"
// #include "GlobalNamespace/PlatformLeaderboardViewController.hpp"
// #include "GlobalNamespace/PlatformLeaderboardsHandler.hpp"
// #include "GlobalNamespace/PlatformLeaderboardsModel.hpp"
// #include "GlobalNamespace/PlatformLeaderboardsModel_GetScoresCompletionHandler.hpp"
// #include "GlobalNamespace/PlatformLeaderboardsModel_GetScoresResult.hpp"
// #include "GlobalNamespace/PlatformLeaderboardsModel_LeaderboardScore.hpp"
// #include "GlobalNamespace/PlatformLeaderboardsModel_ScoresScope.hpp"
// #include "GlobalNamespace/QuestAppInit.hpp"
// #include "GlobalNamespace/RelativeScoreAndImmediateRankCounter.hpp"
// #include "GlobalNamespace/ResultsViewController.hpp"
// #include "GlobalNamespace/ScoreController.hpp"
// #include "GlobalNamespace/SharedCoroutineStarter.hpp"
// #include "GlobalNamespace/StandardLevelDetailView.hpp"

// #include "System/Collections/Generic/List_1.hpp"

// #include "UnityEngine/JsonUtility.hpp"
// #include "UnityEngine/Networking/DownloadHandler.hpp"
// #include "UnityEngine/Networking/UnityWebRequest.hpp"
// #include "UnityEngine/WaitForSeconds.hpp"

// #include "beatsaber-hook/shared/config/config-utils.hpp"
// #include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"
// #include "beatsaber-hook/shared/utils/hooking.hpp"

// #include "questui/shared/BeatSaberUI.hpp"
// #include "questui/shared/QuestUI.hpp"

// #include "logging.hpp"

// #include "Data/LeaderboardInfo.hpp"
// #include "Data/ScoreCollection.hpp"

// DEFINE_TYPE(ScoreSaber::CustomTypes, CustomLeaderboardPlatformHandler);

// using namespace GlobalNamespace;
// using namespace StringUtils;
// using namespace BeatmapUtils;
// using namespace ScoreSaber::CustomTypes;
// using namespace ScoreSaber;

// custom_types::Helpers::Coroutine GetScoresInternal(
//     ScoreSaber::CustomTypes::CustomLeaderboardPlatformHandler* self,
//     GlobalNamespace::IDifficultyBeatmap* beatmap,
//     PlatformLeaderboardsModel::ScoresScope scope,
//     PlatformLeaderboardsModel::GetScoresCompletionHandler* completionHandler)
// {
//     self->leaderboardScoreInfoButtonHandler->set_buttonCount(0);

//     Il2CppString* csHash = reinterpret_cast<IPreviewBeatmapLevel*>(beatmap->get_level())->get_levelID();
//     csHash = csHash->Replace(StrToIl2cppStr("custom_level_"), Il2CppString::_get_Empty());
//     std::string hash = Il2cppStrToStr(csHash);

//     auto set = beatmap->get_parentDifficultyBeatmapSet();
//     auto characteristic = set->get_beatmapCharacteristic();

//     auto gameMode = ::to_utf8(csstrtostr(characteristic->dyn__serializedName()));

//     if (gameMode == "Standard")
//     {
//         gameMode = "SoloStandard";
//     }

//     // TODO: fix data fetching to use proper 10 sized pages, instead of 12 sized pages, possibly just fixed by umby using a different endpoint
//     std::string url = string_format("https://scoresaber.com/api/leaderboard/by-hash/%s/scores?difficulty=%d&page=%d&gameMode=%s", hash.c_str(), getDiff(beatmap), self->page, gameMode.c_str());
//     UnityEngine::Networking::UnityWebRequest* webRequest = UnityEngine::Networking::UnityWebRequest::Get(StrToIl2cppStr(url));
//     INFO("Getting url %s", url.c_str());
//     co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(webRequest->SendWebRequest()));

//     auto scores = System::Collections::Generic::List_1<PlatformLeaderboardsModel::LeaderboardScore*>::New_ctor();
//     auto modifiers = System::Collections::Generic::List_1<GameplayModifierParamsSO*>::New_ctor();

//     if (!webRequest->get_isNetworkError())
//     {
//         rapidjson::GenericDocument<rapidjson::UTF16<char16_t>> doc;
//         std::u16string s = std::u16string(csstrtostr(webRequest->get_downloadHandler()->get_text()));
//         // INFO("Received data: %s", s.c_str());
//         doc.Parse(s.c_str());
//         if (doc.GetParseError() == 0)
//         {
//             auto errorItr = doc.FindMember(u"errorMessage");
//             if (errorItr == doc.MemberEnd())
//             {
//                 // why calculate it every time if the beatmap pointer doesn't change?
//                 int maxScore = BeatmapUtils::getMaxScore(beatmap);

//                 Data::ScoreCollection scoreCollection(doc.GetObject());
//                 int length = scoreCollection.size();
//                 for (auto& score : scoreCollection)
//                 {
//                     auto& leaderboardPlayerInfo = score.leaderboardPlayerInfo;
//                     std::u16string coloredName = Colorize(leaderboardPlayerInfo.name, GetRoleColor(leaderboardPlayerInfo.role));
//                     std::u16string formattedScore = FormatScore(((double)score.modifiedScore / (double)maxScore) * 100.0);
//                     std::u16string formattedPP = FormatPP(score);
//                     std::u16string result = Resize(coloredName + formattedScore + formattedPP, 80);
//                     scores->Add(PlatformLeaderboardsModel::LeaderboardScore::New_ctor(score.modifiedScore, score.rank,
//                                                                                       StrToIl2cppStr(result),
//                                                                                       StrToIl2cppStr("0"), modifiers));
//                     self->mapRanked = score.pp > 0.0f;
//                 }

//                 self->leaderboardScoreInfoButtonHandler->set_scoreCollection(scoreCollection);
//                 if (scores->size == 0)
//                 {
//                     scores->Add(PlatformLeaderboardsModel::LeaderboardScore::New_ctor(0, 0, StrToIl2cppStr("No scores on this leaderboard!"),
//                                                                                       StrToIl2cppStr("0"), modifiers));
//                 }
//                 completionHandler->Invoke(PlatformLeaderboardsModel::GetScoresResult::Ok, scores->ToArray(), -1);
//             }
//             else
//             {
//                 scores->Add(PlatformLeaderboardsModel::LeaderboardScore::New_ctor(0, 0, StrToIl2cppStr(errorItr->value.GetString()),
//                                                                                   StrToIl2cppStr("0"), modifiers));
//                 completionHandler->Invoke(PlatformLeaderboardsModel::GetScoresResult::Ok, scores->ToArray(), -1);
//             }
//         }
//         else
//         {
//             scores->Add(PlatformLeaderboardsModel::LeaderboardScore::New_ctor(0, 0, StrToIl2cppStr("Received invalid response from server"),
//                                                                               StrToIl2cppStr("0"), modifiers));
//             completionHandler->Invoke(PlatformLeaderboardsModel::GetScoresResult::Ok, scores->ToArray(),
//                                       -1);
//         }
//     }
//     else
//     {
//         completionHandler->Invoke(PlatformLeaderboardsModel::GetScoresResult::Failed, scores->ToArray(),
//                                   -1);
//     }
//     webRequest->Dispose();
//     co_return;
// }

// custom_types::Helpers::Coroutine GetLeaderboardInfoInternal(ScoreSaber::CustomTypes::CustomLeaderboardPlatformHandler* self, GlobalNamespace::IDifficultyBeatmap* beatmap)
// {
//     Il2CppString* csHash = reinterpret_cast<IPreviewBeatmapLevel*>(beatmap->get_level())->get_levelID();
//     csHash = csHash->Replace(StrToIl2cppStr("custom_level_"), Il2CppString::_get_Empty());

//     std::string hash = Il2cppStrToStr(csHash);

//     auto set = beatmap->get_parentDifficultyBeatmapSet();
//     auto characteristic = set->get_beatmapCharacteristic();
//     auto gameMode = ::to_utf8(csstrtostr(characteristic->dyn__serializedName()));

//     if (gameMode == "Standard")
//     {
//         gameMode = "SoloStandard";
//     }

//     std::string url = string_format("https://scoresaber.com/api/leaderboard/by-hash/%s/info?difficulty=%d&page=%d&gameMode=%s", hash.c_str(),
//                                     getDiff(beatmap), self->page, gameMode.c_str());

//     UnityEngine::Networking::UnityWebRequest* webRequest = UnityEngine::Networking::UnityWebRequest::Get(StrToIl2cppStr(url));
//     co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(webRequest->SendWebRequest()));

//     if (!webRequest->get_isNetworkError())
//     {
//         rapidjson::Document doc;
//         std::string s = ::to_utf8(csstrtostr(webRequest->get_downloadHandler()->get_text()));
//         doc.Parse(s.c_str());

//         auto errorItr = doc.FindMember("errorMessage");

//         if (errorItr == doc.MemberEnd())
//         {
//             const Data::LeaderboardInfo leaderboardInfo(doc.GetObject());
//             self->scoreSaberBanner->set_status(string_format("%s (modifiers disabled)", leaderboardInfo.ranked ? "Ranked" : "Unranked"), leaderboardInfo.id);
//         }
//         else
//         {
//             self->scoreSaberBanner->Prompt("Invalid query made", false, 3.0f, nullptr);
//             self->scoreSaberBanner->set_bottomText("");
//         }
//     }
//     else
//     {
//     }
//     webRequest->Dispose();
//     co_return;
// }

// void ScoreSaber::CustomTypes::CustomLeaderboardPlatformHandler::changePage(bool inc)
// {
//     if (inc)
//     {
//         page++;
//     }
//     else
//     {
//         if (page != 1)
//         {
//             page--;
//         }
//     }
// }

// HMAsyncRequest* ScoreSaber::CustomTypes::CustomLeaderboardPlatformHandler::GetScores(IDifficultyBeatmap* beatmap, int count, int fromRank,
//                                                                                      PlatformLeaderboardsModel::ScoresScope scope, ::Il2CppString* referencePlayerId,
//                                                                                      PlatformLeaderboardsModel::GetScoresCompletionHandler* completionHandler)
// {
//     GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(reinterpret_cast<custom_types::Helpers::enumeratorT*>(
//         custom_types::Helpers::CoroutineHelper::New(
//             GetScoresInternal(this, beatmap, scope, completionHandler))));

//     GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(
//         reinterpret_cast<custom_types::Helpers::enumeratorT*>(
//             custom_types::Helpers::CoroutineHelper::New(
//                 GetLeaderboardInfoInternal(this, beatmap))));
//     return nullptr;
// }

// HMAsyncRequest* ScoreSaber::CustomTypes::CustomLeaderboardPlatformHandler::UploadScore(LeaderboardScoreUploader::ScoreData* scoreData,
//                                                                                        PlatformLeaderboardsModel::UploadScoreCompletionHandler* completionHandler)
// {
//     return nullptr;
// }