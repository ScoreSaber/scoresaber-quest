#include "Data/InternalLeaderboard.hpp"
#include "beatsaber-hook/shared/utils/utils-functions.h"

inline rapidjson::GenericDocument<rapidjson::UTF16<char16_t>> getdoc(std::u16string_view string)
{
    rapidjson::GenericDocument<rapidjson::UTF16<char16_t>> doc;
    doc.Parse(string.data());
    return doc;
}

inline rapidjson::Document getdoc(std::string_view string)
{
    rapidjson::Document doc;
    doc.Parse(string.data());
    return doc;
}

using namespace GlobalNamespace;

namespace ScoreSaber::Data
{
    InternalLeaderboard::InternalLeaderboard(System::Collections::Generic::List_1<LeaderboardTableView::ScoreData*>* _leaderboardItems, std::optional<Leaderboard> _leaderboard)
    {
        leaderboardItems = _leaderboardItems;
        leaderboard = _leaderboard;
    }
    InternalLeaderboard::InternalLeaderboard() {}
}