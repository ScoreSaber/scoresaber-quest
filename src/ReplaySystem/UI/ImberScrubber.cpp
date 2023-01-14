#include "ReplaySystem/UI/ImberScrubber.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "ReplaySystem/UI/Components/AmeClicker.hpp"
#include "UnityEngine/AdditionalCanvasShaderChannels.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/RectTransformUtility.hpp"
#include "UnityEngine/RenderMode.hpp"
#include "UnityEngine/UI/CanvasScaler.hpp"
#include "UnityEngine/Vector2.hpp"
#include "VRUIControls/VRGraphicRaycaster.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "logging.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "questui/shared/BeatSaberUI.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace GlobalNamespace;
using namespace ScoreSaber::ReplaySystem::UI::Components;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI, ImberScrubber);

namespace ScoreSaber::ReplaySystem::UI
{
    Transform* ImberScrubber::get_transform()
    {
        return _parent;
    }
    void ImberScrubber::set_loopMode(bool value)
    {
        _loopMode = value;
        _loopNode->get_gameObject()->SetActive(value);
        _bar->AssignNodeToPercent(_loopNode, Mathf::Min(_maxPercent, 1.0f));
        MainNode_PositionDidChange(_bar->GetNodePercent(_mainNode));

        _mainNode->max = _maxPercent; // uwu owo owo uwu EVENTUALLY REPLACE WITH LEVEL FAILED TIME YEA YEA
    }
    void ImberScrubber::set_visibility(bool value)
    {
        _parent->get_gameObject()->SetActive(value);
    }
    float ImberScrubber::get_mainNodeValue()
    {
        return _bar->GetNodePercent(_mainNode);
    }
    void ImberScrubber::set_mainNodeValue(float value)
    {
        _bar->AssignNodeToPercent(_mainNode, value);
    }
    // ctor
    void ImberScrubber::ctor(MainCamera* mainCamera, Zenject::DiContainer* container, AudioTimeSyncController* audioTimeSyncController)
    {
        INVOKE_CTOR();
        _mainCamera = mainCamera;
        _container = container;
        _audioTimeSyncController = audioTimeSyncController;
        _scoreSaberBlue = Color(0.0f, 0.4705882f, 0.7254902f, 1.0f);
    }
    void ImberScrubber::Setup(float levelFailTime, bool allowPast)
    {
        _levelFailTime = levelFailTime;
        _allowPast = allowPast;
    }
    void ImberScrubber::Initialize()
    {
        _bar = ImberScrubber::Create(_mainCamera->camera, Vector2(500.0f, 100.0f));
        _bar->get_transform()->set_localScale(Vector3::get_one() * 0.001f);

        float initialSongTime = _audioTimeSyncController->get_songTime() / _audioTimeSyncController->get_songEndTime();
        _bar->set_barFill(initialSongTime);

        auto barRectTransform = _bar->GetComponent<RectTransform*>();

        _mainNode = CreateSlideNode(barRectTransform);
        _loopNode = CreateSlideNode(barRectTransform);

        _bar->RegisterNode(_mainNode);
        _bar->RegisterNode(_loopNode);
        _bar->AssignNodeToPercent(_mainNode, initialSongTime);
        _bar->set_endTime(_audioTimeSyncController->get_songEndTime());
        set_loopMode(_loopMode);

        _mainNode->PositionDidChange = [&](float value) {
            MainNode_PositionDidChange(value);
        };
        _loopNode->PositionDidChange = [&](float value) {
            LoopNode_PositionDidChange(value);
        };

        _mainNode->set_name("Imber Main Node");
        _loopNode->set_name("Imber Loop Node");

        if (_levelFailTime != 0.0f)
        {
            _failNode = CreateTextNode(barRectTransform, "FAILED", Color(0.7f, 0.1f, 0.15f, 1.0f));
            _failNode->set_name("Imber Text Node");
            _failNode->moveable = false;
            if (!_allowPast)
            {
                _maxPercent = _levelFailTime / _audioTimeSyncController->get_songEndTime();
            }
            _bar->AssignNodeToPercent(_failNode, _levelFailTime / _audioTimeSyncController->get_songEndTime());
            _bar->AssignNodeToPercent(_loopNode, _maxPercent);
            _loopNode->max = _maxPercent;
        }

        _mainNode->max = _bar->GetNodePercent(_loopNode) - _minNodeDistance;
        _loopNode->min = _bar->GetNodePercent(_mainNode) + _minNodeDistance;

        auto gameObject = GameObject::New_ctor("Imber Scrubber Wrapper");
        _bar->get_gameObject()->get_transform()->SetParent(gameObject->get_transform(), false);
        _parent = gameObject->get_transform();
        gameObject->set_layer(5);
        set_visibility(false);
    }

    void ImberScrubber::MainNode_PositionDidChange(float value)
    {
        _bar->set_barFill(value);
        if (DidCalculateNewTime != nullptr)
        {
            DidCalculateNewTime(_audioTimeSyncController->get_songLength() * value);
        }
        _bar->set_currentTime(_audioTimeSyncController->get_songLength() * value);
        _loopNode->min = value + _minNodeDistance;
    }

    void ImberScrubber::LoopNode_PositionDidChange(float value)
    {
        _mainNode->max = Mathf::Min(_maxPercent, value) - _minNodeDistance;
    }

    void ImberScrubber::Tick()
    {
        float currentAudioProgress = _audioTimeSyncController->get_songTime() / _audioTimeSyncController->get_songEndTime();
        if (!_mainNode->get_isBeingDragged())
        {
            if (!_loopMode)
            {
                set_mainNodeValue(currentAudioProgress);
            }
            _bar->set_currentTime(_audioTimeSyncController->get_songTime());
            _bar->set_barFill(currentAudioProgress);
        }
        if (_loopMode)
        {
            if (currentAudioProgress >= _bar->GetNodePercent(_loopNode))
            {
                MainNode_PositionDidChange(get_mainNodeValue());
            }
        }
    }

    void ImberScrubber::Dispose()
    {
        _mainNode->PositionDidChange = nullptr;
        _loopNode->PositionDidChange = nullptr;
    }

    AmeBar* ImberScrubber::Create(Camera* camera, Vector2 size)
    {
        // Setup the main game object
        auto ameBar = GameObject::New_ctor("ImberScrubber: Ame Bar");
        auto rectTransformBar = ameBar->AddComponent<RectTransform*>();
        auto barSize = Vector2(size.x, size.y / 10.0f);
        rectTransformBar->set_sizeDelta(size);

        // Create the canvas
        auto canvas = ameBar->AddComponent<Canvas*>();
        canvas->set_additionalShaderChannels(AdditionalCanvasShaderChannels::TexCoord1);
        canvas->set_additionalShaderChannels(AdditionalCanvasShaderChannels::TexCoord2);
        canvas->set_renderMode(RenderMode::WorldSpace);
        canvas->set_worldCamera(camera);
        canvas->set_sortingOrder(31);

        // And then the canvas's dependencies
        ameBar->AddComponent<CanvasScaler*>();
        _container->InstantiateComponent<VRUIControls::VRGraphicRaycaster*>(ameBar);

        auto rectTransform = rectTransformBar;

        // Create the backwall for proper raycast events.
        auto borderElement = CreateImage(rectTransform);
        auto borderRectTransform = borderElement->get_rectTransform();
        borderRectTransform->set_anchorMin(Vector2::get_zero());
        borderRectTransform->set_anchorMax(Vector2::get_one());
        borderRectTransform->set_sizeDelta(rectTransform->get_sizeDelta() * 1.5f);
        borderElement->set_color(Color::get_clear());
        borderElement->set_name("Box Border");

        // Create the background bar image
        auto backgroundImage = CreateImage(rectTransform);
        auto backgroundRectTransform = backgroundImage->get_rectTransform();
        backgroundRectTransform->set_sizeDelta(barSize);
        backgroundRectTransform->set_anchorMin(Vector2(0.0f, 0.5f));
        backgroundRectTransform->set_anchorMax(Vector2(1.0f, 0.5f));
        backgroundImage->set_color(Color::get_gray());
        backgroundImage->set_name("Background Bar");

        // Create the progress bar image
        auto progressImage = CreateImage(rectTransform);
        auto progressRectTransform = progressImage->get_rectTransform();
        progressRectTransform->set_sizeDelta(barSize);
        progressRectTransform->set_anchorMin(Vector2(0.0f, 0.5f));
        progressRectTransform->set_anchorMax(Vector2(0.0f, 0.5f));
        progressImage->set_color(_scoreSaberBlue);
        progressImage->set_name("Progress Bar");

        auto clickScrubImage = CreateImage(rectTransform);
        auto clickScrubRectTransform = clickScrubImage->get_rectTransform();
        clickScrubRectTransform->set_sizeDelta(Vector2(barSize.x, barSize.y * 2.25f));
        clickScrubRectTransform->set_anchorMin(Vector2(0.0f, 0.5f));
        clickScrubRectTransform->set_anchorMax(Vector2(1.0f, 0.5f));
        clickScrubImage->set_color(Color::get_clear());
        auto clicker = clickScrubImage->get_gameObject()->AddComponent<AmeClicker*>();
        clicker->Setup([&](float value) {
            ClickedBackground(value);
        });
        clickScrubImage->set_name("Box Click Scrubber");

        // Create the bar
        auto bar = ameBar->AddComponent<AmeBar*>();
        bar->Setup(progressImage->get_rectTransform(), backgroundImage->get_rectTransform());
        return bar;
    }

    void ImberScrubber::ClickedBackground(float value)
    {
        if (_mainNode->get_isBeingDragged())
        {
            if (DidCalculateNewTime != nullptr)
            {
                DidCalculateNewTime(_audioTimeSyncController->get_songLength() * value);
            }
        }
    }

    HMUI::ImageView* ImberScrubber::CreateImage(RectTransform* parent)
    {
        auto imageGameObject = GameObject::New_ctor("ImberImage");
        auto image = imageGameObject->AddComponent<HMUI::ImageView*>();
        image->set_material(QuestUI::ArrayUtil::First(Resources::FindObjectsOfTypeAll<Material*>(), [](Material* x) { return x->get_name() == "UINoGlow"; }));
        image->set_sprite(BSML::Utilities::ImageResources::GetWhitePixel());
        image->get_rectTransform()->SetParent(parent, false);
        return image;
    }

    AmeNode* ImberScrubber::CreateSlideNode(RectTransform* transform)
    {
        auto nodeGameObject = GameObject::New_ctor("SlideNode");
        auto rectTransform = nodeGameObject->AddComponent<RectTransform*>();
        rectTransform->SetParent(transform, false);
        rectTransform->set_anchoredPosition(Vector2(-6.0f, -50.0f));
        rectTransform->set_sizeDelta(Vector2::get_one() * 100.0f);
        rectTransform->set_anchorMin(Vector2::get_one() / 2.0f);
        rectTransform->set_anchorMin(Vector2::get_one() / 2.0f);

        auto nodeImage = CreateImage(rectTransform);
        auto nodeImageRectTransform = nodeImage->get_rectTransform();
        nodeImageRectTransform->set_sizeDelta(Vector2::get_one() * 25.0f);
        nodeImageRectTransform->set_anchorMin(Vector2(0.5f, 1.0f));
        nodeImageRectTransform->set_anchorMax(Vector2(0.5f, 1.0f));
        nodeImage->set_name("Marker");

        auto nodeStem = CreateImage(rectTransform);
        auto nodeStemRectTransform = nodeStem->get_rectTransform();
        nodeStemRectTransform->set_anchoredPosition(Vector2(0.0f, 15.0f));
        nodeStemRectTransform->set_sizeDelta(Vector2(2.5f, 75.0f));
        nodeStemRectTransform->set_anchorMin(Vector2::get_one() / 2.0f);
        nodeStemRectTransform->set_anchorMax(Vector2::get_one() / 2.0f);
        nodeStem->set_name("Stem");

        auto nodeHandle = CreateImage(rectTransform);
        auto nodeHandleRectTransform = nodeHandle->get_rectTransform();
        nodeHandleRectTransform->set_localRotation(Quaternion::Euler(0.0f, 0.0f, 45.0f));
        nodeHandleRectTransform->set_anchoredPosition(Vector2(0.0f, -25.0f));
        nodeHandleRectTransform->set_sizeDelta(Vector2::get_one() * 30.0f);
        nodeHandleRectTransform->set_anchorMin(Vector2::get_one() / 2.0f);
        nodeHandleRectTransform->set_anchorMax(Vector2::get_one() / 2.0f);
        nodeHandle->set_name("Handle");

        auto node = nodeGameObject->AddComponent<AmeNode*>();
        node->Init(nodeHandle->get_gameObject()->AddComponent<AmeHandle*>());

        return node;
    }

    AmeNode* ImberScrubber::CreateTextNode(RectTransform* transform, StringW initialText, Color color)
    {
        auto nodeGameObject = GameObject::New_ctor("TextNode");
        auto rectTransform = nodeGameObject->AddComponent<RectTransform*>();
        rectTransform->SetParent(transform, false);
        rectTransform->set_anchoredPosition(Vector2(-6.0f, -50.0f));
        rectTransform->set_sizeDelta(Vector2::get_one() * 100.0f);
        rectTransform->set_anchorMin(Vector2::get_one() / 2.0f);
        rectTransform->set_anchorMin(Vector2::get_one() / 2.0f);

        auto nodeImage = CreateImage(rectTransform);
        auto nodeImageRectTransform = nodeImage->get_rectTransform();
        nodeImageRectTransform->set_sizeDelta(Vector2::get_one() * 25.0f);
        nodeImageRectTransform->set_anchorMin(Vector2(0.5f, 1.0f));
        nodeImageRectTransform->set_anchorMax(Vector2(0.5f, 1.0f));
        nodeImage->set_name("Marker");
        nodeImage->set_color(color);

        auto textGameObject = GameObject::New_ctor("Text");
        textGameObject->get_transform()->SetParent(rectTransform, false);

        auto curvedText = textGameObject->AddComponent<HMUI::CurvedTextMeshPro*>();
        curvedText->set_font(QuestUI::BeatSaberUI::GetMainTextFont());
        curvedText->set_fontSharedMaterial(QuestUI::BeatSaberUI::GetMainUIFontMaterial());
        curvedText->set_text(initialText);
        curvedText->get_rectTransform()->set_anchorMin(Vector2(0.5f, 0.5f));
        curvedText->get_rectTransform()->set_anchorMax(Vector2(0.5f, 0.5f));
        curvedText->set_alignment(TMPro::TextAlignmentOptions::Top);
        curvedText->set_color(color);

        auto node = nodeGameObject->AddComponent<AmeNode*>();
        return node;
    }

} // namespace ScoreSaber::ReplaySystem::UI