#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "ReplaySystem/Recorders/PoseRecorder.hpp"
#include "ReplaySystem/Recorders/MetadataRecorder.hpp"
#include "ReplaySystem/Recorders/NoteEventRecorder.hpp"
#include "ReplaySystem/Recorders/ScoreEventRecorder.hpp"
#include "ReplaySystem/Recorders/HeightEventRecorder.hpp"
#include "ReplaySystem/Recorders/EnergyEventRecorder.hpp"
#include <System/IDisposable.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

#define INTERFACES                                                        \
    {                                                                     \
        classof(System::IDisposable*), classof(Zenject::IInitializable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, MainRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_PRIVATE(PoseRecorder*, _poseRecorder);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(MetadataRecorder*, _metadataRecorder);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(NoteEventRecorder*, _noteEventRecorder);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(ScoreEventRecorder*, _scoreEventRecorder);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(HeightEventRecorder*, _heightEventRecorder);
                                    DECLARE_INSTANCE_FIELD_PRIVATE(EnergyEventRecorder*, _energyEventRecorder);
                                    DECLARE_CTOR(ctor, PoseRecorder* poseRecorder, MetadataRecorder* metadataRecorder, NoteEventRecorder* noteEventRecorder, ScoreEventRecorder* scoreEventRecorder, HeightEventRecorder* heightEventRecorder, EnergyEventRecorder* energyEventRecorder);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
                                    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
                                    public:
                                    std::shared_ptr<ScoreSaber::Data::Private::ReplayFile> ExportCurrentReplay();
                                    void StopRecording();
                                    )

#undef INTERFACES