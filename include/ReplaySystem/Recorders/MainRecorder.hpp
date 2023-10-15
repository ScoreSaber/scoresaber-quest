#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "ReplaySystem/Recorders/PoseRecorder.hpp"
#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "ReplaySystem/Recorders/ScoreEventRecorder.hpp"
#include "ReplaySystem/Recorders/HeightEventRecorder.hpp"
#include "ReplaySystem/Recorders/EnergyEventRecorder.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/IInitializable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#define INTERFACES                                                        \
    {                                                                     \
        classof(System::IDisposable*), classof(Zenject::IInitializable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, MainRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(PoseRecorder*, _poseRecorder);
                                    DECLARE_PRIVATE_FIELD(MetadataRecorder*, _metadataRecorder);
                                    DECLARE_PRIVATE_FIELD(NoteEventRecorder*, _noteEventRecorder);
                                    DECLARE_PRIVATE_FIELD(ScoreEventRecorder*, _scoreEventRecorder);
                                    DECLARE_PRIVATE_FIELD(HeightEventRecorder*, _heightEventRecorder);
                                    DECLARE_PRIVATE_FIELD(EnergyEventRecorder*, _energyEventRecorder);
                                    DECLARE_CTOR(ctor, PoseRecorder* poseRecorder, MetadataRecorder* metadataRecorder, NoteEventRecorder* noteEventRecorder, ScoreEventRecorder* scoreEventRecorder, HeightEventRecorder* heightEventRecorder, EnergyEventRecorder* energyEventRecorder);
                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    public:
                                    std::shared_ptr<ScoreSaber::Data::Private::ReplayFile> ExportCurrentReplay();
                                    void StopRecording();
                                    )

#undef INTERFACES