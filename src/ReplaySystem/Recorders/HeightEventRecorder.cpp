#include "ReplaySystem/Recorders/HeightEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/PlayerHeightDetector.hpp"
#include "System/Action_1.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

namespace ScoreSaber::ReplaySystem::Recorders::HeightEventRecorder
{
    AudioTimeSyncController* _audioTimeSyncController;
    PlayerHeightDetector* _playerHeightDetector;
    vector<HeightEvent> _heightKeyframes;

    void LevelStarted(PlayerHeightDetector* playerHeightDetector, AudioTimeSyncController* audioTimeSyncController)
    {
        _heightKeyframes.clear();
        _audioTimeSyncController = audioTimeSyncController;
        _playerHeightDetector = playerHeightDetector;
        std::function<void(float)> fun = [&](float x) {
            PlayerHeightDetector_playerHeightDidChangeEvent(x);
        };
        auto delegate = il2cpp_utils::MakeDelegate<System::Action_1<float>*>(classof(System::Action_1<float>*), fun);
        playerHeightDetector->add_playerHeightDidChangeEvent(delegate);
    }

    void PlayerHeightDetector_playerHeightDidChangeEvent(float newHeight)
    {
        _heightKeyframes.push_back(HeightEvent(newHeight, _audioTimeSyncController->songTime));
    }

    vector<HeightEvent> Export()
    {
        return _heightKeyframes;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders::HeightEventRecorder