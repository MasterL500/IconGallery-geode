#include "GalleryHelp.hpp"

//	Alright, here's the code then
bool GalleryHelpPopup::init()
{
	if (!Popup::init(410.f, 240.f, "GJ_square02.png"))
		return false;

	//	Background
	auto background = NineSlice::create("square02b_001.png", {0, 0, 80, 80});
	background->setContentSize({200, 340});
	background->setAnchorPoint({0, 0.5});
	background->setColor({0, 0, 0});
	background->setOpacity(50);
	background->setScale(0.5);
	background->setID("background");
	m_mainLayer->addChildAtPosition(background, Anchor::Left, ccp(10, 5), false);

	//	Title label (I think there's already one in here)
	auto label = CCLabelBMFont::create("Icon Gallery - Help", "goldFont.fnt");
	label->setAnchorPoint({0.5, 0});
	label->setScale(0.75);
	label->setID("title");
	m_mainLayer->addChildAtPosition(label, Anchor::Top, ccp(0, -25), false);

	//	The menu for the tab selection
	m_selectMenu = CCMenu::create();
	m_selectMenu->setContentSize({100, 170});
	m_selectMenu->setAnchorPoint({0, 0.5});
	m_selectMenu->setLayout(ColumnLayout::create()
								->setCrossAxisLineAlignment(AxisAlignment::Start)
								->setCrossAxisOverflow(false)
								->setAutoScale(false)
								->setAxisReverse(true)
								->setGap(12));
	m_mainLayer->addChildAtPosition(m_selectMenu, Anchor::Left, ccp(10, 5), false);

	//	The Categories
	std::vector<gd::string> text = {
		"About", "How to Use", "Downloading", "Uploading", "FAQ", "Support"};

	//	Creates the button for the categories
	for (int ii = 0; ii < text.size(); ii++)
	{
		auto buttonSpr = CCLabelBMFont::create(text[ii].c_str(), "bigFont.fnt", 100);
		buttonSpr->limitLabelWidth(100, 0.4, 0.3);
		buttonSpr->setAnchorPoint({0, 0.5});
		buttonSpr->setID("label");

		if (ii == 0)
			buttonSpr->setColor({255, 255, 0});

		auto button = CCMenuItemSpriteExtra::create(
			buttonSpr,
			this,
			menu_selector(GalleryHelpPopup::onPage));

		button->m_scaleMultiplier = 1.1f;
		button->setTag(ii);

		m_selectMenu->addChild(button);
		m_selectMenu->updateLayout();
	}

	//	Text area (for the information)
	m_textArea = MDTextArea::create("[Insert Text Here]", {280, 200});
	m_textArea->setAnchorPoint({0.5, 0.5});
	m_textArea->setID("text-area");
	m_mainLayer->addChildAtPosition(m_textArea, Anchor::Center, ccp(52.5, -10), false);

	//	Language Button Sprites
	auto offSpr = CCLabelBMFont::create("Language: ENG", "goldFont.fnt");
	offSpr->setScale(0.5f);
	auto onSpr = CCLabelBMFont::create("Language: ESP", "goldFont.fnt");
	onSpr->setScale(0.5f);

	//	Language Button
	auto m_languageButton = CCMenuItemToggler::create(
		offSpr,
		onSpr,
		this,
		menu_selector(GalleryHelpPopup::onLanguage));
	m_languageButton->m_offButton->m_scaleMultiplier = 1.1f;
	m_languageButton->m_onButton->m_scaleMultiplier = 1.1f;
	m_languageButton->setID("language-button");
	m_languageButton->toggle(m_spanish);
	m_buttonMenu->addChildAtPosition(m_languageButton, Anchor::BottomLeft, ccp(60, 24), false);

	//	Calls the function to set the info
	setInfo();

	this->setID("gallery-help-popup");
	return true;
}

void GalleryHelpPopup::onLanguage(CCObject *sender)
{
	auto button = static_cast<CCMenuItemToggler *>(sender);
	button->toggle(m_spanish);
	m_spanish = !m_spanish;

	//	Reloads the info
	setInfo();
}

void GalleryHelpPopup::onPage(CCObject *sender)
{
	auto tag = sender->getTag();

	if (m_page == tag)
		return;

	auto prevPage = m_page;
	m_page = tag;

	//	The old tab label gets colored back
	if (auto oldBtn = static_cast<CCMenuItemSpriteExtra *>(m_selectMenu->getChildByTag(prevPage)))
	{
		auto text = static_cast<CCLabelBMFont *>(oldBtn->getChildByID("label"));
		text->setColor({255, 255, 255});
	}

	//	The new tab label gets the color
	if (auto btn = static_cast<CCMenuItemSpriteExtra *>(m_selectMenu->getChildByTag(m_page)))
	{
		auto text = static_cast<CCLabelBMFont *>(btn->getChildByID("label"));
		text->setColor({255, 255, 0});
	}

	setInfo();
}

void GalleryHelpPopup::setInfo()
{
	if (!m_textArea)
		return;

	gd::string info;

	switch (m_page)
	{
	case 0:
		//	ABOUT
		info = m_spanish ? aboutSPA : aboutENG;
		break;

	case 1:
		//	HOW TO USE
		info = m_spanish ? tutorialSPA : tutorialENG;
		break;

	case 2:
		//	DOWNLOADING
		info = m_spanish ? downloadSPA : downloadENG;
		break;

	case 3:
		//	UPLOADING
		info = m_spanish ? uploadSPA : uploadENG;
		break;

	case 4:
		//	FAQ
		info = m_spanish ? faqSPA : faqENG;
		break;

	case 5:
		//	SUPPORT
		info = m_spanish ? supportSPA : supportENG;
		break;
	}

	m_textArea->setString(info.c_str());
};

GalleryHelpPopup *GalleryHelpPopup::create()
{
	auto ret = new GalleryHelpPopup;

	if (ret->init())
	{
		ret->autorelease();
		return ret;
	};

	delete ret;
	return nullptr;
}