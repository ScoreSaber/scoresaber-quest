#pragma once
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounterDidChangeReceiver.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounterDidFinishReceiver.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "Utils/StringUtils.hpp"
#include "Zenject/MemoryPool_1.hpp"
#include "custom-types/shared/macros.hpp"
#include "logging.hpp"
#include <functional>

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace ScoreSaber::Data::Private;

#define DECLARE_OVERRIDE_METHOD_ACTUALLY_GOOD(ret, name, overridingMethodInfo, ...) \
  public:                                                                           \
    ret name(__VA_ARGS__);                                                          \
    ___CREATE_INSTANCE_METHOD(name, #name, (overridingMethodInfo->flags & ~METHOD_ATTRIBUTE_ABSTRACT) | METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_VIRTUAL, overridingMethodInfo)

static std::vector<Il2CppClass*> GetInterfaces()
{
    return {classof(GlobalNamespace::ISaberSwingRatingCounterDidFinishReceiver*)};
}

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, method, mptr, ...) \
    DECLARE_OVERRIDE_METHOD_ACTUALLY_GOOD(retval, method, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

___DECLARE_TYPE_WRAPPER_INHERITANCE(
    ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder, SwingFinisher,
    Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber",
    GetInterfaces(), 0, nullptr,

    DECLARE_OVERRIDE_METHOD_MATCH(void, HandleSaberSwingRatingCounterDidFinish, &ISaberSwingRatingCounterDidFinishReceiver::HandleSaberSwingRatingCounterDidFinish, ISaberSwingRatingCounter* saberSwingRatingCounter);
    DECLARE_INSTANCE_FIELD(NoteCutInfo, noteCutInfo);
    DECLARE_INSTANCE_FIELD(ISaberSwingRatingCounter*, saberSwingRatingCounter);
    DECLARE_INSTANCE_FIELD(float, timeWasCut);
    DECLARE_CTOR(ctor);

    public
    :

    NoteID noteId;
    std::function<void(SwingFinisher*)> _didFinish;
    void Init(NoteID noteID, ByRef<NoteCutInfo> noteCutInfo, std::function<void(SwingFinisher*)> didFinish, float timeWasCut);)

namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder
{
    void SetPool(Zenject::MemoryPool_1<SwingFinisher*>* finisherPool);
    void LevelStarted(ScoreController* scoreController, AudioTimeSyncController* audioTimeSyncController);
    void ScoreController_noteWasCutEvent(NoteData* noteData, ByRef<NoteCutInfo> noteCutInfo, int multiplier);
    void SwingFinisher_didFinish(SwingFinisher* swingFinisher);
    void ScoreController_noteWasMissedEvent(NoteData* noteData, int multiplier);
    vector<NoteEvent> Export();
} // namespace ScoreSaber::ReplaySystem::Recorders::NoteEventRecorder