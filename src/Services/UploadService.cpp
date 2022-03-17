

#include "Services/UploadService.hpp"
#include "Data/Private/ScoreSaberUploadData.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "Services/PlayerService.hpp"
#include "Utils/StringUtils.hpp"
#include "Utils/WebUtils.hpp"
#include "Utils/md5.h"
#include "logging.hpp"

using namespace StringUtils;

using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::Services::UploadService
{

    bool uploading;
    void UploadScore(std::string scorePacket, std::function<void(bool)> finished)
    {
        std::string url = "http://192.168.1.8:9999/api/game/upload";
        std::string postData = "data=" + scorePacket;
        WebUtils::PostAsync(url, postData, 30000, [=](long code, std::string result) {
            if (code == 200)
            {
                finished(true);
            }
            else
            {
                finished(false);
            }
            uploading = false;
        });
    }

    std::string CreateScorePacket(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, int rawScore,
                                  int modifiedScore, bool fullCombo, int badCutsCount, int missedCount, int maxCombo, float energy, GlobalNamespace::GameplayModifiers* gameplayModifiers)
    {
        uploading = true;
        auto previewBeatmapLevel = reinterpret_cast<IPreviewBeatmapLevel*>(difficultyBeatmap->get_level());
        Il2CppString* levelId = previewBeatmapLevel->get_levelID();
        levelId = levelId->Replace(StrToIl2cppStr("custom_level_"), Il2CppString::_get_Empty());

        std::string levelHash = Il2cppStrToStr(levelId);
        std::string gameMode = string_format("Solo%s", Il2cppStrToStr(difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName).c_str());
        int difficulty = BeatmapDifficultyMethods::DefaultRating(difficultyBeatmap->get_difficulty());

        std::string songName = Il2cppStrToStr(previewBeatmapLevel->get_songName());
        std::string songSubName = Il2cppStrToStr(previewBeatmapLevel->get_songSubName());
        std::string songAuthorName = Il2cppStrToStr(previewBeatmapLevel->get_songAuthorName());
        std::string levelAuthorName = Il2cppStrToStr(previewBeatmapLevel->get_levelAuthorName());
        int bpm = previewBeatmapLevel->get_beatsPerMinute();

        std::u16string playerName = ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.name;
        std::string playerId = ScoreSaber::Services::PlayerService::playerInfo.localPlayerData.id;
        auto modifiers = GetModifierList(gameplayModifiers, energy);
        int hmd = 32;

        ScoreSaberUploadData data(playerName, playerId, rawScore, levelHash, songName, songSubName, levelAuthorName, songAuthorName, bpm,
                                  difficulty, "infoHash", modifiers, gameMode, badCutsCount, missedCount, maxCombo, fullCombo, hmd);

        std::string uploadData = data.serialize();

        // TODO: Obfuscate
        std::string key = md5("f0b4a81c9bd3ded1081b365f7628781f-" + ScoreSaber::Services::PlayerService::playerInfo.playerKey + "-" + playerId + "-f0b4a81c9bd3ded1081b365f7628781f");

        std::vector<unsigned char> keyBytes(key.begin(), key.end());
        std::vector<unsigned char> uploadDataBytes(uploadData.begin(), uploadData.end());
        std::vector<unsigned char> encrypted = Swap(uploadDataBytes, keyBytes);
        std::string result = ConvertToHex(encrypted);
        transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    std::vector<std::string> GetModifierList(GlobalNamespace::GameplayModifiers* gameplayModifiers, float energy)
    {
        std::vector<std::string> results;
        if (gameplayModifiers->energyType == GameplayModifiers::EnergyType::Battery)
        {
            results.push_back("BE");
        }
        if (gameplayModifiers->noFailOn0Energy && energy == 0)
        {
            results.push_back("NF");
        }
        if (gameplayModifiers->instaFail)
        {
            results.push_back("IF");
        }
        if (gameplayModifiers->failOnSaberClash)
        {
            results.push_back("SC");
        }
        if (gameplayModifiers->enabledObstacleType == GameplayModifiers::EnabledObstacleType::NoObstacles)
        {
            results.push_back("NO");
        }
        if (gameplayModifiers->noBombs)
        {
            results.push_back("NB");
        }
        if (gameplayModifiers->strictAngles)
        {
            results.push_back("SA");
        }
        if (gameplayModifiers->disappearingArrows)
        {
            results.push_back("DA");
        }
        if (gameplayModifiers->ghostNotes)
        {
            results.push_back("GN");
        }
        if (gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::Slower)
        {
            results.push_back("SS");
        }
        if (gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::Faster)
        {
            results.push_back("FS");
        }
        if (gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::SuperFast)
        {
            results.push_back("SF");
        }
        if (gameplayModifiers->smallCubes)
        {
            results.push_back("SC");
        }
        if (gameplayModifiers->strictAngles)
        {
            results.push_back("SA");
        }
        if (gameplayModifiers->proMode)
        {
            results.push_back("PM");
        }
        if (gameplayModifiers->noArrows)
        {
            results.push_back("NA");
        }
        return results;
    }

    std::vector<unsigned char> Swap(std::vector<unsigned char> panda1, std::vector<unsigned char> panda2)
    {
        int N1 = 11;
        int N2 = 13;
        int NS = 257;

        for (int i = 0; i <= panda2.size() - 1; i++)
        {
            NS += NS % (panda2[i] + 1);
        }

        std::vector<unsigned char> T(panda1.size());
        for (int i = 0; i <= panda1.size() - 1; i++)
        {
            NS = panda2[i % panda2.size()] + NS;
            N1 = (NS + 5) * (N1 & 255) + (N1 >> 8);
            N2 = (NS + 7) * (N2 & 255) + (N2 >> 8);
            NS = ((N1 << 8) + N2) & 255;

            T[i] = static_cast<unsigned char>(panda1[i] ^ static_cast<unsigned char>(NS));
        }

        return T;
    }

    std::string ConvertToHex(const std::vector<unsigned char>& v)
    {
        std::stringstream buffer;
        for (int i = 0; i < v.size(); i++)
        {
            buffer << std::hex << std::setfill('0');
            buffer << std::setw(2) << static_cast<unsigned>(v[i]);
        }
        return buffer.str();
    }

} // namespace ScoreSaber::Services::UploadService
