#pragma once

#include <GlobalNamespace/BeatmapCharacteristicSO.hpp>
#include <GlobalNamespace/BeatmapLevelLoader.hpp>
#include <GlobalNamespace/BeatmapLevel.hpp>
#include <GlobalNamespace/BeatmapKey.hpp>
#include <GlobalNamespace/BeatmapDataLoader.hpp>
#include <GlobalNamespace/BeatmapLevelsEntitlementModel.hpp>
#include <GlobalNamespace/BeatmapLevelsModel.hpp>
#include <functional>
#include <System/Collections/Generic/Dictionary_2.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include <unordered_map>
#include "Utils/SafePtr.hpp"

// create std::hash and std::equal_to for BeatmapKey so we can use it as a key in std::unordered_map
namespace std {
    template<>
    struct hash<FixedSafeValueType<GlobalNamespace::BeatmapKey>> {
        size_t operator()(const FixedSafeValueType<GlobalNamespace::BeatmapKey> &key) const {
            auto hashA = key->beatmapCharacteristic->serializedName->GetHashCode();
            auto hashB = (int)key->difficulty;
            auto hashC = key->levelId->GetHashCode();
            std::size_t combinedHash = 0;
            combinedHash ^= static_cast<std::size_t>(hashA) + 0x9e3779b97f4a7c15ULL + (combinedHash << 6) + (combinedHash >> 2);
            combinedHash ^= static_cast<std::size_t>(hashB) + 0x9e3779b97f4a7c15ULL + (combinedHash << 6) + (combinedHash >> 2);
            combinedHash ^= static_cast<std::size_t>(hashC) + 0x9e3779b97f4a7c15ULL + (combinedHash << 6) + (combinedHash >> 2);
            return combinedHash;
            //return const_cast<GlobalNamespace::BeatmapKey&>(key).GetHashCode(); // how on earth is this stripped???????????
        }
    };

    template<>
    struct equal_to<FixedSafeValueType<GlobalNamespace::BeatmapKey>> {
        bool operator()(const FixedSafeValueType<GlobalNamespace::BeatmapKey> &lhs, const FixedSafeValueType<GlobalNamespace::BeatmapKey> &rhs) const {
            return GlobalNamespace::BeatmapKey::op_Equality(*lhs, *rhs);
        }
    };
}

DECLARE_CLASS_CODEGEN(
    ScoreSaber::Utils, MaxScoreCache, Il2CppObject,
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapDataLoader*, _beatmapDataLoader);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapLevelsModel*, _beatmapLevelsModel);;

    DECLARE_CTOR(ctor, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel);

    // the cache needs to store BeatmapKeys as FixedSafeValueType so GC cannot collect any of its members.
    std::unordered_map<FixedSafeValueType<GlobalNamespace::BeatmapKey>, int> cache;
public:
    void GetMaxScore(GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey, const std::function<void(int)> &callback);
)
