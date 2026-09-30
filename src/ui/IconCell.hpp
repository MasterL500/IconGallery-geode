#pragma once
#include "../nodes/Icon.hpp"
using namespace geode::prelude;

class IconCell : public CCLayer
{
protected:
	Icon *m_icon;
	CCLayerColor *m_background;
	CCSize m_size = {340.0f, 73.0f};
	double m_margin = 3.f;
	int m_previewAttempts;

	bool init(Icon *data, bool even);
	CCLabelBMFont *getGamemodeLabel(IconType gamemode);
	float getExtraScale(IconType type);
	CCNode* getTagContainer();

public:
	CCMenuItemSpriteExtra *m_downloadBtn = nullptr;
	LazySprite *m_preview = nullptr;

	static IconCell *create(Icon *data, bool even);

	void onDownload(CCObject *);
	void onInfo(CCObject *);
	void updateStatus();
};