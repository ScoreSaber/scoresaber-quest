#include "Utils/MaxScoreCache.hpp"
#include <custom-types/shared/delegate.hpp>
#include <GlobalNamespace/IBeatmapLevelData.hpp>
#include <GlobalNamespace/ScoreModel.hpp>
#include <GlobalNamespace/LoadBeatmapLevelDataResult.hpp>
#include "Utils/DelegateUtils.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "logging.hpp"

using namespace GlobalNamespace;
using namespace System::Threading;
using namespace std;
using namespace BSML;

DEFINE_TYPE(ScoreSaber::Utils, MaxScoreCache);

namespace ScoreSaber::Utils {
    void MaxScoreCache::ctor(BeatmapDataLoader* beatmapDataLoader, BeatmapLevelsModel* beatmapLevelsModel) {
        INVOKE_CTOR();

        _beatmapDataLoader = beatmapDataLoader;
        _beatmapLevelsModel = beatmapLevelsModel;
    }

    void MaxScoreCache::GetMaxScore(BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, const function<void(int)> &callback) {
        if(cache.contains(beatmapKey)) {
            callback(cache[beatmapKey]);
            return;
        }

        MainThreadScheduler::Schedule([this, beatmapKey, beatmapLevel, callback] {
            DelegateHelper::ContinueWith(_beatmapLevelsModel->LoadBeatmapLevelDataAsync(beatmapKey.levelId, CancellationToken::get_None()), std::function(
                [beatmapKey, beatmapLevel, this, callback](LoadBeatmapLevelDataResult beatmapLevelDataResult) {
                    MainThreadScheduler::Schedule([this, beatmapLevelDataResult, beatmapKey, beatmapLevel, callback]() {
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
                                INFO("step D");
                                int maxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(beatmapData);
                                INFO("step E");
                                cache[beatmapKey] = maxScore;
                                callback(maxScore);
                            }
                        ));
                    });
                }
            ));
        });
    }
}