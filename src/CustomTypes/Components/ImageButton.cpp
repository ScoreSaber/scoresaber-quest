#include "CustomTypes/Components/ImageButton.hpp"

#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "HMUI/ButtonSpriteSwap.hpp"
#include "HMUI/ImageView.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "VRUIControls/VRPointer.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"

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
            for (int i = 0; i < images->Length(); i++)
            {
                HMUI::ImageView* image = images->get(i);
                image->set_color(UnityEngine::Color(self->r * 0.6, self->g * 0.8f, self->b * 1.0f, 1.0f));
            }
        }
        else
        {
            auto images = self->button->GetComponentsInChildren<HMUI::ImageView*>();
            if (images->get(0)->get_color().r != self->r)
            {
                for (int i = 0; i < images->Length(); i++)
                {
                    HMUI::ImageView* image = images->get(i);
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
        button = QuestUI::BeatSaberUI::CreateUIButton(parent, std::string(), "SettingsButton", anchoredPosition, sizeDelta, onClick);
    }
    else
    {
        button = QuestUI::BeatSaberUI::CreateUIButton(parent, std::string(), "SettingsButton", onClick);
    }
    BeatSaberUI::SetButtonSprites(button, sprite, sprite);
    button->get_gameObject()->AddComponent<Collider*>();

    if (sizeDelta != Vector2(0.0f, 0.0f))
    {
        RectTransform* rectTransform = reinterpret_cast<RectTransform*>(button->get_transform()->GetChild(0));
        rectTransform->set_sizeDelta(sizeDelta);
    }

    this->pointerArray = UnityEngine::Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>();
    HMUI::ButtonSpriteSwap* swap = this->GetComponent<HMUI::ButtonSpriteSwap*>();

    auto images = button->GetComponentsInChildren<HMUI::ImageView*>();
    for (int i = 0; i < images->Length(); i++)
    {
        HMUI::ImageView* img = images->get(i);
        this->r = img->get_color().r;
        this->g = img->get_color().g;
        this->b = img->get_color().b;
        this->a = img->get_color().a;
    }

    GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(Update(this)));
}