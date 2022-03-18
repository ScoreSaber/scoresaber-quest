#pragma once
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounterDidFinishReceiver.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "Utils/StringUtils.hpp"
#include "custom-types/shared/macros.hpp"
#include "logging.hpp"
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

static std::vector<Il2CppClass*> GetInterfaces()
{
    return {classof(GlobalNamespace::ISaberSwingRatingCounterDidFinishReceiver*)};
}

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, method, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, method, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

___DECLARE_TYPE_WRAPPER_INHERITANCE(
    ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder, SwingFinisher,
    Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber",
    GetInterfaces(), 0, nullptr,

    DECLARE_INSTANCE_FIELD(NoteCutInfo, noteCutInfo);
    DECLARE_INSTANCE_FIELD(ISaberSwingRatingCounter*, saberSwingRatingCounter);
    DECLARE_INSTANCE_FIELD(float, timeWasCut);

    // example of making the interface method work:
    DECLARE_OVERRIDE_METHOD_MATCH(void, HandleSaberSwingRatingCounterDidFinish, &ISaberSwingRatingCounterDidFinishReceiver::HandleSaberSwingRatingCounterDidFinish, ISaberSwingRatingCounter* saberSwingRatingCounter);

    public
    :

    NoteID noteId;
    std::function<void(SwingFinisher*)> _didFinish;
    void Init(NoteID noteID, NoteCutInfo noteCutInfo, std::function<void(SwingFinisher*)> didFinish, float timeWasCut);

)

namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder
{
    void LevelStarted(ScoreController* scoreController, AudioTimeSyncController* audioTimeSyncController);
    void ScoreController_noteWasCutEvent(NoteData* noteData, ByRef<NoteCutInfo> noteCutInfo, int multiplier);
    void ScoreController_noteWasMissedEvent(NoteData* noteData, int multiplier);
    vector<NoteEvent> Export();
} // namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder