#include "ReplaySystem/Recorders/HeightEventRecorder.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/PlayerHeightDetector.hpp"
#include "System/Action_1.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "Utils/StringUtils.hpp"
#include "logging.hpp"
#include "custom-types/shared/delegate.hpp"
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Recorders, HeightEventRecorder);

namespace ScoreSaber::ReplaySystem::Recorders
{
    void HeightEventRecorder::ctor(AudioTimeSyncController* audioTimeSyncController, Zenject::DiContainer* container)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _playerHeightDetector = container->TryResolve<PlayerHeightDetector*>();
    }
    
    void HeightEventRecorder::Initialize()
    {
        if(_playerHeightDetector != nullptr) {
            playerHeightDidChangeDelegate = custom_types::MakeDelegate<System::Action_1<float>*>(classof(System::Action_1<float>*), (function<void(float)>)[&](float energy) {PlayerHeightDetector_playerHeightDidChangeEvent(energy);});
            _playerHeightDetector->add_playerHeightDidChangeEvent(playerHeightDidChangeDelegate);
        }
    }

    void HeightEventRecorder::Dispose()
    {
        if(_playerHeightDetector != nullptr && playerHeightDidChangeDelegate) {
            _playerHeightDetector->remove_playerHeightDidChangeEvent(playerHeightDidChangeDelegate);
        }
    }

    void HeightEventRecorder::PlayerHeightDetector_playerHeightDidChangeEvent(float newHeight)
    {
        _heightKeyframes.push_back(HeightEvent(newHeight, _audioTimeSyncController->songTime));
    }

    vector<HeightEvent> HeightEventRecorder::Export()
    {
        return _heightKeyframes;
    }
} // namespace ScoreSaber::ReplaySystem::Recorders