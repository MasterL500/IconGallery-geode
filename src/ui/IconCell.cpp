#include "IconCell.hpp"

// someone doesn't wanna clutter init sorry
float getDescThreshold()
{
	auto m_measureLine = CCLabelBMFont::create("A", "chatFont.fnt");
	m_measureLine->setScale(0.5f);
	float singleLineHeight = m_measureLine->getContentSize().height;

	return singleLineHeight * 2.f;
}

std::string removeTildes(std::string original)
{
	std::string ret = original;

	// god i wish there was an easier way to do this (idk if there is actually)
	utils::string::replaceIP(ret, "á", "a");
	utils::string::replaceIP(ret, "é", "e");
	utils::string::replaceIP(ret, "í", "i");
	utils::string::replaceIP(ret, "ó", "o");
	utils::string::replaceIP(ret, "ú", "u");

	utils::string::replaceIP(ret, "Á", "A");
	utils::string::replaceIP(ret, "É", "E");
	utils::string::replaceIP(ret, "Í", "I");
	utils::string::replaceIP(ret, "Ó", "O");
	utils::string::replaceIP(ret, "Ú", "U");

	return ret;
}

// stolen from deltadash by asterveila
// - sarah (asterveila)
ccColor3B pastelizeColor(const ccColor3B &color, float factor = 0.4f)
{
	factor = std::max(0.0f, std::min(1.0f, factor));

	GLubyte r = static_cast<GLubyte>(color.r + (255 - color.r) * factor);
	GLubyte g = static_cast<GLubyte>(color.g + (255 - color.g) * factor);
	GLubyte b = static_cast<GLubyte>(color.b + (255 - color.b) * factor);

	ccColor3B pastelized = ccColor3B({r, g, b});

	return pastelized;
}

bool IconCell::init(Icon *icon, bool even)
{
	if (!CCLayer::init())
		return false;

	m_icon = icon;
	m_icon->m_cell = this;
	this->setContentSize(m_size);
	float scale = CCDirector::sharedDirector()->getContentScaleFactor() / 4;
	float lowerMarginMult = 3.f;

	//	Background of the Cell
	auto m_background = CCLayerColor::create();
	m_background->setContentSize(m_size);
	m_background->setOpacity(255);
	m_background->setZOrder(-2);
	addChildAtPosition(m_background, Anchor::BottomLeft, ccp(0, 0), false);

	//	Set Color of Background
	if (even)
		m_background->setColor({194, 114, 62});
	else
		m_background->setColor({161, 88, 44});

	//	Preview the Icon
	m_preview = LazySprite::create({160, 160});
	m_preview->setScale(0.6f);
	m_preview->setLoadCallback(
		[this, icon, scale](Result<> result)
		{
			if (!result.isOk())
			{
				if (m_previewAttempts < 3)
				{
					log::info("failed to load preview, please refresh Icon Gallery to try again");
					m_preview->initWithFile("Placeholder.png"_spr);
				}
				else
				{
					m_preview->loadFromUrl(m_icon->m_previewURL, geode::LazySprite::Format::kFmtPng);
					m_previewAttempts += 1;
				}
			}
			else
			{
				m_preview->setScale(scale + getExtraScale(icon->m_gamemode) * CCDirector::sharedDirector()->getContentScaleFactor());
			}
		});
	m_preview->loadFromUrl(m_icon->m_previewURL, geode::LazySprite::Format::kFmtPng);
	this->addChildAtPosition(m_preview, Anchor::Left, ccp(35, 0), false);

	//	Name of the Icon
	auto m_iconName = CCLabelBMFont::create(m_icon->m_name.c_str(), "bigFont.fnt");
	m_iconName->limitLabelWidth(220.0f, 0.5f, 0.25f);
	m_iconName->setAnchorPoint({0, 1.f});
	this->addChildAtPosition(m_iconName, Anchor::Left, ccp(70, m_size.height / 2.f - m_margin / 3.f), false);

	std::string author = fmt::format("By {}", m_icon->m_author);
	//	log::debug("Is collab? = {}", !m_icon->m_collaborators.empty());

	if (!m_icon->m_collaborators.empty())
	{
		if (m_icon->m_collaborators.size() > 1)
		{
			author = fmt::format("By {} and More", m_icon->m_author);
		}
		else
		{
			author = fmt::format("By {} and {}", m_icon->m_author, m_icon->m_collaborators[0]);
		}
	};

	//	Author of the Icon
	auto m_iconAuthor = CCLabelBMFont::create(author.c_str(), "goldFont.fnt");
	m_iconAuthor->setScale(0.45f);
	m_iconAuthor->setAnchorPoint({0, 1});
	this->addChildAtPosition(m_iconAuthor, Anchor::Left, ccp(70, m_size.height / 2.f - m_margin * 5.65f), false);

	//	Download Count
	auto m_downloadIcon = CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
	m_downloadIcon->setScale(0.4f);
	m_downloadIcon->setAnchorPoint({0, 0.5f});
	// m_downloadIcon->setPosition(ccp(m_downloadCount->getPosition().x - .3f, 11));
	this->addChildAtPosition(m_downloadIcon, Anchor::Left, ccp(70, -m_size.height / 2.f + m_margin * lowerMarginMult), false);

	auto m_downloadCount = CCLabelBMFont::create(fmt::format("{}", m_icon->m_downloads).c_str(), "bigFont.fnt");
	m_downloadCount->setScale(0.3f);
	m_downloadCount->setAnchorPoint({0, 0.5f});
	// m_downloadCount->setColor({100, 255, 100});
	this->addChildAtPosition(m_downloadCount, Anchor::Left, ccp(83.5, -m_size.height / 2.f + m_margin * lowerMarginMult + 1), false);

	// Description
	std::string m_noBsDesc = removeTildes(utils::string::replace(m_icon->m_description, "\n", " "));
	if (m_noBsDesc.empty())
		m_noBsDesc = "[No Description Provided]";

	auto m_descText = CCLabelBMFont::create(m_noBsDesc.c_str(), "chatFont.fnt");
	m_descText->setAnchorPoint({0, 0.5f});
	m_descText->setScale(0.5f);

	float descWidth = 210.f;
	float descScale = 0.45f;

	m_descText->setWidth(descWidth);
	m_descText->limitLabelWidth(descWidth, descScale, 0.2f);

	if (m_descText->getContentSize().height > getDescThreshold())
	{
		descWidth += 5.f;
		descScale = 0.42f;

		m_descText->setScale(descScale);
		m_descText->setWidth(descWidth);
		m_descText->limitLabelWidth(descWidth, descScale, 0.2f);
	}

	this->addChildAtPosition(m_descText, Anchor::Left, {75, -7}, false);

	//	Description Background
	auto m_descBg = NineSlice::create("BlackSquare.png"_spr);
	m_descBg->setContentSize({220.f, 24.f});
	m_descBg->setAnchorPoint({0, 0.5f});
	m_descBg->setOpacity(60);
	m_descBg->setZOrder(-1);
	this->addChildAtPosition(m_descBg, Anchor::Left, {70, -7}, false);

	//	Gamemode & Format Labels
	auto m_tagsContainer = getTagContainer();
	m_tagsContainer->setScale(0.25f);
	this->addChildAtPosition(m_tagsContainer, Anchor::Left, ccp(m_downloadCount->getPositionX() + m_downloadCount->getScaledContentWidth() + 5, -m_size.height / 2.f + m_margin * lowerMarginMult), false);

	//	Main Menu
	auto m_menu = CCMenu::create();
	m_menu->setContentSize(m_size);
	this->addChildAtPosition(m_menu, Anchor::BottomLeft, ccp(0, 0), false);

	/*	Info Button
	auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png");
	infoSpr->setScale(0.775f);

	auto m_infoBtn = CCMenuItemSpriteExtra::create(
		infoSpr,
		this,
		menu_selector(IconCell::onInfo));
	m_menu->addChildAtPosition(m_infoBtn, Anchor::Right, ccp(-70, 0), false);
	*/

	//	Download Button
	auto downloadSpr = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
	m_downloadBtn = CCMenuItemSpriteExtra::create(
		downloadSpr,
		this,
		menu_selector(IconCell::onDownload));
	m_downloadBtn->setScale(0.8f);
	m_menu->addChildAtPosition(m_downloadBtn, Anchor::Right, ccp(-25, 0), false);

	//	SLIDER
	m_icon->m_downloadBar = Slider::create(this, nullptr);
	m_icon->m_downloadBar->setContentSize(m_size);
	this->addChild(m_icon->m_downloadBar);

	m_icon->m_downloadBar->m_groove->setScale(0.75f);
	m_icon->m_downloadBar->m_groove->setAnchorPoint({1, 0.5});
	m_icon->m_downloadBar->m_touchLogic->setVisible(false);
	m_icon->m_downloadBar->m_groove->setPosition({m_size.width - 5.f, 7.5});

	updateStatus();
	return true;
}

float IconCell::getExtraScale(IconType type)
{
	float ret = 0.f;

	switch (type)
	{
	case IconType::Cube:
		ret = 0.35f;
		break;
	case IconType::Ball:
		ret = 0.3f;
		break;
	case IconType::Wave:
		ret = 0.35f;
		break;
	case IconType::Swing:
		ret = 0.25f;
		break;
	default:
		ret = 0.2f;
		break;
	}

	return ret / 4;
}

CCNode *IconCell::getTagContainer()
{
	CCNode *m_tagContainer = CCNode::create();
	int m_tagMargin = 10;

	// Gamemode Label
	auto m_gamemodeLabel = getGamemodeLabel(m_icon->m_gamemode);
	auto m_gmLabelSize = m_gamemodeLabel->getContentSize();

	// Format Label
	const char *formatText = m_icon->m_format == IconFormat::Vanilla ? "Vanilla" : "More Icons";
	auto m_formatLabel = CCLabelBMFont::create(formatText, "bigFont.fnt");
	m_formatLabel->setColor({150, 150, 255});
	// m_formatLabel->setScale(0.3f);

	if (m_icon->m_format == IconFormat::Vanilla)
		m_formatLabel->setColor({150, 255, 150});

	auto m_formatLabelSize = m_formatLabel->getContentSize();

	// create tag bgs
	auto m_gamemodeBg = NineSlice::create("TagSquare.png"_spr);
	m_gamemodeBg->setContentSize({m_gmLabelSize.width + m_tagMargin * 2.f + m_tagMargin / 2.f, m_gmLabelSize.height + m_tagMargin});
	m_gamemodeBg->setColor(m_gamemodeLabel->getColor());

	auto m_formatBg = NineSlice::create("TagSquare.png"_spr);
	m_formatBg->setContentSize({m_formatLabelSize.width + m_tagMargin * 2.f + m_tagMargin / 2.f, m_formatLabelSize.height + m_tagMargin});
	m_formatBg->setColor(m_formatLabel->getColor());

	// put the labels on them
	m_gamemodeBg->addChildAtPosition(m_gamemodeLabel, Anchor::Center, {0, 2}, false);
	m_gamemodeBg->setZOrder(2);

	m_formatBg->addChildAtPosition(m_formatLabel, Anchor::Center, {0, 2}, false);
	m_formatBg->setZOrder(2);

	// final node touches
	m_tagContainer->addChild(m_gamemodeBg);
	m_tagContainer->addChild(m_formatBg);
	m_tagContainer->setLayout(
		RowLayout::create()
			->setGap(3.0f)
			->setAxisAlignment(AxisAlignment::Start)
			->setAxisReverse(false)
			->setAutoGrowAxis(true));
	m_tagContainer->updateLayout();
	m_tagContainer->setAnchorPoint({0, 0.5f});

	return m_tagContainer;
}

void IconCell::updateStatus()
{
	if (!m_icon)
		return;

	//	log::debug("Update called");

	if (m_icon->isDownloading)
	{
		m_downloadBtn->setVisible(false);
	}

	if (m_icon->m_downloadBar)
	{
		m_icon->m_downloadBar->setVisible(m_icon->isDownloading);
	}

	if (m_icon->isDownloadSuccesful)
	{
		m_downloadBtn->setVisible(false);
		//	log::debug("Succesfull Called");
	}
}

CCLabelBMFont *IconCell::getGamemodeLabel(IconType gamemode)
{
	const char *text = "????";
	ccColor3B color = {120, 120, 120};

	switch (gamemode)
	{
	case IconType::Cube:
		text = "Cube";
		color = {7, 255, 60};
		break;

	case IconType::Ship:
		text = "Ship";
		color = {255, 81, 255};
		break;

	case IconType::Ball:
		text = "Ball";
		color = {255, 77, 31};
		break;

	case IconType::Ufo:
		text = "UFO";
		color = {255, 187, 0};
		break;

	case IconType::Wave:
		text = "Wave";
		color = {117, 235, 255};
		break;

	case IconType::Robot:
		text = "Robot";
		color = {216, 216, 216};
		break;

	case IconType::Spider:
		text = "Spider";
		color = {177, 74, 255};
		break;

	case IconType::Swing:
		text = "Swing";
		color = {255, 255, 80};
		break;

	case IconType::Jetpack:
		text = "Jetpack";
		color = {255, 81, 255};
		break;

	default:
		break;
	}

	auto label = CCLabelBMFont::create(text, "bigFont.fnt");
	// label->setAnchorPoint({0, 0.5});
	label->setColor(color);
	// label->setScale(0.3f);

	return label;
}

void IconCell::onDownload(CCObject *)
{
	auto noPackExists = Mod::get()->getSettingValue<std::filesystem::path>("icon-pack-folder").empty();
	if (noPackExists)
	{
		auto warning = createQuickPopup(
			"Set Folder",
			"Please set a valid <cc>Texture Pack</c> location in the \"Icon pack folder\" settings of the mod to download icons.",
			"Ok",
			nullptr,
			[](auto, auto) {});

		return;
	};

	auto popup = createQuickPopup(
		"Download Icon?",
		"Are you sure you want to download <cy>" + m_icon->m_name + "</c>?",
		"No",
		"Yes",
		[this](auto, bool btn)
		{
			if (btn)
			{
				m_icon->downloadIcon();
				updateStatus();
			}
		});
}

/*
void IconCell::onInfo(CCObject *)
{
	if (!m_icon)
	return;

	std::string data =
	"### Author: " + m_icon->m_author +
	"\n***" +
	"\n\n" + m_icon->m_description;

	MDPopup::create(
		m_icon->m_name,
		data,
		"Ok")
		->show();
	}
*/

IconCell *IconCell::create(Icon *m_icon, bool even)
{
	{
		auto ret = new IconCell();

		if (ret->init(m_icon, even))
		{
			ret->autorelease();
			return ret;
		}

		delete ret;
		return nullptr;
	}
}

/*
	std::string m_noBsDesc = utils::string::replace(m_icon->m_description, "\n", " ");
	if (m_noBsDesc.empty()) m_noBsDesc = "[No Description Provided]";
	auto m_descText = CCLabelBMFont::create(m_noBsDesc.c_str(), "chatFont.fnt");
	m_descText->setScale(0.5f);
	m_descText->setAnchorPoint({0, 1});
	m_descText->setWidth(220.f);
	m_descText->limitLabelWidth(220.f, 0.5f, 0.2f);
	this->addChildAtPosition(m_descText, Anchor::Left, {75, 2}, false);
*/