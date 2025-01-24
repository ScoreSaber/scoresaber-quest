#include <GlobalNamespace/BeatmapDifficulty.hpp>
#include <GlobalNamespace/EnvironmentInfoSO.hpp>
#include <HMUI/CurvedCanvasSettings.hpp>
#include "ReplaySystem/UI/GameReplayUI.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include <TMPro/TMP_FontAsset.hpp>
#include <UnityEngine/Canvas.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/RenderMode.hpp>
#include <UnityEngine/Resources.hpp>
#include "logging.hpp"
#include <sstream>

using namespace UnityEngine;
using namespace GlobalNamespace;

typedef std::basic_stringstream<char16_t> u16sstream;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, GameReplayUI);

namespace ScoreSaber::ReplaySystem::UI
{
    void GameReplayUI::ctor(GameplayCoreSceneSetupData* gameplayCoreSceneSetupData)
    {
        _gameplayCoreSceneSetupData = gameplayCoreSceneSetupData;
    }

    void GameReplayUI::Start()
    {
        CreateReplayUI();
    }

    void GameReplayUI::CreateReplayUI()
    {
        StringW replayText;
        replayText += "REPLAY MODE - Watching ";
        replayText += ReplaySystem::ReplayLoader::CurrentPlayerName;
        replayText += " play ";
        replayText += ReplaySystem::ReplayLoader::CurrentBeatmapLevel->songAuthorName;
        replayText += " - ";
        replayText += ReplaySystem::ReplayLoader::CurrentBeatmapLevel->songName;
        replayText += " (";
        replayText += GetFriendlyDifficulty(ReplaySystem::ReplayLoader::CurrentBeatmapKey.difficulty);
        replayText += ")";
        float timeScale = 1.0f;
        if (ReplaySystem::ReplayLoader::LoadedReplay->noteKeyframes.size() > 0)
            timeScale = ReplaySystem::ReplayLoader::LoadedReplay->noteKeyframes[0].TimeSyncTimescale;
        if (timeScale != 1.0f) {
            stringstream ss;
            ss << std::setprecision(3) << " [" << timeScale * 100 << "%]";
            replayText += ss.str();
        }
        std::string friendlyMods = ReplaySystem::ReplayLoader::CurrentModifiers;
        if (friendlyMods != "") {
            replayText += " [";
            replayText += friendlyMods;
            replayText += "]";
        }
        auto _watermarkCanvas = GameObject::New_ctor("InGameReplayUI");
        // TODO fix the Interscope check (something here was null and thus caused a crash)
        //if (_gameplayCoreSceneSetupData->environmentInfo->environmentName == "Interscope") {
        //    _watermarkCanvas->transform->position = {0.0f, 3.5f, 12.0f};
        //} else {
            _watermarkCanvas->transform->position = {0.0f, 4.0f, 12.0f};
        //}
        _watermarkCanvas->transform->localScale = {0.025f, 0.025f, 0.025f};

        auto _canvas = _watermarkCanvas->AddComponent<Canvas*>();
        _watermarkCanvas->AddComponent<HMUI::CurvedCanvasSettings*>();
        _canvas->renderMode = RenderMode::WorldSpace;
        _canvas->enabled = false;
        auto _text = CreateText(_canvas->transform.cast<RectTransform>(), replayText, {0, 10}, {100, 20}, 15.0f);
        _text->alignment = TMPro::TextAlignmentOptions::Center;
        auto rectTransform = _text->transform;
        rectTransform->SetParent(_canvas->transform, false);
        _canvas->enabled = true;
    }

    TMPro::TextMeshProUGUI* GameReplayUI::CreateText(UnityEngine::RectTransform* parent, StringW text, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, float fontSize)
    {
        auto gameObject = GameObject::New_ctor("CustomUIText-ScoreSaber");
        gameObject->SetActive(false);
        auto textMeshProUGUI = gameObject->AddComponent<TMPro::TextMeshProUGUI*>();
        auto font = UnityEngine::Resources::FindObjectsOfTypeAll<TMPro::TMP_FontAsset*>()->First([](TMPro::TMP_FontAsset* t) { return t->name == "Teko-Medium SDF";});
        textMeshProUGUI->font = font;
        textMeshProUGUI->rectTransform->SetParent(parent, false);
        textMeshProUGUI->text = text;
        textMeshProUGUI->fontSize = fontSize;
        textMeshProUGUI->color = Color::get_white();
        textMeshProUGUI->rectTransform->anchorMin = {0.5f, 0.5f};
        textMeshProUGUI->rectTransform->anchorMax = {0.5f, 0.5f};
        textMeshProUGUI->rectTransform->sizeDelta = sizeDelta;
        textMeshProUGUI->rectTransform->anchoredPosition = anchoredPosition;
        gameObject->SetActive(true);
        return textMeshProUGUI;
    }

    std::string GameReplayUI::GetFriendlyDifficulty(GlobalNamespace::BeatmapDifficulty diff) {
        switch(diff) {
            case BeatmapDifficulty::Easy: return "Easy";
            case BeatmapDifficulty::Normal: return "Normal";
            case BeatmapDifficulty::Hard: return "Hard";
            case BeatmapDifficulty::Expert: return "Expert";
            case BeatmapDifficulty::ExpertPlus: return "Expert+";
            default: return "unknown";
        }
    }
} // namespace ScoreSaber::ReplaySystem::UI