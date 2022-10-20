#include "ReplaySystem/Playback/ComboPlayer.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "System/Action_1.hpp"
#include "UnityEngine/Animator.hpp"
#include "logging.hpp"
#include <algorithm>

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, ComboPlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void ComboPlayer::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ComboController* comboController, GlobalNamespace::ComboUIController* comboUIController)
    {
        _audioTimeSyncController = audioTimeSyncController;
        _comboController = comboController;
        _comboUIController = comboUIController;
        _sortedNoteEvents = ReplayLoader::LoadedReplay->noteKeyframes;
        _sortedComboEvents = ReplayLoader::LoadedReplay->comboKeyframes;
    }
    void ComboPlayer::TimeUpdate(float newTime)
    {
        for (int c = 0; c < _sortedComboEvents.size(); c++)
        {
            if (_sortedComboEvents[c].Time >= newTime)
            {
                UpdateCombo(newTime, c != 0 ? _sortedComboEvents[c - 1].Combo : 0);
                return;
            }
        }
        UpdateCombo(newTime, _sortedComboEvents[_sortedComboEvents.size() - 1].Combo);
    }
    void ComboPlayer::UpdateCombo(float time, int combo)
    {
        auto previousComboEvents = std::vector<NoteEvent>();
        for (int c = 0; c < _sortedNoteEvents.size(); c++)
        {
            if (_sortedNoteEvents[c].EventType != NoteEventType::None && time > _sortedNoteEvents[c].Time)
            {
                previousComboEvents.push_back(_sortedNoteEvents[c]);
            }
        }
        int cutOrMissRecorded = 0;
        for (int c = 0; c < previousComboEvents.size(); c++)
        {
            if (previousComboEvents[c].EventType == NoteEventType::BadCut || previousComboEvents[c].EventType == NoteEventType::GoodCut || previousComboEvents[c].EventType == NoteEventType::Miss)
            {
                cutOrMissRecorded++;
            }
        }
        _comboController->combo = combo;
        _comboController->maxCombo = cutOrMissRecorded;
        if (_comboController->comboDidChangeEvent != nullptr)
        {
            _comboController->comboDidChangeEvent->Invoke(combo);
        }

        bool didLoseCombo = false;
        for (int c = 0; c < _sortedComboEvents.size(); c++)
        {
            if (time > _sortedComboEvents[c].Time && _sortedComboEvents[c].Combo == 0)
            {
                didLoseCombo = true;
            }
        }

        if ((combo == 0 && cutOrMissRecorded > 0) || didLoseCombo)
        {
            _comboUIController->animator->Rebind();
            _comboUIController->fullComboLost = false;
        }
        else
        {
            _comboUIController->animator->SetTrigger(_comboUIController->comboLostId);
            _comboUIController->fullComboLost = true;
        }
    }
} // namespace ScoreSaber::ReplaySystem::Playback