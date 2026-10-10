#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/Mod.hpp>
using namespace geode::prelude;

class GalleryLayer : public CCLayer, public SetIDPopupDelegate, public SetTextPopupDelegate
{
protected:
	CCLabelBMFont *m_pageLabel = nullptr;
	CCLabelBMFont *m_errorLabel = nullptr;

	CCMenu *m_pagesMenu = nullptr;
	CCMenu *m_modesMenu = nullptr;

	CCMenuItemSpriteExtra *m_prevBtn = nullptr;
	CCMenuItemSpriteExtra *m_nextBtn = nullptr;
	CCMenuItemSpriteExtra *m_findBtn = nullptr;
	CCMenuItemSpriteExtra *m_pagesBtn = nullptr;
	CCMenuItemSpriteExtra *m_authorBtn = nullptr;

	LoadingCircleSprite *m_loading = nullptr;
	ScrollLayer *m_scrollLayer = nullptr;

	virtual bool init() override;
	virtual void keyBackClicked() override;

	//	Creating Sprites
	CCSprite *getModeSprite(int tag, bool active);
	void createModeButton(int tag, bool active = 0);
	void setIDPopupClosed(SetIDPopup *popup, int value) override;
	void setTextPopupClosed(SetTextPopup *popup, gd::string text) override;

	//	Web Requesting
	TaskHolder<web::WebResponse> m_listener;
	void fetchGallery();
	void loadGallery();
	matjson::Value m_fetchedData;

	//	Get the URL
	TaskHolder<web::WebResponse> m_fetchListener;
	void fetchURL();

public:
	IconType m_mode = IconType::Item;
	gd::string m_searchFilter = "";
	gd::string m_authorFilter = "";

	unsigned int m_page = 0;
	unsigned int m_maxPage = 999;
	unsigned int m_activeBtn = 0;
	bool m_isFilterActive = false;

	void onBack(CCObject *sender);
	void onFolder(CCObject *sender);
	void onSettings(CCObject *sender);
	void onReset(CCObject *sender);
	void onHelp(CCObject *sender);

	void onNavButton(CCObject *sender);
	void onPage(CCObject *sender);
	void onFind(CCObject *sender);

	void onWebsite(CCObject *sender);
	void onDiscord(CCObject *sender);

	void refreshGallery();
	void setupIconPack();
	void errorPopup();

	static GalleryLayer *create();
	CCScene *scene();
};