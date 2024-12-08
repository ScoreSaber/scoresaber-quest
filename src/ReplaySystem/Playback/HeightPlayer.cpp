#include "ReplaySystem/Playback/HeightPlayer.hpp"
#include <GlobalNamespace/PlayerSpecificSettings.hpp>
#include "ReplaySystem/ReplayLoader.hpp"
#include <System/Action_1.hpp>
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/Transform.hpp>
#include "logging.hpp"
#include <algorithm>

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, HeightPlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void HeightPlayer::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::PlayerHeightDetector* playerHeightDetector, GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData)
    {
        _audioTimeSyncController = audioTimeSyncController;
        _playerHeightDetector = playerHeightDetector;
        _sortedHeightEvents = ReplayLoader::LoadedReplay->heightKeyframes;
        _gameplayCoreSceneSetupData = gameplayCoreSceneSetupData;
    }

    void HeightPlayer::Initialize()
    {
        if (_gameplayCoreSceneSetupData->playerSpecificSettings->automaticPlayerHeight)
        {
            _playerHeightDetector->OnDestroy();
        }
    }

    void HeightPlayer::Tick()
    {
        optional<float> newHeight;
        while (_nextIndex < _sortedHeightEvents.size() && _audioTimeSyncController->songEndTime >= _sortedHeightEvents[_nextIndex].Time) {
            newHeight = _sortedHeightEvents[_nextIndex].Height;
            _nextIndex++;
        }
        if (newHeight.has_value()) {
            if (_playerHeightDetector->playerHeightDidChangeEvent != nullptr) {
                _playerHeightDetector->playerHeightDidChangeEvent->Invoke(newHeight.value());
            }
        }
    }

    void HeightPlayer::TimeUpdate(float songTime)
    {
        _nextIndex = _sortedHeightEvents.size();
        for (int c = 0; c < _sortedHeightEvents.size(); c++)
        {
            if (_sortedHeightEvents[c].Time > songTime) {
                _nextIndex = c;
                break;
            }
        }
        if (_nextIndex > 0) {
            if( _playerHeightDetector->playerHeightDidChangeEvent != nullptr) {
                _playerHeightDetector->playerHeightDidChangeEvent->Invoke(_sortedHeightEvents[_nextIndex - 1].Height);
            }
        }
    }
} // namespace ScoreSaber::ReplaySystem::Playback