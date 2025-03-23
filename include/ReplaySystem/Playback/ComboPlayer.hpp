#pragma once

#include "Data/Private/ReplayFile.hpp"
#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/ComboController.hpp>
#include <GlobalNamespace/ComboUIController.hpp>
#include <custom-types/shared/macros.hpp>
#include <lapiz/shared/macros.hpp>

DECLARE_CLASS_CODEGEN(
    ScoreSaber::ReplaySystem::Playback, ComboPlayer, Il2CppObject,
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::AudioTimeSyncController>, _audioTimeSyncController);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::ComboController>, _comboController);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityW<GlobalNamespace::ComboUIController>, _comboUIController);
    DECLARE_CTOR(ctor, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ComboController* comboController);
    DECLARE_INSTANCE_METHOD(void, TimeUpdate, float songTime);
    vector<Data::Private::NoteEvent> _sortedNoteEvents;
    vector<Data::Private::ComboEvent> _sortedComboEvents;
    void UpdateCombo(float time, int combo);)
