#include "questui/QuestUI.hpp"

#include <GlobalNamespace/UIKeyboardManager.hpp>
#include <GlobalNamespace/ColorPickerButtonController.hpp>
#include <GlobalNamespace/HSVPanelController.hpp>
#include <GlobalNamespace/MenuShockwave.hpp>
#include <GlobalNamespace/LevelCollectionTableView.hpp>

#include <UnityEngine/Canvas.hpp>
#include <UnityEngine/CanvasGroup.hpp>
#include <UnityEngine/AdditionalCanvasShaderChannels.hpp>
#include <UnityEngine/RenderMode.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Rect.hpp>
#include <UnityEngine/RectOffset.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/TextureFormat.hpp>
#include <UnityEngine/TextureWrapMode.hpp>
#include <UnityEngine/ImageConversion.hpp>
#include <UnityEngine/Material.hpp>
#include <UnityEngine/UI/RectMask2D.hpp>
#include <UnityEngine/UI/ScrollRect.hpp>
#include <UnityEngine/UI/CanvasScaler.hpp>
#include <UnityEngine/Events/UnityAction_1.hpp>
#include <UnityEngine/Events/UnityAction.hpp>

#include <HMUI/Touchable.hpp>
#include <HMUI/HoverHintController.hpp>
#include <HMUI/TableView.hpp>
#include <HMUI/ImageView.hpp>
#include <HMUI/TextPageScrollView.hpp>
#include <HMUI/CurvedTextMeshPro.hpp>
#include <HMUI/TextSegmentedControl.hpp>
#include <HMUI/UIKeyboard.hpp>
#include <HMUI/CurvedCanvasSettings.hpp>
#include <HMUI/EventSystemListener.hpp>
#include <HMUI/DropdownWithTableView.hpp>
#include <HMUI/ButtonSpriteSwap.hpp>
#include <HMUI/TimeSlider.hpp>
#include <HMUI/ColorGradientSlider.hpp>
#include <HMUI/TextSegmentedControl.hpp>
#include <HMUI/HoverTextSetter.hpp>

#include <VRUIControls/VRGraphicRaycaster.hpp>
#include "BGLib/Polyglot/LocalizedTextMeshProUGUI.hpp"

#include <System/Convert.hpp>
#include <System/Action_2.hpp>
#include <System/Action.hpp>
#include <System/Collections/Generic/HashSet_1.hpp>

#include "Libraries/HM/HMLib/VR/HapticPresetSO.hpp"
#include "UI/Components/ExternalComponents.hpp"

#include <Zenject/DiContainer.hpp>

#define DEFAULT_BUTTONTEMPLATE "PracticeButton"

#include <custom-types/shared/delegate.hpp>

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace UnityEngine::Events;
using namespace TMPro;
using namespace HMUI;
using namespace VRUIControls;
using namespace Zenject;
using namespace BGLib::Polyglot;

namespace QuestUI {
    GameObject* beatSaberUIObject = nullptr;
    GameObject* dropdownListPrefab = nullptr;
    GameObject* modalPrefab = nullptr;
    
    void SetupPersistentObjects() {
        if(!beatSaberUIObject) {
            static ConstString name("BeatSaberUIObject");
            beatSaberUIObject = GameObject::New_ctor(name);
            GameObject::DontDestroyOnLoad(beatSaberUIObject);
            // beatSaberUIObject->AddComponent<MainThreadScheduler*>();
        }
        if(!dropdownListPrefab) {
            GameObject* search = Resources::FindObjectsOfTypeAll<SimpleTextDropdown*>()->First([](SimpleTextDropdown* x) { 
                    return x->transform->parent->name == "NormalLevels"; 
                }
            )->transform->parent->gameObject;
            dropdownListPrefab = Object::Instantiate(search, beatSaberUIObject->transform, false);
            static ConstString name("QuestUIDropdownListPrefab");
            dropdownListPrefab->name = name;
            dropdownListPrefab->SetActive(false);
        }
        if (!modalPrefab) {
            GameObject* search = Resources::FindObjectsOfTypeAll<ModalView*>()->First([](ModalView* x) { 
                    return x->transform->name == "DropdownTableView";
                }
            )->gameObject;
            modalPrefab = Object::Instantiate(search, beatSaberUIObject->transform, false);
            
            modalPrefab->GetComponent<ModalView*>()->_presentPanelAnimations = search->GetComponent<ModalView*>()->_presentPanelAnimations;
            modalPrefab->GetComponent<ModalView*>()->_dismissPanelAnimation = search->GetComponent<ModalView*>()->_dismissPanelAnimation;

            static ConstString name("QuestUIModalPrefab");
            modalPrefab->name = name;
            modalPrefab->SetActive(false);
        }
    }

    static PhysicsRaycasterWithCache* physicsRaycaster = nullptr;
    PhysicsRaycasterWithCache* GetPhysicsRaycasterWithCache()
    {
        if(!physicsRaycaster) physicsRaycaster = Resources::FindObjectsOfTypeAll<MainMenuViewController*>()->First()->GetComponent<VRGraphicRaycaster*>()->_physicsRaycaster;
        if(!physicsRaycaster) {
            return nullptr;
        }
        return physicsRaycaster;
    }

    static DiContainer* diContainer = nullptr;
    DiContainer* GetDiContainer()
    {
        if(!diContainer) diContainer = Resources::FindObjectsOfTypeAll<TextSegmentedControl*>()->FirstOrDefault([](TextSegmentedControl* x) { return x->transform->parent->name == "PlayerStatisticsViewController" && x->_container; })->_container;
        if(!diContainer) {
            return nullptr;
        }
        return diContainer;
    }

    static SafePtrUnity<Material> mat_UINoGlow;
    Material* NoGlowMaterial() {
        if(!mat_UINoGlow) mat_UINoGlow.emplace(Resources::FindObjectsOfTypeAll<Material*>()->First([](Material* x) { return x->name == "UINoGlow"; }));
        if(!mat_UINoGlow) {
            return nullptr;
        }

        return mat_UINoGlow.ptr();
    }

    static SafePtrUnity<TMP_FontAsset> mainTextFont;
    TMP_FontAsset* GetMainTextFont() {
        if(!mainTextFont) mainTextFont.emplace(Resources::FindObjectsOfTypeAll<TMP_FontAsset*>()->FirstOrDefault([](TMP_FontAsset* x) { return x->name == "Teko-Medium SDF"; }));
        if(!mainTextFont) {
            return nullptr;
        }
        return mainTextFont.ptr();
    }

    static SafePtrUnity<Material> mainUIFontMaterial;
    Material* GetMainUIFontMaterial() {
        if(!mainUIFontMaterial) mainUIFontMaterial.emplace(Resources::FindObjectsOfTypeAll<Material*>()->FirstOrDefault([](Material* x) { return x->name == "Teko-Medium SDF Curved Softer"; }));
        if(!mainUIFontMaterial) {
            return nullptr;
        }
        return mainUIFontMaterial.ptr();
    }

    void ClearCache() {
        diContainer = nullptr;
        physicsRaycaster = nullptr;
    }

    ModalView* CreateModal(Transform* parent, UnityEngine::Vector2 sizeDelta, UnityEngine::Vector2 anchoredPosition, std::function<void(ModalView*)> onBlockerClicked, bool dismissOnBlockerClicked) {
        static ConstString name("QuestUIModalPrefab");

        // declare var
        ModalView* orig = modalPrefab->GetComponent<ModalView*>();
        
        // instantiate
        GameObject* modalObj = Object::Instantiate(modalPrefab, parent, false);
        
        modalObj->name = name;
        modalObj->SetActive(false);

        // get the modal
        ModalView* modal = modalObj->GetComponent<ModalView*>();

        // copy fields
        modal->_presentPanelAnimations = orig->_presentPanelAnimations;
        modal->_dismissPanelAnimation = orig->_dismissPanelAnimation;
        modal->_container = GetDiContainer();
        modalObj->GetComponent<VRGraphicRaycaster*>()->_physicsRaycaster = GetPhysicsRaycasterWithCache();

        // destroy unneeded objects
        Object::DestroyImmediate(modalObj->GetComponent<TableView*>());
        Object::DestroyImmediate(modalObj->GetComponent<ScrollRect*>());
        Object::DestroyImmediate(modalObj->GetComponent<ScrollView*>());
        Object::DestroyImmediate(modalObj->GetComponent<EventSystemListener*>());

        // destroy all children except background
        int childCount = modal->transform->childCount;
        for (int i = 0; i < childCount; i++) {
            auto* child = modal->transform->GetChild(i)->GetComponent<RectTransform*>();

            if (child->gameObject->name == "BG") {
                child->anchoredPosition = Vector2::zero;
                child->sizeDelta = Vector2::zero;
                child->GetComponent<Image*>()->raycastTarget = true;
            }
            else {
                // yeet the child
                Object::Destroy(child->gameObject);
            }
        }

        // set recttransform data
        auto rect = modalObj->GetComponent<RectTransform*>();
        rect->anchorMin = {0.5f, 0.5f};
        rect->anchorMax = {0.5f, 0.5f};
        rect->sizeDelta = sizeDelta;
        rect->anchoredPosition = anchoredPosition;

        // add callback
        modal->add_blockerClickedEvent(
            custom_types::MakeDelegate<System::Action *>(classof(System::Action *), (std::function<void()>) [onBlockerClicked, modal, dismissOnBlockerClicked] () {
                if (onBlockerClicked)
                    onBlockerClicked(modal); 
                if (dismissOnBlockerClicked)
                    modal->Hide(true, nullptr);
            })
        );
        return modal;
    }

    TextMeshProUGUI* CreateText(Transform* parent, StringW text, UnityEngine::Vector2 anchoredPosition) {
        return CreateText(parent, text, true, anchoredPosition);
    }

    TextMeshProUGUI* CreateText(Transform* parent, StringW text, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta) {
        return CreateText(parent, text, true, anchoredPosition, sizeDelta);
    }

    TextMeshProUGUI* CreateText(Transform* parent, StringW text, bool italic) {
        return CreateText(parent, text, italic, UnityEngine::Vector2(0.0f, 0.0f), UnityEngine::Vector2(60.0f, 10.0f));
    }

    TextMeshProUGUI* CreateText(Transform* parent, StringW text, bool italic, UnityEngine::Vector2 anchoredPosition) {
        return CreateText(parent, text, italic, anchoredPosition, UnityEngine::Vector2(60.0f, 10.0f));
    }

    TextMeshProUGUI* CreateText(Transform* parent, StringW text, bool italic, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta) {
        static ConstString name("QuestUIText");
        GameObject* gameObj = GameObject::New_ctor(name);
        gameObj->SetActive(false);

        CurvedTextMeshPro* textMesh = gameObj->AddComponent<CurvedTextMeshPro*>();
        RectTransform* rectTransform = textMesh->rectTransform;
        rectTransform->SetParent(parent, false);
        textMesh->font = GetMainTextFont();
        textMesh->fontSharedMaterial = GetMainUIFontMaterial();
        if (italic) textMesh->fontStyle = TMPro::FontStyles::Italic;
        textMesh->text = text;
        textMesh->fontSize = 4.0f;
        textMesh->color = UnityEngine::Color::white;
        textMesh->richText = true;
        rectTransform->anchorMin = UnityEngine::Vector2(0.5f, 0.5f);
        rectTransform->anchorMax = UnityEngine::Vector2(0.5f, 0.5f);
        rectTransform->anchoredPosition = anchoredPosition;
        rectTransform->sizeDelta = sizeDelta;
        
        gameObj->AddComponent<LayoutElement*>();

        gameObj->SetActive(true);
        return textMesh;
    }

    Button* CreateUIButton(Transform* parent, StringW buttonText, std::string_view buttonTemplate, std::function<void()> onClick) {
        static std::unordered_map<std::string, SafePtrUnity<Button>> buttonCopyMap;
        auto& buttonCopy = buttonCopyMap[std::string(buttonTemplate)];
        if (!buttonCopy) {
            buttonCopy = Resources::FindObjectsOfTypeAll<Button*>()->LastOrDefault([&buttonTemplate](auto* x) { return x->name == buttonTemplate; });
        }

        Button* button = Object::Instantiate(buttonCopy.ptr(), parent, false);
        button->onClick = Button::ButtonClickedEvent::New_ctor();
        static ConstString name("QuestUIButton");
        button->name = name;
        if(onClick)
            button->onClick->AddListener(custom_types::MakeDelegate<UnityEngine::Events::UnityAction*>(onClick));

        LocalizedTextMeshProUGUI* localizer = button->GetComponentInChildren<LocalizedTextMeshProUGUI*>();
        if (localizer != nullptr)
            GameObject::Destroy(localizer);
        ExternalComponents* externalComponents = button->gameObject->AddComponent<ExternalComponents*>();

        TextMeshProUGUI* textMesh = button->GetComponentInChildren<TextMeshProUGUI*>();
        if (textMesh)
        {
            textMesh->richText = true;
            textMesh->alignment = TextAlignmentOptions::Center;
            textMesh->text = buttonText;
            externalComponents->Add(textMesh);
        }
        RectTransform* rectTransform = button->transform.cast<RectTransform>();
        rectTransform->anchorMin = UnityEngine::Vector2(0.5f, 0.5f);
        rectTransform->anchorMax = UnityEngine::Vector2(0.5f, 0.5f);
        rectTransform->pivot = UnityEngine::Vector2(0.5f, 0.5f);

        HorizontalLayoutGroup* horiztonalLayoutGroup = button->GetComponentInChildren<HorizontalLayoutGroup*>();
        if (horiztonalLayoutGroup != nullptr)
            externalComponents->Add(horiztonalLayoutGroup);
        
        // if the original button was for some reason not interactable, now it will be
        button->interactable = true;
        button->gameObject->SetActive(true);
        return button;
    }

    Button* CreateUIButton(Transform* parent, StringW buttonText, std::string_view buttonTemplate, UnityEngine::Vector2 anchoredPosition, std::function<void()> onClick) {
        Button* button = CreateUIButton(parent, buttonText, buttonTemplate, onClick);
        button->GetComponent<RectTransform*>()->anchoredPosition = anchoredPosition;
        return button;
    }

    Button* CreateUIButton(Transform* parent, StringW buttonText, std::string_view buttonTemplate, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, std::function<void()> onClick) {
        Button* button = CreateUIButton(parent, buttonText, buttonTemplate, anchoredPosition, onClick);
        button->GetComponent<RectTransform*>()->sizeDelta = sizeDelta;
        LayoutElement* layoutElement = button->GetComponent<LayoutElement*>();
        if(!layoutElement)
            layoutElement = button->gameObject->AddComponent<LayoutElement*>();
        layoutElement->minWidth = sizeDelta.x;
        layoutElement->minHeight = sizeDelta.y;
        layoutElement->preferredWidth = sizeDelta.x;
        layoutElement->preferredHeight = sizeDelta.y;
        layoutElement->flexibleWidth = sizeDelta.x;
        layoutElement->flexibleHeight = sizeDelta.y;
        return button;
    }

    Button* CreateUIButton(Transform* parent, StringW buttonText, std::function<void()> onClick) {
        return CreateUIButton(parent, buttonText, DEFAULT_BUTTONTEMPLATE, onClick);
    }

    Button* CreateUIButton(Transform* parent, StringW buttonText, UnityEngine::Vector2 anchoredPosition, std::function<void()> onClick) {
        return CreateUIButton(parent, buttonText, DEFAULT_BUTTONTEMPLATE, anchoredPosition, onClick);
    }

    Button* CreateUIButton(Transform* parent, StringW buttonText, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, std::function<void()> onClick) {
        return CreateUIButton(parent, buttonText, DEFAULT_BUTTONTEMPLATE, anchoredPosition, sizeDelta, onClick);
    }

    ImageView* CreateImage(Transform* parent, Sprite* sprite, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta) {
        static ConstString name("QuestUIImage");
        GameObject* gameObj = GameObject::New_ctor(name);
        ImageView* image = gameObj->AddComponent<ImageView*>();
        image->material = NoGlowMaterial();
        image->transform->SetParent(parent, false);
        image->sprite = sprite;
        RectTransform* rectTransform = image->transform.cast<UnityEngine::RectTransform>();
        rectTransform->anchorMin = UnityEngine::Vector2(0.5f, 0.5f);
        rectTransform->anchorMax = UnityEngine::Vector2(0.5f, 0.5f);
        rectTransform->anchoredPosition = anchoredPosition;
        rectTransform->sizeDelta = sizeDelta;
        
        gameObj->AddComponent<LayoutElement*>();
        return image;
    }
}