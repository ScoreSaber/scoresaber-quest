#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"
#include "ReplaySystem/UI/GameReplayUI.hpp"
#include "ReplaySystem/ReplayLoader.hpp"
#include "TMPro/TMP_FontAsset.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RenderMode.hpp"
#include "UnityEngine/Resources.hpp"
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
        replayText += ReplaySystem::ReplayLoader::CurrentLevel->get_level()->i_IPreviewBeatmapLevel()->get_songAuthorName();
        replayText += " - ";
        replayText += ReplaySystem::ReplayLoader::CurrentLevel->get_level()->i_IPreviewBeatmapLevel()->get_songName();
        replayText += " (";
        replayText += GetFriendlyDifficulty(ReplaySystem::ReplayLoader::CurrentLevel->get_difficulty());
        replayText += ")";
        float timeScale = 1.0f;
        if (ReplaySystem::ReplayLoader::LoadedReplay->noteKeyframes.size() > 0)
            timeScale = ReplaySystem::ReplayLoader::LoadedReplay->noteKeyframes[0].TimeSyncTimescale;
        if (timeScale != 1.0f) {
            stringstream ss;
            ss << std::fixed << std::setprecision(3) << " [" << timeScale * 100 << "]";
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
        //    _watermarkCanvas->get_transform()->set_position({0.0f, 3.5f, 12.0f});
        //} else {
            _watermarkCanvas->get_transform()->set_position({0.0f, 4.0f, 12.0f});
        //}
        _watermarkCanvas->get_transform()->set_localScale({0.025f, 0.025f, 0.025f});

        auto _canvas = _watermarkCanvas->AddComponent<Canvas*>();
        _watermarkCanvas->AddComponent<HMUI::CurvedCanvasSettings*>();
        _canvas->set_renderMode(RenderMode::WorldSpace);
        _canvas->set_enabled(false);
        auto _text = CreateText(reinterpret_cast<RectTransform*>(_canvas->get_transform()), replayText, {0, 10}, {100, 20}, 15.0f);
        _text->set_alignment(TMPro::TextAlignmentOptions::Center);
        auto rectTransform = _text->get_transform();
        rectTransform->SetParent(_canvas->get_transform(), false);
        _canvas->set_enabled(true);
    }

    TMPro::TextMeshProUGUI* GameReplayUI::CreateText(UnityEngine::RectTransform* parent, StringW text, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, float fontSize)
    {
        auto gameObject = GameObject::New_ctor("CustomUIText-ScoreSaber");
        gameObject->SetActive(false);
        auto textMeshProUGUI = gameObject->AddComponent<TMPro::TextMeshProUGUI*>();
        auto font = UnityEngine::Resources::FindObjectsOfTypeAll<TMPro::TMP_FontAsset*>().First([](TMPro::TMP_FontAsset* t) { return t->get_name() == "Teko-Medium SDF";});
        textMeshProUGUI->set_font(font);
        textMeshProUGUI->get_rectTransform()->SetParent(parent, false);
        textMeshProUGUI->set_text(text);
        textMeshProUGUI->set_fontSize(fontSize);
        textMeshProUGUI->set_color(Color::get_white());
        textMeshProUGUI->get_rectTransform()->set_anchorMin({0.5f, 0.5f});
        textMeshProUGUI->get_rectTransform()->set_anchorMax({0.5f, 0.5f});
        textMeshProUGUI->get_rectTransform()->set_sizeDelta(sizeDelta);
        textMeshProUGUI->get_rectTransform()->set_anchoredPosition(anchoredPosition);
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
    
    std::string GameReplayUI::GetFriendlyModifiers(GameplayModifiers* gameplayModifiers)
    {
        if (!gameplayModifiers) return "";

        std::vector<std::string> result;

        if (gameplayModifiers->energyType == GameplayModifiers::EnergyType::Battery) {
            result.push_back("BE");
        }
        if (gameplayModifiers->noFailOn0Energy) {
            result.push_back("NF");
        }
        if (gameplayModifiers->instaFail) {
            result.push_back("IF");
        }
        if (gameplayModifiers->failOnSaberClash) {
            result.push_back("SC");
        }
        if (gameplayModifiers->enabledObstacleType == GameplayModifiers::EnabledObstacleType::NoObstacles) {
            result.push_back("NO");
        }
        if (gameplayModifiers->noBombs) {
            result.push_back("NB");
        }
        if (gameplayModifiers->strictAngles) {
            result.push_back("SA");
        }
        if (gameplayModifiers->disappearingArrows) {
            result.push_back("DA");
        }
        if (gameplayModifiers->ghostNotes) {
            result.push_back("GN");
        }
        if (gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::Slower) {
            result.push_back("SS");
        }
        if (gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::Faster) {
            result.push_back("FS");
        }
        if (gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::SuperFast) {
            result.push_back("SF");
        }
        if (gameplayModifiers->smallCubes) {
            result.push_back("SC");
        }
        if (gameplayModifiers->strictAngles) {
            result.push_back("SA");
        }
        if (gameplayModifiers->proMode) {
            result.push_back("PM");
        }
        if (gameplayModifiers->noArrows) {
            result.push_back("NA");
        }

        std::stringstream res;
        for(int i = 0; i < result.size(); ++i) {
            if(i > 0) res << ",";
            res << result[i];
        }
        return res.str();
    }

} // namespace ScoreSaber::ReplaySystem::UI