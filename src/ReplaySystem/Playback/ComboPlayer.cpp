#include "ReplaySystem/Playback/ComboPlayer.hpp"
#include "Data/Private/ReplayFile.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include <System/Action_1.hpp>
#include <UnityEngine/Animator.hpp>
#include <UnityEngine/Resources.hpp>
#include <metacore/shared/internals.hpp>

using namespace UnityEngine;
using namespace ScoreSaber::Data::Private;

DEFINE_TYPE(ScoreSaber::ReplaySystem::Playback, ComboPlayer);

namespace ScoreSaber::ReplaySystem::Playback
{
    void ComboPlayer::ctor(GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ComboController* comboController)
    {
        INVOKE_CTOR();
        _audioTimeSyncController = audioTimeSyncController;
        _comboController = comboController;
        // _comboUIController = comboUIController;
        _comboUIController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::ComboUIController*>()->First();
        _sortedNoteEvents = ReplayLoader::LoadedReplay->noteKeyframes;
        _sortedComboEvents = ReplayLoader::LoadedReplay->comboKeyframes;
    }

    void ComboPlayer::TimeUpdate(float newTime)
    {
        for (int c = 0; c < _sortedComboEvents.size(); c++)
        {
            // TODO: this has potential to have problems if _sortedComboEvents[c].Time is within an epsilon of newTime, potentially applying combo twice or not at all
            if (_sortedComboEvents[c].Time > newTime)
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
        int bombsHitL = 0;
        int bombsHitR = 0;

        int leftCombo = 0;
        int rightCombo = 0;

        int leftHighest = 0;
        int rightHighest = 0;

        for (int c = 0; c < previousComboEvents.size(); c++)
        {
            if (previousComboEvents[c].EventType == NoteEventType::BadCut || previousComboEvents[c].EventType == NoteEventType::GoodCut || previousComboEvents[c].EventType == NoteEventType::Miss)
            {
                cutOrMissRecorded++;
            }
            else if (previousComboEvents[c].EventType == NoteEventType::Bomb)
            {
                if (previousComboEvents[c].SaberType == 0) {
                    bombsHitL++;
                }
                else {
                    bombsHitR++;
                }
            }


            if (previousComboEvents[c].EventType == NoteEventType::GoodCut)
            {
                if (previousComboEvents[c].SaberType == 0)
                {
                    leftCombo++;
                    if (leftHighest < leftCombo) {
                        leftHighest = leftCombo;
                    }
                }
                else
                {
                    rightCombo++;
                    if (rightHighest < rightCombo) {
                        rightHighest = rightCombo;
                    }
                }
            }
            else{
                if (previousComboEvents[c].SaberType == 0) {
                    leftCombo = 0;
                }
                else {
                    rightCombo = 0;
                }
            }
        }
        _comboController->_combo = combo;
        _comboController->_maxCombo = cutOrMissRecorded;
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

        if ((combo == 0 && cutOrMissRecorded == 0) || !didLoseCombo)
        {
            _comboUIController->_animator->Rebind();
            _comboUIController->_fullComboLost = false;
        }
        else
        {
            _comboUIController->_animator->SetTrigger(_comboUIController->_comboLostId);
            _comboUIController->_fullComboLost = true;
        }


        MetaCore::Internals::combo = combo;
        MetaCore::Internals::highestCombo = cutOrMissRecorded == 0 ? 0 : std::max(combo, cutOrMissRecorded);
        MetaCore::Internals::leftCombo = leftCombo;
        MetaCore::Internals::rightCombo = rightCombo;
        MetaCore::Internals::highestLeftCombo = leftHighest;
        MetaCore::Internals::highestRightCombo = rightHighest;
        MetaCore::Internals::bombsLeftHit = bombsHitL;
        MetaCore::Internals::bombsRightHit = bombsHitR; 
    }
} // namespace ScoreSaber::ReplaySystem::Playback