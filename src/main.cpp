#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include "ui/GalleryLayer.hpp"

using namespace geode::prelude;

class $modify(GarageLayer, GJGarageLayer)
{
	bool init()
	{
		if (!GJGarageLayer::init())
			return false;

		auto menu = CCMenu::create();
		menu->setID("icon-gallery-menu");
		menu->setZOrder(2);
		this->addChildAtPosition(menu, Anchor::BottomLeft, ccp(32, 32), false);

		auto spr = CrossButtonSprite::createWithSprite("GalleryIcon.png"_spr, 0.9f);
		spr->setScale(0.75f);

		auto button = CCMenuItemSpriteExtra::create(
			spr,
			this,
			menu_selector(GarageLayer::onIconGallery));
		button->setID("Icon-Gallery-Button");

		menu->addChild(button);
		menu->updateLayout();

		return true;
	}

	void onIconGallery(CCObject *)
	{
		auto scene = CCScene::create();
		scene->addChild(GalleryLayer::create());

		CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
	}
};

$execute
{
	listenForSettingChanges<std::string>("sort-order", [](std::string value){ 
		if(auto layer = static_cast<GalleryLayer *>(CCScene::get()->getChildByID("icon-gallery-layer"))){
			layer->refreshGallery();
		} });
};