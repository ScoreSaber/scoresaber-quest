#include "Utils/StringUtils.hpp"

#include "Data/InternalLeaderboard.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/GameplayModifierParamsSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_LeaderboardScore.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_ScoresScope.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "Services/LeaderboardService.hpp"
#include "Utils/BeatmapUtils.hpp"

#include "Data/Leaderboard.hpp"
#include "Utils/WebUtils.hpp"

#include "Services/PlayerService.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "Utils/obfuscation.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "logging.hpp"
#include "static.hpp"

using namespace GlobalNamespace;
using namespace StringUtils;
using namespace ScoreSaber;

namespace ScoreSaber::Services::LeaderboardService
{
    std::string GetLeaderboardUrl(IDifficultyBeatmap* difficultyBeatmap, PlatformLeaderboardsModel::ScoresScope scope, int page, bool filterAroundCountry)
    {
        if (difficultyBeatmap == nullptr)
        {
            return "";
        }

        std::string url = ScoreSaber::Static::BASE_URL + ENCRYPT_STRING_AUTO_A(encoder, "/api/game/leaderboard");

        GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel;

        try
        {
            previewBeatmapLevel = reinterpret_cast<IPreviewBeatmapLevel*>(difficultyBeatmap->get_level());
        }
        catch (const std::exception& e)
        {
            return "";
        }
        if (previewBeatmapLevel == nullptr)
        {
            return "";
        }

        auto levelId = previewBeatmapLevel->get_levelID();
        std::string levelHash = StringUtils::GetFormattedHash(levelId);

        std::string gameMode = "Solo" + difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName;
        int difficulty = BeatmapDifficultyMethods::DefaultRating(difficultyBeatmap->get_difficulty());
        if (!filterAroundCountry)
        {
            switch (scope)
            {
                case PlatformLeaderboardsModel::ScoresScope::Global: {
                    url = string_format(ENCRYPT_STRING_AUTO_A(encoder, "%s/%s/mode/%s/difficulty/%d?page=%d"), url.c_str(), levelHash.c_str(), gameMode.c_str(), difficulty, page);
                    break;
                }
                case PlatformLeaderboardsModel::ScoresScope::AroundPlayer: {
                    url = string_format(ENCRYPT_STRING_AUTO_A(encoder, "%s/around-player/%s/mode/%s/difficulty/%d?page=%d"), url.c_str(), levelHash.c_str(), gameMode.c_str(), difficulty, page);
                    break;
                }
                case PlatformLeaderboardsModel::ScoresScope::Friends: {
                    url = string_format(ENCRYPT_STRING_AUTO_A(encoder, "%s/around-friends/%s/mode/%s/difficulty/%d?page=%d"), url.c_str(), levelHash.c_str(), gameMode.c_str(), difficulty, page);
                    break;
                }
            }
        }
        else
        {
            url = string_format(ENCRYPT_STRING_AUTO_A(encoder, "%s/around-country/%s/mode/%s/difficulty/%d?page=%d"), url.c_str(), levelHash.c_str(), gameMode.c_str(), difficulty, page);
        }

        return url;
        // UMBY: hide NA scores, just implement a config first lol
        // url = url + "&hideNA=1";
    }

    void GetLeaderboardData(IDifficultyBeatmap* difficultyBeatmap, PlatformLeaderboardsModel::ScoresScope scope, int page,
                            std::function<void(Data::InternalLeaderboard)> finished,
                            bool filterAroundCountry)
    {
        std::string url = GetLeaderboardUrl(difficultyBeatmap, scope, page, filterAroundCountry);

        if (url == "")
        {
            finished(GetLeaderboardError("Failed to get beatmap data! Please refresh"));
            return;
        }

        auto [beatmapDataBasicInfo, readonlyBeatmapData] = BeatmapUtils::getBeatmapData(difficultyBeatmap);

        bool containsV3Stuff = BeatmapUtils::containsV3Stuff(readonlyBeatmapData);

        if (containsV3Stuff)
        {
            finished(GetLeaderboardError("Maps with new note types currently not supported"));
            return;
        }

        int maxScore = BeatmapUtils::getMaxScore(beatmapDataBasicInfo);

        WebUtils::GetAsync(
            url, [=](long code, std::string result) {
                Data::InternalLeaderboard data;
                switch (code)
                {
                    case 200:
                        data = ParseLeaderboardData(result, difficultyBeatmap, scope, page, filterAroundCountry, maxScore);
                        break;
                    case 404:
                        data = GetLeaderboardError("No scores on this leaderboard!");
                        break;
                    default:
                        data = GetLeaderboardError("Received invalid response from server");
                        break;
                }
                finished(data);
            });
    }

    Data::InternalLeaderboard GetLeaderboardError(std::string error)
    {
        auto scores = List<LeaderboardTableView::ScoreData*>::New_ctor();
        scores->Add(LeaderboardTableView::ScoreData::New_ctor(0, error, 0, false));
        return Data::InternalLeaderboard(scores);
    }

    Data::InternalLeaderboard ParseLeaderboardData(std::string rawData, IDifficultyBeatmap* difficultyBeatmap, PlatformLeaderboardsModel::ScoresScope scope,
                                                   int page, bool filterAroundCountry, int maxScore)
    {
        auto scores = List<LeaderboardTableView::ScoreData*>::New_ctor();
        auto modifiers = List<GameplayModifierParamsSO*>::New_ctor();
        Data::Leaderboard currentLeaderboard;
        rapidjson::Document doc;
        doc.Parse(rawData.data());
        if (doc.GetParseError() == 0)
        {
            auto errorItr = doc.FindMember("errorMessage");
            if (errorItr == doc.MemberEnd())
            {
                currentLeaderboard = Data::Leaderboard(doc.GetObject());
                int length = currentLeaderboard.scores.size();

                for (auto& score : currentLeaderboard.scores)
                {
                    auto& leaderboardPlayerInfo = score.leaderboardPlayerInfo;
                    std::u16string formattedScore = FormatScore(((double)score.modifiedScore / (double)maxScore) * 100.0);
                    std::u16string formattedPP = FormatPP(score);
                    std::u16string result = Resize(leaderboardPlayerInfo.name.value() + formattedScore + formattedPP, 80);
                    scores->Add(LeaderboardTableView::ScoreData::New_ctor(score.modifiedScore, result, score.rank, false));
                }

                if (scores->size == 0)
                {
                    return GetLeaderboardError("No scores on this leaderboard!");
                }
                return Data::InternalLeaderboard(scores, std::make_optional<Data::Leaderboard>(currentLeaderboard));
            }
            else
            {
                return GetLeaderboardError(errorItr->value.GetString());
            }
        }
        else
        {
            return GetLeaderboardError("Received invalid response from server");
        }
        return GetLeaderboardError("Received invalid response from server");
    }
} // namespace ScoreSaber::Services::LeaderboardService
