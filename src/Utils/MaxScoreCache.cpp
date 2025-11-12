#include "Utils/MaxScoreCache.hpp"
#include <custom-types/shared/delegate.hpp>
#include <GlobalNamespace/IBeatmapLevelData.hpp>
#include <GlobalNamespace/ScoreModel.hpp>
#include <GlobalNamespace/LoadBeatmapLevelDataResult.hpp>
#include "GlobalNamespace/zzzz__BeatmapLevelDataVersion_def.hpp"
#include "GlobalNamespace/zzzz__BeatmapLevelsEntitlementModel_def.hpp"
#include "System/Threading/zzzz__CancellationToken_def.hpp"
#include "Utils/DelegateUtils.hpp"
#include "Utils/SafePtr.hpp"
#include "Utils/GCUtil.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "logging.hpp"

using namespace GlobalNamespace;
using namespace System::Threading;
using namespace std;
using namespace BSML;

DEFINE_TYPE(ScoreSaber::Utils, MaxScoreCache);

namespace ScoreSaber::Utils {
    void MaxScoreCache::ctor(BeatmapDataLoader* beatmapDataLoader, BeatmapLevelsModel* beatmapLevelsModel, BeatmapLevelsEntitlementModel* beatmapLevelsEntitlementModel) {
        INVOKE_CTOR();

        _beatmapDataLoader = beatmapDataLoader;
        _beatmapLevelsModel = beatmapLevelsModel;
        _beatmapLevelsEntitlementModel = beatmapLevelsEntitlementModel;
    }

    void MaxScoreCache::GetMaxScore(BeatmapLevel* beatmapLevel, BeatmapKey beatmapKey, const function<void(int)> &callback) {
        FixedSafeValueType<BeatmapKey> beatmapKeySafe(beatmapKey);
        if(cache.contains(beatmapKeySafe)) {
            callback(cache[beatmapKeySafe]);
            return;
        }

        FixedSafePtr<MaxScoreCache> self(this);
        FixedSafePtr<BeatmapLevel> beatmapLevelSafe(beatmapLevel);

        MainThreadScheduler::Schedule(gc_aware_function([self, beatmapLevelSafe, beatmapKeySafe, callback] {
            DelegateHelper::ContinueWith(self->_beatmapLevelsEntitlementModel->GetLevelDataVersionAsync(beatmapKeySafe->levelId, CancellationToken::get_None()), std::function(gc_aware_function(
                [self, beatmapLevelSafe, beatmapKeySafe, callback](BeatmapLevelDataVersion beatmapLevelDataVersion) {
                    DelegateHelper::ContinueWith(self->_beatmapLevelsModel->LoadBeatmapLevelDataAsync(beatmapKeySafe->levelId, beatmapLevelDataVersion, CancellationToken::get_None()), std::function(gc_aware_function(
                        [self, beatmapLevelSafe, beatmapKeySafe, callback, beatmapLevelDataVersion](LoadBeatmapLevelDataResult beatmapLevelDataResult) {
                            MainThreadScheduler::Schedule(gc_aware_function([self, beatmapLevelSafe, beatmapKeySafe, callback, beatmapLevelDataVersion, beatmapLevelDataResult]() {
                                DelegateHelper::ContinueWith(self->_beatmapDataLoader->LoadBeatmapDataAsync(
                                    beatmapLevelDataResult.beatmapLevelData, // ::GlobalNamespace::IBeatmapLevelData *beatmapLevelData,
                                    *beatmapKeySafe, // ::GlobalNamespace::BeatmapKey beatmapKey,
                                    beatmapLevelSafe->beatsPerMinute, // float_t startBpm,
                                    false, // bool loadingForDesignatedEnvironment,
                                    nullptr, // ::GlobalNamespace::IEnvironmentInfo *targetEnvironmentInfo
                                    nullptr, // ::GlobalNamespace::IEnvironmentInfo *originalEnvironmentInfo
                                    beatmapLevelDataVersion,// ::GlobalNamespace::BeatmapLevelDataVersion beatmapLevelDataVersion
                                    nullptr, // ::GlobalNamespace::GameplayModifiers *gameplayModifiers,
                                    nullptr, // ::GlobalNamespace::PlayerSpecificSettings *playerSpecificSettings,
                                    false// bool enableBeatmapDataCaching
                                ), std::function(gc_aware_function(
                                    [self, beatmapKeySafe, callback](IReadonlyBeatmapData *beatmapData) {
                                        int maxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(beatmapData);
                                        self->cache[beatmapKeySafe] = maxScore;
                                        callback(maxScore);
                                    }
                                )));
                            }));
                        }
                    )));
                }
            )));
        }));
    }
}