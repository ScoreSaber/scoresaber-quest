#include "Utils/MaxScoreCache.hpp"
#include <custom-types/shared/delegate.hpp>
#include <GlobalNamespace/IBeatmapLevelData.hpp>
#include <GlobalNamespace/ScoreModel.hpp>
#include <GlobalNamespace/LoadBeatmapLevelDataResult.hpp>
#include "Utils/DelegateUtils.hpp"

using namespace GlobalNamespace;
using namespace System::Threading;
using namespace std;

DEFINE_TYPE(ScoreSaber::Utils, MaxScoreCache);

namespace ScoreSaber::Utils {
    void MaxScoreCache::ctor(BeatmapLevelLoader* beatmapLevelLoader, BeatmapDataLoader* beatmapDataLoader, BeatmapLevelsEntitlementModel* beatmapLevelsEntitlementModel) {
        INVOKE_CTOR();

        _beatmapLevelLoader = beatmapLevelLoader;
        _beatmapDataLoader = beatmapDataLoader;
        _beatmapLevelsEntitlementModel = beatmapLevelsEntitlementModel;
    }

    void MaxScoreCache::GetMaxScore(BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, const function<void(int)> &callback) {
        if(cache.contains(beatmapKey)) {
            callback(cache[beatmapKey]);
            return;
        }

        callback(1000000);

        // TODO: reenable max score computation
        return;
        DelegateHelper::ContinueWith(_beatmapLevelLoader->LoadBeatmapLevelDataAsync(beatmapLevel, CancellationToken::get_None()), std::function(
            [beatmapKey, beatmapLevel, this, callback](LoadBeatmapLevelDataResult beatmapLevelDataResult) {
                DelegateHelper::ContinueWith(_beatmapDataLoader->LoadBeatmapDataAsync(
                    beatmapLevelDataResult.beatmapLevelData, // ::GlobalNamespace::IBeatmapLevelData *beatmapLevelData,
                    beatmapKey, // ::GlobalNamespace::BeatmapKey beatmapKey,
                    beatmapLevel->beatsPerMinute, // float_t startBpm,
                    false, // bool loadingForDesignatedEnvironment,
                    nullptr, // ::GlobalNamespace::IEnvironmentInfo *environmentInfo,
                    nullptr, // ::GlobalNamespace::GameplayModifiers *gameplayModifiers,
                    nullptr, // ::GlobalNamespace::PlayerSpecificSettings *playerSpecificSettings,
                    false// bool enableBeatmapDataCaching
                ), std::function(
                    [beatmapKey, this, callback](IReadonlyBeatmapData *beatmapData) {
                        int maxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(beatmapData);
                        cache[beatmapKey] = maxScore;
                        callback(maxScore);
                    }
                ));
            }
        ));
    }
}