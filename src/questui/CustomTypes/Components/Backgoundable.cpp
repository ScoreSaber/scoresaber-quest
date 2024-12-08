#include <sstream>
#include <string>
#include <map>

#include <beatsaber-hook/shared/utils/utils.h>
#include "questui/CustomTypes/Components/Backgroundable.hpp"
#include "questui/ArrayUtil.hpp"

#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Transform.hpp>
#include <HMUI/ImageView.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/HideFlags.hpp>

DEFINE_TYPE(QuestUI, Backgroundable);

std::map<std::string, std::string> backgrounds =  {
    {
		"round-rect-panel",
		"RoundRect10"
	},
	{
	    "panel-top",
	    "RoundRect10"
	},
	{
		"panel-fade-gradient",
		"RoundRect10Thin"
	},
	{
		"panel-top-gradient",
		"RoundRect10"
	},
	{
		"title-gradient",	
		"RoundRect10"
	}
};

std::map<std::string, std::string> objectNames =  {
    {
		"round-rect-panel",
		"KeyboardWrapper"
	},
	{
		"panel-top",
		"BG"
	},
	{
		"panel-fade-gradient",
		"Background"
	},
	{
		"panel-top-gradient",
		"BG"
	},
	{
		"title-gradient",	
		"BG"
	}
};

std::map<std::string, std::string> objectParentNames =  {
    {
		"round-rect-panel",
		"Wrapper"
	},
	{
		"panel-top",
		"PracticeButton"
	},
	{
		"panel-fade-gradient",
		"LevelListTableCell"
	},
	{
		"panel-top-gradient",
		"ActionButton"
	},
	{
		"title-gradient",	
		"TitleViewController"
	}
};

void QuestUI::Backgroundable::ApplyBackgroundWithAlpha(StringW name, float alpha) {
	
    std::string stringName(name);
    HMUI::ImageView* search = UnityEngine::Resources::FindObjectsOfTypeAll<HMUI::ImageView*>().Last([&stringName](HMUI::ImageView* x){ 
        UnityEngine::Sprite* sprite = x->sprite;
        if(sprite && sprite->name != backgrounds[stringName])
            return false;
		if(sprite && x->transform->parent->name != objectParentNames[stringName])
            return false;
        return x->name == objectNames[stringName];
    });
    if(!search) {
		getLogger().error("Couldn't find background: %s", stringName.c_str());
        return;
	}
	if(!background) 
		background = gameObject->AddComponent<HMUI::ImageView*>();
	if(background) {
		//Copy Image: some methods are probably not needed
		background->alphaHitTestMinimumThreshold = search->alphaHitTestMinimumThreshold;
		UnityEngine::Color backgroundColor = search->color;
		backgroundColor.a = alpha;
		background->color = backgroundColor;
		background->eventAlphaThreshold = search->eventAlphaThreshold;
		background->fillAmount = search->fillAmount;
		background->fillCenter = search->fillCenter;
		background->fillClockwise = search->fillClockwise;
		background->fillMethod = search->fillMethod;
		background->fillOrigin = search->fillOrigin;
		background->hideFlags = search->hideFlags;
		background->maskable = search->maskable;
		background->material = search->material;
		//background->name = search->name;
		background->onCullStateChanged = search->onCullStateChanged;
		background->overrideSprite = search->overrideSprite;
		background->pixelsPerUnitMultiplier = search->pixelsPerUnitMultiplier;
		background->preserveAspect = search->preserveAspect;
		background->raycastTarget = search->raycastTarget;
		background->sprite = search->sprite;
		background->tag = search->tag;
		background->type = search->type;
		background->useGUILayout = search->useGUILayout;
		background->useLegacyMeshGeneration = search->useLegacyMeshGeneration;
		background->useSpriteMesh = search->useSpriteMesh;

		background->enabled = true;
	} else {
		getLogger().error("Couldn't apply background!");
	}
}

void QuestUI::Backgroundable::ApplyBackground(StringW name) {
    ApplyBackgroundWithAlpha(name, 0.5f);
}