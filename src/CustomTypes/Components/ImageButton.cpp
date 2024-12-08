#include "CustomTypes/Components/ImageButton.hpp"

#include <GlobalNamespace/OVRInput.hpp>
#include <GlobalNamespace/VRController.hpp>
#include <HMUI/ButtonSpriteSwap.hpp>
#include <HMUI/ImageView.hpp>
#include <UnityEngine/Collider.hpp>
#include <UnityEngine/Physics.hpp>
#include <UnityEngine/Quaternion.hpp>
#include <UnityEngine/RaycastHit.hpp>
#include <UnityEngine/RectTransform.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Transform.hpp>
#include <UnityEngine/Vector3.hpp>
#include <VRUIControls/VRPointer.hpp>
#include <custom-types/shared/coroutine.hpp>
#include <custom-types/shared/macros.hpp>
#include "questui/QuestUI.hpp"
#include "Utils/OperatorOverloads.hpp"

DEFINE_TYPE(ScoreSaber::CustomTypes::Components, ImageButton);

using namespace ScoreSaber::CustomTypes::Components;
using namespace QuestUI;
using namespace UnityEngine;

float grabDist = 0.0f;
bool isHolding = false;
UnityEngine::Vector3 raycastPos;
UnityEngine::Quaternion raycastRot;

custom_types::Helpers::Coroutine Update(ImageButton* self)
{
    while (true)
    {
        if (!self)
        {
            co_return;
        }
        if (self->button->isPointerInside)
        {
            auto images = self->button->GetComponentsInChildren<HMUI::ImageView*>();
            for (int i = 0; i < images.size(); i++)
            {
                HMUI::ImageView* image = images[i];
                image->color = UnityEngine::Color(self->r * 0.6, self->g * 0.8f, self->b * 1.0f, 1.0f);
            }
        }
        else
        {
            auto images = self->button->GetComponentsInChildren<HMUI::ImageView*>();
            if (images[0]->color.r != self->r)
            {
                for (int i = 0; i < images.size(); i++)
                {
                    HMUI::ImageView* image = images[i];
                    image->set_color(
                        UnityEngine::Color(self->r, self->g, self->b, self->a));
                }
            }
        }
        co_yield nullptr;
    }
    co_return;
}

void ImageButton::Init(Transform* parent, Vector2 anchoredPosition, Vector2 sizeDelta, std::function<void()> onClick)
{
    if (anchoredPosition != Vector2(0.0f, 0.0f))
    {
        button = QuestUI::CreateUIButton(parent, std::string(), "SettingsButton", anchoredPosition, sizeDelta, onClick);
    }
    else
    {
        button = QuestUI::CreateUIButton(parent, std::string(), "SettingsButton", onClick);
    }
    QuestUI::SetButtonSprites(button, sprite, sprite);
    button->gameObject->AddComponent<Collider*>();

    if (sizeDelta != Vector2(0.0f, 0.0f))
    {
        
        UnityW<RectTransform> rectTransform = button->transform->GetChild(0).cast<RectTransform>();
        rectTransform->sizeDelta = sizeDelta;
    }

    this->pointerArray = UnityEngine::Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>();
    HMUI::ButtonSpriteSwap* swap = this->GetComponent<HMUI::ButtonSpriteSwap*>();

    auto images = button->GetComponentsInChildren<HMUI::ImageView*>();
    for (HMUI::ImageView* img : images)
    {
        this->r = img->color.r;
        this->g = img->color.g;
        this->b = img->color.b;
        this->a = img->color.a;
    }

    this->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(Update(this)));
}