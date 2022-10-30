#include "ReplaySystem/UI/Components/AmeBar.hpp"
#include "System/Single.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/RectTransformUtility.hpp"
#include "UnityEngine/Resources.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;

DEFINE_TYPE(ScoreSaber::ReplaySystem::UI::Components, AmeBar);

namespace ScoreSaber::ReplaySystem::UI::Components
{
    float AmeBar::get_barFill()
    {
        return _fillBarTransform->get_anchorMax().x;
    }

    void AmeBar::set_barFill(float value)
    {
        _fillBarTransform->set_anchorMax(Vector2(Mathf::Lerp(-1.0f, 1.0f, value), _fillBarTransform->get_anchorMax().y));
    }

    void AmeBar::set_currentTime(float value)
    {
        // TODO: Implement properly
        _currentTimeText->set_text(std::to_string(value));
    }

    void AmeBar::set_endTime(float value)
    {
        // TODO: Implement properly
        _endTimeText->set_text(std::to_string(value));
    }

    void AmeBar::Setup(UnityEngine::RectTransform* fillBarTransform, UnityEngine::RectTransform* otherTransform)
    {
        _otherTransform = otherTransform;
        _rectTransform = get_transform()->GetComponent<RectTransform*>();
        _fillBarTransform = fillBarTransform;
        _currentTimeText = CreateText();
        _currentTimeText->get_rectTransform()->set_sizeDelta(fillBarTransform->get_sizeDelta());
        _currentTimeText->get_rectTransform()->set_anchorMin(Vector2(0.0f, 0.5f));
        _currentTimeText->set_alignment(TMPro::TextAlignmentOptions::Left);
        _currentTimeText->m_text = "0:00";
        _currentTimeText->set_name("Current Time");

        _endTimeText = CreateText();
        _endTimeText->get_rectTransform()->set_sizeDelta(fillBarTransform->get_sizeDelta());
        _endTimeText->get_rectTransform()->set_anchorMin(Vector2(0.0f, 0.5f));
        _endTimeText->set_alignment(TMPro::TextAlignmentOptions::Right);
        _endTimeText->m_text = "0:00";
        _endTimeText->set_name("End Time");
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
        return PercentForX(node->get_transform()->get_localPosition().x);
    }

    void AmeBar::AssignNodeToPercent(ScoreSaber::ReplaySystem::UI::Components::AmeNode* node, float percent)
    {
        node->get_transform()->set_localPosition(Vector3(XForPercent(percent), node->get_transform()->get_localPosition().y, 0.0f));
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

        node->get_transform()->set_localPosition(Vector3(computed, node->get_transform()->get_localPosition().y, 0.0f));
        node->SendUpdatePositionCall(PercentForX(computed));
    }

    float AmeBar::XForPercent(float percent)
    {
        float maxX = _rectTransform->get_rect().get_width();
        return Mathf::Lerp(-maxX, maxX, percent);
    }

    float AmeBar::PercentForX(float x)
    {
        float maxX = _rectTransform->get_rect().get_width();
        return Mathf::InverseLerp(-maxX, maxX, x);
    }

    HMUI::CurvedTextMeshPro* AmeBar::CreateText()
    {
        GameObject* textGameObject = GameObject::New_ctor("AmeText");
        textGameObject->get_transform()->SetParent(get_transform(), false);
        HMUI::CurvedTextMeshPro* curvedText = textGameObject->AddComponent<HMUI::CurvedTextMeshPro*>();
        curvedText->set_font(QuestUI::BeatSaberUI::GetMainTextFont());
        curvedText->set_fontSharedMaterial(QuestUI::BeatSaberUI::GetMainUIFontMaterial());
        curvedText->set_text("");
        curvedText->get_rectTransform()->set_anchorMin(Vector2::get_zero());
        curvedText->get_rectTransform()->set_anchorMax(Vector2::get_one());
        return curvedText;
    }

} // namespace ScoreSaber::ReplaySystem::UI::Components