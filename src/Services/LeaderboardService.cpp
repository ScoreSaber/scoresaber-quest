#include "Utils/StringUtils.hpp"

#include "Data/InternalLeaderboard.hpp"
#include "Data/Private/Settings.hpp"

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
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "logging.hpp"
#include "static.hpp"

using namespace GlobalNamespace;
using namespace StringUtils;
using namespace ScoreSaber;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::Services::LeaderboardService
{
    std::string GetLeaderboardUrl(IDifficultyBeatmap* difficultyBeatmap, PlatformLeaderboardsModel::ScoresScope scope, int page, bool filterAroundCountry)
    {
        if (difficultyBeatmap == nullptr)
        {
            return "";
        }

        std::string url = ScoreSaber::Static::BASE_URL + "/api/game/leaderboard";

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
        
        bool hasPage = true;

        if (!filterAroundCountry)
        {
            switch (scope)
            {
                case PlatformLeaderboardsModel::ScoresScope::Global: {
                    url = string_format("%s/%s/mode/%s/difficulty/%d?page=%d", url.c_str(), levelHash.c_str(), gameMode.c_str(), difficulty, page);
                    break;
                }
                case PlatformLeaderboardsModel::ScoresScope::AroundPlayer: {
                    url = string_format("%s/around-player/%s/mode/%s/difficulty/%d?page=%d", url.c_str(), levelHash.c_str(), gameMode.c_str(), difficulty, page);
                    break;
                }
                case PlatformLeaderboardsModel::ScoresScope::Friends: {
                    url = string_format("%s/around-friends/%s/mode/%s/difficulty/%d?page=%d", url.c_str(), levelHash.c_str(), gameMode.c_str(), difficulty, page);
                    break;
                }
            }
        }
        else
        {
            std::string mode = Settings::locationFilterMode;
            for (auto& c : mode)
            {
                c = tolower(c);
            }
            if(mode == "region") {
                url = string_format("%s/around-region/%s/mode/%s/difficulty/%d?page=%d", url.c_str(), levelHash.c_str(), gameMode.c_str(), difficulty, page);
            } else {
                if(mode != "country") {
                    ERROR("Invalid location filter mode, falling back to country");
                }
                url = string_format("%s/around-country/%s/mode/%s/difficulty/%d?page=%d", url.c_str(), levelHash.c_str(), gameMode.c_str(), difficulty, page);
            }
        }

        if (Settings::hideNAScoresFromLeaderboard) {
            if (hasPage)
                url += "&hideNA=1";
            else
                url += "?hideNA=1";
        }

        return url;
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

        int maxScore = BeatmapUtils::getMaxScore(readonlyBeatmapData);

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
        return Data::InternalLeaderboard(scores, {});
    }

    Data::InternalLeaderboard ParseLeaderboardData(std::string rawData, IDifficultyBeatmap* difficultyBeatmap, PlatformLeaderboardsModel::ScoresScope scope,
                                                   int page, bool filterAroundCountry, int maxScore)
    {
        auto scores = List<LeaderboardTableView::ScoreData*>::New_ctor();
        auto modifiers = List<GameplayModifierParamsSO*>::New_ctor();
        std::vector<std::string> profilePictures;
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
                    std::u16string result = Resize(leaderboardPlayerInfo.name.value() + formattedScore + formattedPP, 75);
                    scores->Add(LeaderboardTableView::ScoreData::New_ctor(score.modifiedScore, result, score.rank, false));
                    profilePictures.push_back(leaderboardPlayerInfo.profilePicture);
                }

                if (scores->size == 0)
                {
                    return GetLeaderboardError("No scores on this leaderboard!");
                }
                return Data::InternalLeaderboard(scores, profilePictures, std::make_optional<Data::Leaderboard>(currentLeaderboard));
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
