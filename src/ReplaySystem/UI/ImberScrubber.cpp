#include "ReplaySystem/UI/ImberScrubber.hpp"
#include <HMUI/CurvedTextMeshPro.hpp>
#include "ReplaySystem/UI/Components/AmeClicker.hpp"
#include <UnityEngine/AdditionalCanvasShaderChannels.hpp>
#include <UnityEngine/Canvas.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/RectTransformUtility.hpp>
#include <UnityEngine/RenderMode.hpp>
#include <UnityEngine/UI/CanvasScaler.hpp>
#include <UnityEngine/Vector2.hpp>
#include <VRUIControls/VRGraphicRaycaster.hpp>
#include <bsml/shared/Helpers/utilities.hpp>
#include <bsml/shared/Helpers/getters.hpp>
#include "Utils/SafePtr.hpp"
#include "logging.hpp"
#include "questui/ArrayUtil.hpp"
#include "Utils/OperatorOverloads.hpp"

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
        _loopNode->gameObject->SetActive(value);
        _bar->AssignNodeToPercent(_loopNode, Mathf::Min(_maxPercent, 1.0f));
        MainNode_PositionDidChange(_bar->GetNodePercent(_mainNode));

        _mainNode->max = _maxPercent; // uwu owo owo uwu EVENTUALLY REPLACE WITH LEVEL FAILED TIME YEA YEA
    }
    void ImberScrubber::set_visibility(bool value)
    {
        _parent->gameObject->SetActive(value);
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
        _bar->transform->localScale = Vector3::get_one() * 0.001f;

        float initialSongTime = _audioTimeSyncController->songTime / _audioTimeSyncController->songEndTime;
        _bar->set_barFill(initialSongTime);

        auto barRectTransform = _bar->GetComponent<RectTransform*>();

        _mainNode = CreateSlideNode(barRectTransform);
        _loopNode = CreateSlideNode(barRectTransform);

        _bar->RegisterNode(_mainNode);
        _bar->RegisterNode(_loopNode);
        _bar->AssignNodeToPercent(_mainNode, initialSongTime);
        _bar->set_endTime(_audioTimeSyncController->songEndTime);
        set_loopMode(_loopMode);

        FixedSafePtr<ImberScrubber> self(this);
        _mainNode->PositionDidChange = [self](float value) {
            self->MainNode_PositionDidChange(value);
        };
        _loopNode->PositionDidChange = [self](float value) {
            self->LoopNode_PositionDidChange(value);
        };

        _mainNode->name = "Imber Main Node";
        _loopNode->name = "Imber Loop Node";

        if (_levelFailTime != 0.0f)
        {
            _failNode = CreateTextNode(barRectTransform, "FAILED", Color(0.7f, 0.1f, 0.15f, 1.0f));
            _failNode->name = "Imber Text Node";
            _failNode->moveable = false;
            if (!_allowPast)
            {
                _maxPercent = _levelFailTime / _audioTimeSyncController->songEndTime;
            }
            _bar->AssignNodeToPercent(_failNode, _levelFailTime / _audioTimeSyncController->songEndTime);
            _bar->AssignNodeToPercent(_loopNode, _maxPercent);
            _loopNode->max = _maxPercent;
        }

        _mainNode->max = _bar->GetNodePercent(_loopNode) - _minNodeDistance;
        _loopNode->min = _bar->GetNodePercent(_mainNode) + _minNodeDistance;

        auto gameObject = GameObject::New_ctor("Imber Scrubber Wrapper");
        _bar->gameObject->transform->SetParent(gameObject->transform, false);
        _parent = gameObject->transform;
        gameObject->layer = 5;
        set_visibility(false);
    }

    void ImberScrubber::MainNode_PositionDidChange(float value)
    {
        _bar->set_barFill(value);
        if (DidCalculateNewTime != nullptr)
        {
            DidCalculateNewTime(_audioTimeSyncController->songLength * value);
        }
        _bar->set_currentTime(_audioTimeSyncController->songLength * value);
        _loopNode->min = value + _minNodeDistance;
    }

    void ImberScrubber::LoopNode_PositionDidChange(float value)
    {
        _mainNode->max = Mathf::Min(_maxPercent, value) - _minNodeDistance;
    }

    void ImberScrubber::Tick()
    {
        float currentAudioProgress = _audioTimeSyncController->songTime / _audioTimeSyncController->songEndTime;
        if (!_mainNode->get_isBeingDragged())
        {
            if (!_loopMode)
            {
                set_mainNodeValue(currentAudioProgress);
            }
            _bar->set_currentTime(_audioTimeSyncController->songTime);
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
        rectTransformBar->sizeDelta = size;

        // Create the canvas
        auto canvas = ameBar->AddComponent<Canvas*>();
        canvas->additionalShaderChannels = AdditionalCanvasShaderChannels::TexCoord1;
        canvas->additionalShaderChannels = AdditionalCanvasShaderChannels::TexCoord2;
        canvas->renderMode = RenderMode::WorldSpace;
        canvas->worldCamera = camera;
        canvas->sortingOrder = 31;

        // And then the canvas's dependencies
        ameBar->AddComponent<CanvasScaler*>();
        _container->InstantiateComponent<VRUIControls::VRGraphicRaycaster*>(ameBar);

        auto rectTransform = rectTransformBar;

        // Create the backwall for proper raycast events.
        auto borderElement = CreateImage(rectTransform);
        auto borderRectTransform = borderElement->rectTransform;
        borderRectTransform->anchorMin = Vector2::get_zero();
        borderRectTransform->anchorMax = Vector2::get_one();
        borderRectTransform->sizeDelta = rectTransform->sizeDelta * 1.5f;
        borderElement->color = Color::get_clear();
        borderElement->name = "Box Border";

        // Create the background bar image
        auto backgroundImage = CreateImage(rectTransform);
        auto backgroundRectTransform = backgroundImage->rectTransform;
        backgroundRectTransform->sizeDelta = barSize;
        backgroundRectTransform->anchorMin = Vector2(0.0f, 0.5f);
        backgroundRectTransform->anchorMax = Vector2(1.0f, 0.5f);
        backgroundImage->color = Color::get_gray();
        backgroundImage->name = "Background Bar";

        // Create the progress bar image
        auto progressImage = CreateImage(rectTransform);
        auto progressRectTransform = progressImage->rectTransform;
        progressRectTransform->sizeDelta = barSize;
        progressRectTransform->anchorMin = Vector2(0.0f, 0.5f);
        progressRectTransform->anchorMax = Vector2(0.0f, 0.5f);
        progressImage->color = _scoreSaberBlue;
        progressImage->name = "Progress Bar";

        auto clickScrubImage = CreateImage(rectTransform);
        auto clickScrubRectTransform = clickScrubImage->rectTransform;
        clickScrubRectTransform->sizeDelta = Vector2(barSize.x, barSize.y * 2.25f);
        clickScrubRectTransform->anchorMin = Vector2(0.0f, 0.5f);
        clickScrubRectTransform->anchorMax = Vector2(1.0f, 0.5f);
        clickScrubImage->color = Color::get_clear();
        auto clicker = clickScrubImage->gameObject->AddComponent<AmeClicker*>();
        FixedSafePtr<ImberScrubber> self(this);
        clicker->Setup([self](float value) {
            self->ClickedBackground(value);
        });
        clickScrubImage->name = "Box Click Scrubber";

        // Create the bar
        auto bar = ameBar->AddComponent<AmeBar*>();
        bar->Setup(progressImage->rectTransform, backgroundImage->rectTransform);
        return bar;
    }

    void ImberScrubber::ClickedBackground(float value)
    {
        if (_mainNode->get_isBeingDragged())
        {
            if (DidCalculateNewTime != nullptr)
            {
                DidCalculateNewTime(_audioTimeSyncController->songLength * value);
            }
        }
    }

    HMUI::ImageView* ImberScrubber::CreateImage(RectTransform* parent)
    {
        auto imageGameObject = GameObject::New_ctor("ImberImage");
        auto image = imageGameObject->AddComponent<HMUI::ImageView*>();
        
        image->material = BSML::Helpers::GetUINoGlowMat();
        image->sprite = BSML::Utilities::ImageResources::GetWhitePixel();
        image->rectTransform->SetParent(parent, false);
        return image;
    }

    AmeNode* ImberScrubber::CreateSlideNode(RectTransform* transform)
    {
        auto nodeGameObject = GameObject::New_ctor("SlideNode");
        auto rectTransform = nodeGameObject->AddComponent<RectTransform*>();
        rectTransform->SetParent(transform, false);
        rectTransform->anchoredPosition = Vector2(-6.0f, -50.0f);
        rectTransform->sizeDelta = Vector2::get_one() * 100.0f;
        rectTransform->anchorMin = Vector2::get_one() / 2.0f;
        rectTransform->anchorMin = Vector2::get_one() / 2.0f;

        auto nodeImage = CreateImage(rectTransform);
        auto nodeImageRectTransform = nodeImage->rectTransform;
        nodeImageRectTransform->sizeDelta = Vector2::get_one() * 25.0f;
        nodeImageRectTransform->anchorMin = Vector2(0.5f, 1.0f);
        nodeImageRectTransform->anchorMax = Vector2(0.5f, 1.0f);
        nodeImage->name = "Marker";

        auto nodeStem = CreateImage(rectTransform);
        auto nodeStemRectTransform = nodeStem->rectTransform;
        nodeStemRectTransform->anchoredPosition = Vector2(0.0f, 15.0f);
        nodeStemRectTransform->sizeDelta = Vector2(2.5f, 75.0f);
        nodeStemRectTransform->anchorMin = Vector2::get_one() / 2.0f;
        nodeStemRectTransform->anchorMax = Vector2::get_one() / 2.0f;
        nodeStem->name = "Stem";

        auto nodeHandle = CreateImage(rectTransform);
        auto nodeHandleRectTransform = nodeHandle->rectTransform;
        nodeHandleRectTransform->localRotation = Quaternion::Euler(0.0f, 0.0f, 45.0f);
        nodeHandleRectTransform->anchoredPosition = Vector2(0.0f, -25.0f);
        nodeHandleRectTransform->sizeDelta = Vector2::get_one() * 30.0f;
        nodeHandleRectTransform->anchorMin = Vector2::get_one() / 2.0f;
        nodeHandleRectTransform->anchorMax = Vector2::get_one() / 2.0f;
        nodeHandle->name = "Handle";

        auto node = nodeGameObject->AddComponent<AmeNode*>();
        node->Init(nodeHandle->gameObject->AddComponent<AmeHandle*>());

        return node;
    }

    AmeNode* ImberScrubber::CreateTextNode(RectTransform* transform, StringW initialText, Color color)
    {
        auto nodeGameObject = GameObject::New_ctor("TextNode");
        auto rectTransform = nodeGameObject->AddComponent<RectTransform*>();
        rectTransform->SetParent(transform, false);
        rectTransform->anchoredPosition = Vector2(-6.0f, -50.0f);
        rectTransform->sizeDelta = Vector2::get_one() * 100.0f;
        rectTransform->anchorMin = Vector2::get_one() / 2.0f;
        rectTransform->anchorMin = Vector2::get_one() / 2.0f;

        auto nodeImage = CreateImage(rectTransform);
        auto nodeImageRectTransform = nodeImage->rectTransform;
        nodeImageRectTransform->sizeDelta = Vector2::get_one() * 25.0f;
        nodeImageRectTransform->anchorMin = Vector2(0.5f, 1.0f);
        nodeImageRectTransform->anchorMax = Vector2(0.5f, 1.0f);
        nodeImage->name = "Marker";
        nodeImage->color = color;

        auto textGameObject = GameObject::New_ctor("Text");
        textGameObject->transform->SetParent(rectTransform, false);

        auto curvedText = textGameObject->AddComponent<HMUI::CurvedTextMeshPro*>();
        curvedText->font = BSML::Helpers::GetMainTextFont();
        curvedText->fontSharedMaterial = BSML::Helpers::GetMainUIFontMaterial();
        curvedText->text = initialText;
        curvedText->rectTransform->anchorMin = Vector2(0.5f, 0.5f);
        curvedText->rectTransform->anchorMax = Vector2(0.5f, 0.5f);
        curvedText->alignment = TMPro::TextAlignmentOptions::Top;
        curvedText->color = color;

        auto node = nodeGameObject->AddComponent<AmeNode*>();
        return node;
    }

} // namespace ScoreSaber::ReplaySystem::UI