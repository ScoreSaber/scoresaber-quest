#include "ReplaySystem/UI/Components/AmeBar.hpp"
#include <System/Single.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Mathf.hpp>
#include <UnityEngine/Rect.hpp>
#include <UnityEngine/RectTransformUtility.hpp>
#include <UnityEngine/Resources.hpp>
#include "logging.hpp"
#include "questui/QuestUI.hpp"
#include <sstream>
#include <iomanip>

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI::Components, AmeBar);

namespace ScoreSaber::ReplaySystem::UI::Components
{
    float AmeBar::barFill
    {
        return _fillBarTransform->anchorMax.x;
    }

    void AmeBar::barFill = float value
    {
        _fillBarTransform->anchorMax = Vector2(Mathf::Lerp(-1.0f, 1.0f, value), _fillBarTransform->anchorMax.y);
    }

    void AmeBar::currentTime = float value
    {
        std::stringstream ss;
        ss << int(value / 60) << ":" << std::setw(2) << std::setfill('0') << int(fmod(value, 60));
        _currentTimeText->text = ss.str();
    }

    void AmeBar::endTime = float value
    {
        std::stringstream ss;
        ss << int(value / 60) << ":" << std::setw(2) << std::setfill('0') << int(fmod(value, 60));
        _endTimeText->text = ss.str();
    }

    void AmeBar::Setup(UnityEngine::RectTransform* fillBarTransform, UnityEngine::RectTransform* otherTransform)
    {
        _otherTransform = otherTransform;
        _rectTransform = transform->GetComponent<RectTransform*>();
        _fillBarTransform = fillBarTransform;
        _currentTimeText = CreateText();
        _currentTimeText->rectTransform->sizeDelta = fillBarTransform->sizeDelta;
        _currentTimeText->rectTransform->anchorMin = Vector2(0.0f, 0.5f);
        _currentTimeText->alignment = TMPro::TextAlignmentOptions::Left;
        _currentTimeText->m_text = "0:00";
        _currentTimeText->name = "Current Time";

        _endTimeText = CreateText();
        _endTimeText->rectTransform->sizeDelta = fillBarTransform->sizeDelta;
        _endTimeText->rectTransform->anchorMin = Vector2(0.0f, 0.5f);
        _endTimeText->alignment = TMPro::TextAlignmentOptions::Right;
        _endTimeText->m_text = "0:00";
        _endTimeText->name = "End Time";
    }

    void AmeBar::RegisterNode(ScoreSaber::ReplaySystem::UI::Components::AmeNode* node)
    {
        node->AddCallback([=](ScoreSaber::ReplaySystem::UI::Components::AmeNode* node, UnityEngine::Vector2 x, UnityEngine::Camera* camera) {
            DragCallback(node, x, camera);
        });
    }

    void AmeBar::UnregisterNode(ScoreSaber::ReplaySystem::UI::Components::AmeNode* node)
    {
        node->AddCallback(nullptr);
    }

    float AmeBar::GetNodePercent(ScoreSaber::ReplaySystem::UI::Components::AmeNode* node)
    {
        return PercentForX(node->transform->localPosition.x);
    }

    void AmeBar::AssignNodeToPercent(ScoreSaber::ReplaySystem::UI::Components::AmeNode* node, float percent)
    {
        node->transform->localPosition = Vector3(XForPercent(percent), node->transform->localPosition.y, 0.0f);
    }

    void AmeBar::DragCallback(ScoreSaber::ReplaySystem::UI::Components::AmeNode* node, UnityEngine::Vector2 x, UnityEngine::Camera* camera)
    {
        Vector2 computedVector;
        RectTransformUtility::ScreenPointToLocalPointInRectangle(_rectTransform, x, camera, &computedVector);
        if (System::Single::IsNaN(computedVector.x) || System::Single::IsNaN(computedVector.y))
        {
            return;
        }

        float computed = computedVector.x;
        float min = XForPercent(Mathf::Clamp(node->min, 0.0f, 1.0f));
        float max = XForPercent(Mathf::Clamp(node->max, 0.0f, 1.0f));

        if (computed > max || computed < min)
        {
            return;
        }

        node->transform->localPosition = Vector3(computed, node->transform->localPosition.y, 0.0f);
        node->SendUpdatePositionCall(PercentForX(computed));
    }

    float AmeBar::XForPercent(float percent)
    {
        float maxX = _rectTransform->rect.width;
        return Mathf::Lerp(-maxX, maxX, percent);
    }

    float AmeBar::PercentForX(float x)
    {
        float maxX = _rectTransform->rect.width;
        return Mathf::InverseLerp(-maxX, maxX, x);
    }

    HMUI::CurvedTextMeshPro* AmeBar::CreateText()
    {
        GameObject* textGameObject = GameObject::New_ctor("AmeText");
        textGameObject->transform->SetParent(transform, false);
        HMUI::CurvedTextMeshPro* curvedText = textGameObject->AddComponent<HMUI::CurvedTextMeshPro*>();
        curvedText->font = QuestUI::BeatSaberUI::GetMainTextFont();
        curvedText->fontSharedMaterial = QuestUI::BeatSaberUI::GetMainUIFontMaterial();
        curvedText->text = "";
        curvedText->rectTransform->anchorMin = Vector2::zero;
        curvedText->rectTransform->anchorMax = Vector2::one;
        return curvedText;
    }

} // namespace ScoreSaber::ReplaySystem::UI::Components