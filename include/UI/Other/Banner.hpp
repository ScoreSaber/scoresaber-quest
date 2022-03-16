#pragma once

#include "CustomTypes/Components/ClickableText.hpp"
#include "UI/Other/PlayerProfileModal.hpp"

#include "HMUI/ImageView.hpp"
#include "HMUI/ModalView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include <string_view>

DECLARE_CLASS_CODEGEN(
    ScoreSaber::UI::Other, Banner, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_FIELD(QuestUI::Backgroundable*, bg);
    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, bgImage);
    DECLARE_INSTANCE_FIELD(ScoreSaber::CustomTypes::Components::ClickableText*, topText);
    DECLARE_INSTANCE_FIELD(ScoreSaber::CustomTypes::Components::ClickableText*, bottomText);
    DECLARE_INSTANCE_FIELD(ScoreSaber::UI::Other::PlayerProfileModal*, playerProfileModal);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::VerticalLayoutGroup*, loadingVertical);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, promptText);

    public
    :

    static ScoreSaber::UI::Other::Banner * Create(UnityEngine::Transform * parent);
    void Setup();
    void OpenMainMenuFlowCoordinator();
    void OpenPlayerInfoModal();
    void OpenSongInBrowser();

    void set_rainbow(bool value);
    void set_color(UnityEngine::Color color);

    void set_ranking(int rank, float pp);
    void set_status(std::string_view status, int scoreboardId);

    void set_loading(bool value);
    void Prompt(std::string status, bool loadingIndicator, float dismiss,
                std::function<void()> callback);

    void set_topText(std::u16string_view newText);
    void set_topText(std::string_view newText) { set_topText(to_utf16(newText)); };
    void set_bottomText(std::u16string_view newText);
    void set_bottomText(std::string_view newText) { set_bottomText(to_utf16(newText)); };

    private
    :

    bool rainbow = false;
    bool wasRainbow = false;
    float colorAngle = 0.0f;
    int scoreboardId;
    static constexpr const UnityEngine::Color defaultColor = {0, 0.47, 0.72, 1.0};
    custom_types::Helpers::Coroutine SetPrompt(
        std::string status, bool loadingIndicator, float dismiss,
        std::function<void()> callback);

)