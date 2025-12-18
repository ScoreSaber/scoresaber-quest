#pragma once
#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/ComboController.hpp>
#include <GlobalNamespace/ScoreController.hpp>
#include <System/IDisposable.hpp>
#include <Zenject/DiContainer.hpp>
#include <Zenject/IInitializable.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>
#include <vector>
#include "Utils/DelegateUtils.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(
    ScoreSaber::ReplaySystem::Recorders,
    ScoreEventRecorder,
    System::Object,
    System::IDisposable*,
    Zenject::IInitializable*) {
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::ScoreController>, _scoreController);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::ComboController>, _comboController);
    DECLARE_CTOR(ctor, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ScoreController* scoreController, GlobalNamespace::ComboController* comboController);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_INSTANCE_METHOD(void, ComboController_comboDidChangeEvent, int combo);
    DECLARE_INSTANCE_METHOD(void, ScoreController_scoreDidChangeEvent, int rawScore, int score);
    DECLARE_INSTANCE_METHOD(void, ScoreController_multiplierDidChangeEvent, int multiplier, float nextMultiplierProgress);
    std::vector<Data::Private::ScoreEvent> _scoreKeyFrames;
    std::vector<Data::Private::ComboEvent> _comboKeyFrames;
    std::vector<Data::Private::MultiplierEvent> _multiplierKeyFrames;
    DelegateUtils::DelegateW<System::Action_1<int>> comboDidChangeDelegate;
    DelegateUtils::DelegateW<System::Action_2<int, int>> scoreDidChangeDelegate;
    DelegateUtils::DelegateW<System::Action_2<int, float>> multiplierDidChangeDelegate;
public:
    std::vector<Data::Private::ScoreEvent> ExportScoreKeyframes();
    std::vector<Data::Private::ComboEvent> ExportComboKeyframes();
    std::vector<Data::Private::MultiplierEvent> ExportMultiplierKeyframes();
};