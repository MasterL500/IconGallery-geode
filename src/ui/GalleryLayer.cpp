#include <algorithm>
#include "../nodes/Icon.hpp"
#include "GalleryLayer.hpp"
#include "IconCell.hpp"

const int CELL_HEIGHT = 73;

bool GalleryLayer::init()
{
	if (!CCLayer::init())
		return false;

	auto winSize = CCDirector::sharedDirector()->getWinSize();

	// Background
	auto background = CCSprite::create("background.png"_spr);
	background->setScaleY(winSize.height / background->getContentSize().height);
	background->setScaleX(winSize.width / background->getContentSize().width);
	background->setPosition(winSize / 2);
	addChild(background, -2);

	//	Frame
	auto frame = NineSlice::create("Frame.png"_spr);
	frame->setContentSize({400, 255});
	addChildAtPosition(frame, Anchor::Center, ccp(0, 0), false);

	//	Title
	auto title = CCSprite::create("GalleryLabel.png"_spr);
	addChildAtPosition(title, Anchor::Top, ccp(0, -30), false);

	//	Page Numbering
	m_pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
	m_pageLabel->limitLabelWidth(200.0f, 0.5f, 0.5f);
	m_pageLabel->setAnchorPoint({1, 1});
	m_pageLabel->setVisible(false);
	addChildAtPosition(m_pageLabel, Anchor::TopRight, ccp(-10, -10), false);

	//  Back Button
	auto backMenu = CCMenu::create();
	backMenu->setID("back-menu");
	addChildAtPosition(backMenu, Anchor::TopLeft, ccp(24, -24), false);

	auto backBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
		this,
		menu_selector(GalleryLayer::onBack));
	backBtn->setID("back-button");
	backBtn->setSizeMult(1.2f);
	backMenu->addChild(backBtn);

	//	Pages Menu
	m_pagesMenu = CCMenu::create();
	m_pagesMenu->setID("pages-menu");
	addChildAtPosition(m_pagesMenu, Anchor::BottomLeft, ccp(0, 0), false);

	//  Page Navigation Buttons
	m_prevBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
		this,
		menu_selector(GalleryLayer::onPage));
	m_prevBtn->setID("prev-page-button");
	m_prevBtn->setVisible(false);
	m_prevBtn->setTag(-1);

	auto nextSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
	nextSpr->setFlipX(true);
	m_nextBtn = CCMenuItemSpriteExtra::create(
		nextSpr,
		this,
		menu_selector(GalleryLayer::onPage));
	m_nextBtn->setID("next-page-button");
	m_nextBtn->setVisible(false);
	m_nextBtn->setTag(1);

	m_pagesMenu->addChildAtPosition(m_prevBtn, Anchor::Center, ccp(-216, 0), false);
	m_pagesMenu->addChildAtPosition(m_nextBtn, Anchor::Center, ccp(216, 0), false);

	//	Gamemodes Menu
	m_modesMenu = CCMenu::create();
	m_modesMenu->setID("gamemodes-menu");
	m_modesMenu->setLayout(RowLayout::create()->setGap(2.5f));
	addChildAtPosition(m_modesMenu, Anchor::Bottom, ccp(0, 30), false);

	//	For the Gamemodes
	for (int ii = 0; ii < 10; ii++)
		createModeButton(ii, ii == 0);

	//	Buttons Menu
	auto buttonMenu = CCMenu::create();
	buttonMenu->setID("button-menu");
	addChildAtPosition(buttonMenu, Anchor::BottomLeft, ccp(0, 0), false);

	m_pagesBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create(fmt::format("{}", m_page + 1).c_str(), 20, 20, 0.8f, true, "bigFont.fnt", "GJ_button_01.png"),
		this,
		menu_selector(GalleryLayer::onFind));
	m_pagesBtn->setTag(0);
	m_pagesBtn->setID("pages-button");
	buttonMenu->addChildAtPosition(m_pagesBtn, Anchor::TopRight, ccp(-25, -50), false);

	m_findBtn = CCMenuItemSpriteExtra::create(
		EditorButtonSprite::createWithSprite("Search.png"_spr, 1.2f),
		this,
		menu_selector(GalleryLayer::onFind));
	m_findBtn->setTag(1);
	m_findBtn->setID("search-button");
	buttonMenu->addChildAtPosition(m_findBtn, Anchor::TopLeft, ccp(25, -70), false);

	m_authorBtn = CCMenuItemSpriteExtra::create(
		EditorButtonSprite::createWithSprite("SearchAuthor.png"_spr, 1.2f),
		this,
		menu_selector(GalleryLayer::onFind));
	m_authorBtn->setTag(2);
	m_authorBtn->setID("author-button");
	buttonMenu->addChildAtPosition(m_authorBtn, Anchor::TopLeft, ccp(25, -110), false);

	//	Settings
	auto settingsSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
	settingsSpr->setScale(0.85f);

	auto settingsBtn = CCMenuItemSpriteExtra::create(
		settingsSpr,
		this,
		menu_selector(GalleryLayer::onSettings));
	settingsBtn->setID("settings-button");
	buttonMenu->addChildAtPosition(settingsBtn, Anchor::BottomLeft, ccp(30, 30), false);

	auto folderBtn = CCMenuItemSpriteExtra::create(
		CircleButtonSprite::createWithSpriteFrameName("gj_folderBtn_001.png", 1, CircleBaseColor::Green, CircleBaseSize::SmallAlt),
		this,
		menu_selector(GalleryLayer::onFolder));
	folderBtn->setID("folder-button");
	buttonMenu->addChildAtPosition(folderBtn, Anchor::BottomLeft, ccp(30, 75), false);

	//	Socials
	auto discordBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png"),
		this,
		menu_selector(GalleryLayer::onDiscord));
	discordBtn->setID("discord-button");
	buttonMenu->addChildAtPosition(discordBtn, Anchor::BottomRight, ccp(-25, 60), false);

	auto websiteBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("WebsiteIcon.png"_spr),
		this,
		menu_selector(GalleryLayer::onWebsite));
	websiteBtn->setID("website-button");
	buttonMenu->addChildAtPosition(websiteBtn, Anchor::BottomRight, ccp(-25, 25), false);

	//	Scroll Layer
	m_scrollLayer = ScrollLayer::create({357, 220});
	m_scrollLayer->setID("scroll-layer");
	m_scrollLayer->setZOrder(-2);
	this->addChildAtPosition(m_scrollLayer, Anchor::Center, ccp(-178, -110), false);

	//	Loading Circle
	m_loading = LoadingCircleSprite::create(1);
	m_loading->setID("loading");
	m_loading->setVisible(false);
	m_loading->setScale(0.6f);
	this->addChildAtPosition(m_loading, Anchor::Center, ccp(0, 0), false);

	//	If there's no "Icon Pack" settled (via settings), creates one.
	auto noPackExists = Mod::get()->getSettingValue<std::filesystem::path>("icon-pack-folder").empty();
	if (noPackExists)
	{
		this->runAction(CCSequence::create(
			CCDelayTime::create(1.f),
			CCCallFunc::create(this, callfunc_selector(GalleryLayer::setupIconPack)),
			0));
	};

	//	Calls teh function to fetch the Gallery.
	fetchURL();

	setKeyboardEnabled(true);
	setKeypadEnabled(true);

	this->setID("icon-gallery-layer");
	return true;
};

void GalleryLayer::setupIconPack()
{
	if (Mod::get()->getSettingValue<bool>("more-icons-folder"))
		return;

	auto popup = createQuickPopup(
		"No Icon Pack found",
		"Icons downloaded from here are saved in a Texture Pack. Do you want to make one? (Note: This is to avoid overwritting existing icons)",
		"No",
		"Yes",
		[this](bool no, bool yes)
		{
			//	If yes, creates the directory
			if (yes)
			{
				auto directory = Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir() / "packs";

				if (std::filesystem::create_directories(directory / "Icon Gallery"))
				{
					auto json = matjson::makeObject({{"textureldr", "1.5.0"},
													 {"name", "Downloaded Icons"},
													 {"id", "icon_gallery.pack"},
													 {"version", "1.0.0"},
													 {"author", "Icon Gallery mod"}});

					auto packPath = directory / "Icon Gallery";

					if (!utils::file::writeString(packPath / "pack.json", json.dump()))
					{
						Notification::create("Error while creating the pack.json", NotificationIcon::Error)->show();
						log::error("There was an error creating the Pack.json");
					}
					else
					{
						Mod::get()->setSettingValue<std::filesystem::path>("icon-pack-folder", packPath);
						Notification::create("Icon Pack succesfully created!", NotificationIcon::Success)->show();
						log::debug("Pack.json succesfully written!");
					}
				}
				else
				{
					Notification::create("Error while creating Pack Folder", NotificationIcon::Error)->show();
					log::error("There was an error attempting to create the directory.");
				}
			}
			else if (no)
			{
				auto warning = createQuickPopup(
					"Set Folder",
					"Please set a Texture Pack folder in the settings of the mod to download icons",
					"Ok",
					nullptr,
					[](auto, auto) {});
			}
		});
}

void GalleryLayer::fetchURL()
{
	auto req = web::WebRequest();

	m_fetchListener.spawn(
		req.get("https://iconsgallery.pages.dev/assets/API_BASE.txt"),
		[this](web::WebResponse res)
		{
			if (res.ok())
			{
				auto prevURL = Mod::get()->getSavedValue<std::string>("API");
				auto newURL = utils::string::replace(res.string().unwrap(), "\n", "");

				if ((std::string_view(prevURL) != std::string_view(newURL)) || prevURL.empty())
					Mod::get()->setSavedValue<std::string>("API", newURL);

				fetchGallery();
			}
			else
			{
				Notification::create("Error while fetching API", NotificationIcon::Error)->show();
				log::error("There was an error fetching the URL from website");
			}
		});
}

void GalleryLayer::fetchGallery()
{
	if (m_scrollLayer && m_scrollLayer->m_contentLayer->getChildrenCount() > 0)
		m_scrollLayer->m_contentLayer->removeAllChildren();

	if (m_pageLabel)
		m_pageLabel->setVisible(false);

	if (m_loading)
		m_loading->setVisible(true);

	//	Main URL
	std::string url = fmt::format("{}/api/index", Mod::get()->getSavedValue<std::string>("API"));

	//	Sorting
	auto order = Mod::get()->getSettingValue<std::string>("sort-order");
	if (std::string_view(order) == std::string_view("Recent"))
		url = fmt::format("{}?order=Recent", url);
	else
		url = fmt::format("{}?order=Downloads", url);

	//	Page
	url = fmt::format("{}&page={}", url, m_page + 1);

	//	Gamemode
	if (m_mode != IconType::Item)
		url = fmt::format("{}&mode={}", url, (int)m_mode);

	//	Author
	if (!m_authorFilter.empty())
		url = fmt::format("{}&artist={}", url, m_authorFilter);

	//	Query
	if (!m_searchFilter.empty())
		url = fmt::format("{}&query={}", url, m_searchFilter);

	log::debug("URL = {}", url);

	//	Makes the request
	auto req = web::WebRequest();

	m_listener.spawn(
		req.get(url),
		[this](web::WebResponse res)
		{
			if (res.ok() && res.json().isOk())
			{
				m_fetchedData = res.json().unwrap();
				loadGallery();
			}
			else
			{
				if (m_errorLabel)
					m_errorLabel->removeMeAndCleanup();

				m_errorLabel = CCLabelBMFont::create(fmt::format("Something went wrong (Error {})", res.code()).c_str(), "goldFont.fnt");
				this->addChildAtPosition(m_errorLabel, Anchor::Center, ccp(0, 0), false);
				m_errorLabel->setID("error-text");
				m_errorLabel->setScale(0.6f);

				m_loading->setVisible(false);
				log::error("Error {}: Failed on fetching gallery data... {}", res.code(), res.errorMessage());
			}
		});
};

void GalleryLayer::loadGallery()
{
	if (m_loading)
		m_loading->setVisible(false);

	m_maxPage = m_fetchedData["totalPages"].asInt().unwrapOr(999) - 1;
	auto totalIcons = m_fetchedData["totalIcons"].asInt().unwrapOr(1);
	auto offset = (m_page * 10);

	if (m_pageLabel)
	{
		m_pageLabel->setCString(fmt::format("{} to {} of {}", offset + 1, offset + 10, totalIcons).c_str());
		m_pageLabel->setVisible(true);
	}

	auto fetchedIcons = m_fetchedData["icons"];

	std::vector<Icon *> icons = {};
	int ii = 0;

	for (auto &value : fetchedIcons)
	{
		auto iconData = value;

		Icon *newIcon = Icon::create(
			iconData["iconName"].asString().unwrap(),
			iconData["author"].asString().unwrap(),
			iconData["filename"].asString().unwrap(),
			iconData["previewUrl"].asString().unwrap(),
			iconData["gamemode"].asInt().unwrap(),
			iconData["downloads"].asInt().unwrapOr(0),
			iconData["description"].asString().unwrapOr(""),
			iconData["format"].asString().unwrapOr(""));

		//	If there's data of collaborators
		auto collab = iconData["collaborators"].as<std::vector<std::string>>().unwrap();
		if (!collab.empty())
		{
			newIcon->addCollab(collab);
		}

		icons.push_back(newIcon);

		IconCell *cell = IconCell::create(newIcon, ii % 2 == 0);
		m_scrollLayer->m_contentLayer->addChild(cell);
		cell->setPosition(0, (CELL_HEIGHT * fetchedIcons.size()) - CELL_HEIGHT * (ii + 1));
		ii++;
	}

	//	Fixes the scroll layer
	int iconCount = m_scrollLayer->m_contentLayer->getChildrenCount();
	m_scrollLayer->m_contentLayer->setContentSize(ccp(m_scrollLayer->m_contentLayer->getContentSize().width, (CELL_HEIGHT * fetchedIcons.size())));
	m_scrollLayer->moveToTop();

	m_prevBtn->setVisible(m_page > 0);
	m_nextBtn->setVisible(m_page < m_maxPage);
}

void GalleryLayer::refreshGallery()
{
	m_page = 0;
	fetchGallery();
}

void GalleryLayer::createModeButton(int tag, bool active)
{
	std::vector<const char *> modeNames = {
		"streak",
		"icon",
		"ship",
		"ball",
		"bird",
		"dart",
		"robot",
		"spider",
		"swing",
		"jetpack"};

	//	The sprites
	auto inactiveSpr = tag == 0 ? CCSprite::create("AllModesOff.png"_spr) : CCSprite::createWithSpriteFrameName(fmt::format("gj_{}Btn_off_001.png", modeNames[tag]).c_str());
	auto activeSpr = tag == 0 ? CCSprite::create("AllModesOn.png"_spr) : CCSprite::createWithSpriteFrameName(fmt::format("gj_{}Btn_on_001.png", modeNames[tag]).c_str());
	inactiveSpr->setScale(0.9f);
	activeSpr->setScale(0.9f);

	//	The button
	auto *m_button = CCMenuItemToggler::create(
		inactiveSpr,
		activeSpr,
		this,
		menu_selector(GalleryLayer::onNavButton));

	m_button->setID(fmt::format("gamemode-button-{:02}", tag + 1));
	m_button->toggle(active);
	m_button->setTag(tag);

	//  Adds button to menu and updates layout.
	m_modesMenu->addChild(m_button);
	m_modesMenu->updateLayout();
};

void GalleryLayer::onNavButton(CCObject *sender)
{
	auto tag = sender->getTag();
	auto m_prevModeBtn = m_activeBtn;
	m_activeBtn = tag;

	if (m_activeBtn == m_prevModeBtn)
		return;

	if (auto oldButton = static_cast<CCMenuItemToggler *>(m_modesMenu->getChildByTag(m_prevModeBtn)))
	{
		oldButton->toggle(false);
	}

	m_isFilterActive = tag != 0;
	m_mode = tag != 0 ? IconType{tag - 1} : IconType::Item;
	m_page = 0;

	//	Arrow Buttons
	m_prevBtn->setVisible(false);
	m_nextBtn->setVisible(false);

	fetchGallery();
}

void GalleryLayer::onPage(CCObject *sender)
{
	//	Arrow Buttons
	m_prevBtn->setVisible(false);
	m_nextBtn->setVisible(false);

	m_page += sender->getTag();
	fetchGallery();
}

void GalleryLayer::onFind(CCObject *sender)
{
	auto tag = sender->getTag();

	if (tag == 0)
	{
		log::debug("Page = {} - Max Page = {}", m_page, m_maxPage);

		auto popup = SetIDPopup::create(m_page + 1, 1, m_maxPage + 1, "Go to page", "Go", true, 1, 0, false, true);
		popup->m_delegate = this;
		popup->setTag(3);
		popup->show();
	}
	else if (tag == 1)
	{
		auto popup = SetTextPopup::create(m_searchFilter, "Enter a Name", 100, "Search Icon", "Go", true, 0);
		popup->m_delegate = this;
		popup->setTag(0);
		popup->show();
	}
	else
	{
		auto popup = SetTextPopup::create(m_authorFilter, "Enter an User", 100, "Search by Author", "Go", true, 0);
		popup->m_delegate = this;
		popup->setTag(1);
		popup->show();
	}
};

void GalleryLayer::setIDPopupClosed(SetIDPopup *popup, int value)
{
	if (!popup || popup->m_cancelled)
		return;

	log::debug("Changed Page = {}", value);

	//	Arrow Buttons
	m_prevBtn->setVisible(false);
	m_nextBtn->setVisible(false);

	//	Page changed
	int newPage = value;
	if (m_page == newPage - 1)
		return;

	m_page = newPage - 1;

	//	Changes the sprite of the button
	if (m_pagesBtn)
	{
		m_pagesBtn->setSprite(
			ButtonSprite::create(fmt::format("{}", m_page + 1).c_str(), 20, 20, 0.8f, true, "bigFont.fnt", "GJ_button_01.png"));
	}

	fetchGallery();
};

void GalleryLayer::setTextPopupClosed(SetTextPopup *popup, gd::string text)
{
	if (!popup || popup->m_cancelled)
		return;

	log::debug("Input = {} - Tag = {}", text, popup->getTag());

	if (popup->getTag() == 0)
	{
		if (std::string_view(text) == std::string_view(m_searchFilter))
			return;

		m_searchFilter = text;

		log::debug("Search filter updated");
	}
	else
	{
		if (std::string_view(text) == std::string_view(m_authorFilter))
			return;

		m_authorFilter = text;

		log::debug("Author filter updated");
	}

	if (m_findBtn)
	{
		auto spriteName = m_searchFilter.empty() ? "geode.loader/baseEditor_Normal_Green.png" : "geode.loader/baseEditor_Normal_Cyan.png";
		static_cast<CCSprite *>(m_findBtn->getNormalImage())->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(spriteName));
		m_findBtn->updateSprite();
	}

	if (m_authorBtn)
	{
		auto spriteName = m_authorFilter.empty() ? "geode.loader/baseEditor_Normal_Green.png" : "geode.loader/baseEditor_Normal_Cyan.png";
		static_cast<CCSprite *>(m_authorBtn->getNormalImage())->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(spriteName));
		m_authorBtn->updateSprite();
	}

	m_page = 0;
	fetchGallery();
}

void GalleryLayer::onSettings(CCObject *)
{
	geode::openSettingsPopup(Mod::get());
}

void GalleryLayer::onFolder(CCObject *)
{
	//	utils::file::openFolder(Mod::get()->getConfigDir());

	if (!Mod::get()->getSettingValue<std::filesystem::path>("icon-pack-folder").empty())
	{
		utils::file::openFolder(Mod::get()->getSettingValue<std::filesystem::path>("icon-pack-folder"));
	}
	else if (Mod::get()->getSettingValue<bool>("more-icons-folder"))
	{
		utils::file::openFolder(Loader::get()->getInstalledMod("hiimjustin000.more_icons")->getConfigDir());
	}
}

void GalleryLayer::onDiscord(CCObject *)
{
	CCApplication::sharedApplication()->openURL("https://discord.gg/dceY3uvGzD");
}

void GalleryLayer::onWebsite(CCObject *)
{
	CCApplication::sharedApplication()->openURL("https://iconsgallery.pages.dev/");
}

void GalleryLayer::onBack(CCObject *)
{
	keyBackClicked();
};

void GalleryLayer::keyBackClicked()
{
	CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
};

GalleryLayer *GalleryLayer::create()
{
	auto ret = new GalleryLayer();

	if (ret->init())
	{
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
};

CCScene *GalleryLayer::scene()
{
	auto layer = GalleryLayer::create();
	auto scene = CCScene::create();
	scene->addChild(layer);
	return scene;
};