#pragma once
#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/ComboController.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/IInitializable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include <vector>

using namespace UnityEngine;
using namespace GlobalNamespace;

#define INTERFACES                                                        \
    {                                                                     \
        classof(System::IDisposable*), classof(Zenject::IInitializable*), \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Recorders, ScoreEventRecorder, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Recorders", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_PRIVATE_FIELD(ScoreController*, _scoreController);
                                    DECLARE_PRIVATE_FIELD(ComboController*, _comboController);
                                    DECLARE_CTOR(ctor, AudioTimeSyncController* audioTimeSyncController, ScoreController* scoreController, ComboController* comboController);
                                    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
                                    DECLARE_INSTANCE_METHOD(void, ComboController_comboDidChangeEvent, int combo);
                                    DECLARE_INSTANCE_METHOD(void, ScoreController_scoreDidChangeEvent, int rawScore, int score);
                                    DECLARE_INSTANCE_METHOD(void, ScoreController_multiplierDidChangeEvent, int multiplier, float nextMultiplierProgress);
                                    std::vector<Data::Private::ScoreEvent> _scoreKeyFrames;
                                    std::vector<Data::Private::ComboEvent> _comboKeyFrames;
                                    std::vector<Data::Private::MultiplierEvent> _multiplierKeyFrames;
                                    System::Action_1<int> *comboDidChangeDelegate;
                                    System::Action_2<int, int> *scoreDidChangeDelegate;
                                    System::Action_2<int, float> *multiplierDidChangeDelegate;
                                    public:
                                    std::vector<Data::Private::ScoreEvent> ExportScoreKeyframes();
                                    std::vector<Data::Private::ComboEvent> ExportComboKeyframes();
                                    std::vector<Data::Private::MultiplierEvent> ExportMultiplierKeyframes();
                                    )

#undef INTERFACES