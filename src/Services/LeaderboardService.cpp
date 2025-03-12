#include "Utils/SafePtr.hpp"
#include "Utils/StringUtils.hpp"

#include "Data/InternalLeaderboard.hpp"
#include "Data/Private/Settings.hpp"

#include <GlobalNamespace/BeatmapCharacteristicSO.hpp>
#include <GlobalNamespace/BeatmapDifficulty.hpp>
#include <GlobalNamespace/BeatmapDifficultyMethods.hpp>
#include <GlobalNamespace/GameplayModifierParamsSO.hpp>
#include <GlobalNamespace/PlatformLeaderboardsModel.hpp>
#include "Services/LeaderboardService.hpp"

#include "Data/Leaderboard.hpp"
#include "Utils/WebUtils.hpp"

#include <UnityEngine/Networking/UnityWebRequest.hpp>
#include <beatsaber-hook/shared/config/rapidjson-utils.hpp>
#include "logging.hpp"
#include "static.hpp"

using namespace GlobalNamespace;
using namespace StringUtils;
using namespace ScoreSaber;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::Services::LeaderboardService
{
    std::string GetLeaderboardUrl(BeatmapKey beatmapKey, PlatformLeaderboardsModel::ScoresScope scope, int page, bool filterAroundCountry)
    {
        std::string url = ScoreSaber::Static::BASE_URL + "/api/game/leaderboard";
        std::string levelHash = StringUtils::GetFormattedHash(beatmapKey.levelId);
        std::string gameMode = "Solo" + beatmapKey.beatmapCharacteristic->serializedName;
        int difficulty = BeatmapDifficultyMethods::DefaultRating(beatmapKey.difficulty);
        
        bool hasPage = true;

        if (!filterAroundCountry)
        {
            switch (scope)
            {
                case PlatformLeaderboardsModel::ScoresScope::Global: {
                    url = fmt::format("{:s}/{:s}/mode/{:s}/difficulty/{:d}?page={:d}", url, levelHash, gameMode, difficulty, page);
                    break;
                }
                case PlatformLeaderboardsModel::ScoresScope::AroundPlayer: {
                    url = fmt::format("{:s}/around-player/{:s}/mode/{:s}/difficulty/{:d}?page={:d}", url, levelHash, gameMode, difficulty, page);
                    break;
                }
                case PlatformLeaderboardsModel::ScoresScope::Friends: {
                    url = fmt::format("{:s}/around-friends/{:s}/mode/{:s}/difficulty/{:d}?page={:d}", url, levelHash, gameMode, difficulty, page);
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
                url = fmt::format("{:s}/around-region/{:s}/mode/{:s}/difficulty/{:d}?page={:d}", url, levelHash, gameMode, difficulty, page);
            } else {
                if(mode != "country") {
                    ERROR("Invalid location filter mode, falling back to country");
                }
                url = fmt::format("{:s}/around-country/{:s}/mode/{:s}/difficulty/{:d}?page={:d}", url, levelHash, gameMode, difficulty, page);
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

    void GetLeaderboardData(int maxMultipliedScore, BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, PlatformLeaderboardsModel::ScoresScope scope, int page,
                            std::function<void(Data::InternalLeaderboard)> finished,
                            bool filterAroundCountry)
    {
        std::string url = GetLeaderboardUrl(beatmapKey, scope, page, filterAroundCountry);

        if (url == "")
        {
            finished(GetLeaderboardError("Failed to get beatmap data! Please refresh"));
            return;
        }

        FixedSafePtr<BeatmapLevel> beatmapLevelSafe(beatmapLevel);

        int * const a = nullptr;
        int *b = a;

        WebUtils::GetAsync(
            url, [beatmapLevelSafe, beatmapKey, scope, page, filterAroundCountry, maxMultipliedScore, finished](long code, std::string result) {
                Data::InternalLeaderboard data;
                switch (code)
                {
                    case 200:
                        data = ParseLeaderboardData(result, beatmapLevelSafe.ptr(), beatmapKey, scope, page, filterAroundCountry, maxMultipliedScore);
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
        auto scores = ListW<LeaderboardTableView::ScoreData*>::New();
        scores->Add(LeaderboardTableView::ScoreData::New_ctor(0, error, 0, false));
        return Data::InternalLeaderboard(scores, {});
    }

    Data::InternalLeaderboard ParseLeaderboardData(std::string rawData, BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, PlatformLeaderboardsModel::ScoresScope scope,
                                                   int page, bool filterAroundCountry, int maxScore)
    {
        auto scores = ListW<LeaderboardTableView::ScoreData*>::New();
        auto modifiers = ListW<GameplayModifierParamsSO*>::New();
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
                    std::u16string formattedScore = FormatScore(((double)score.baseScore / (double)maxScore) * 100.0);
                    std::u16string formattedPP = FormatPP(score);
                    std::u16string result = Resize(leaderboardPlayerInfo.name.value() + formattedScore + formattedPP, 75);
                    INFO("score: {:d}, rank: {:d}", score.modifiedScore, score.rank);
                    scores->Add(LeaderboardTableView::ScoreData::New_ctor(score.modifiedScore, result, score.rank, false));
                    INFO("it's added now");
                    profilePictures.push_back(leaderboardPlayerInfo.profilePicture);
                }

                if (scores.size() == 0)
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
