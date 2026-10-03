// Blimp: the prize blimp that crosses the city after a perfect day and drops BalloonTips.
#include <string>

#include "engine/Application.h"
#include "engine/Color.h"
#include "engine/Font.h"
#include "engine/Graphics.h"
#include "engine/Image.h"
#include "engine/Oscillator.h"
#include "engine/Rect.h"
#include "engine/Selector.h"
#include "engine/SoundHandle.h"
#include "engine/SoundMgr.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "engine/User.h"
#include "engine/UserManager.h"
#include "BalloonTip.h"
#include "Blimp.h"
#include "CityMap.h"
#include "GameLogic.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

engine::Range Blimp::s_tipDropInterval(1.0f, 1.75f);		// 0x53290C
int Blimp::s_pendingTips;									// 0x532958

// 0x430970
void Blimp::fly(engine::UpdateContext& ctx)
{
	int startY = (int)getY();
	engine::Vector2 pos(getPosition());
	pos += m_velocity * ctx.elapsed;
	setPosition(pos);
	if ((int)getY() != startY)
		m_zOrderDirty = true;
	int height = engine::getApplication()->getHeight();
	int width = engine::getApplication()->getWidth();
	if ((pos.x > width + 100 && m_velocity.x > 0.0f) || (pos.x < -100.0f && m_velocity.x < 0.0f)
		|| (pos.y > height + 50 && m_velocity.y > 0.0f) || (pos.y < -50.0f && m_velocity.y < 0.0f))
	{
		removeFromMap();
		if (m_engineSound)
			m_engineSound->stop();
		m_finished = true;
	}
}

// 0x430AC0
void Blimp::setDirection(const engine::Vector2& dir)
{
	m_velocity = dir * m_speed;
	faceTowards(engine::Point(0, 0));
}

// 0x430B10
bool Blimp::isFinished()
{
	return m_finished && s_pendingTips <= 0;
}

// 0x430B30
void Blimp::faceTowards(const engine::Point& tile)
{
	m_selector->select(0, 0.0f, true);
	float scaleX = 1.0f;
	if (m_velocity.x > 0.0f)
		scaleX = -1.0f;
	setScale(scaleX, 1.0f);
}

// 0x430B90
void Blimp::updateBanner(engine::UpdateContext& ctx)
{
	m_bannerTime -= ctx.elapsed;
	if (m_bannerTime < 0.0f)
		m_bannerTime = 8.0f;
	float t = 1.0f - m_bannerTime * 0.125f;
	int x = 61 - (int)((m_banner->getImageWidth() + 143) * t);
	m_banner->setPosition((float)(x > -82 ? x : -82), -17.0f);
	int left = -82 - x > 0 ? -82 - x : 0;
	int width = m_banner->getImageWidth();
	int right = width < 61 - x ? width : 61 - x;
	m_banner->setSourceRect(engine::IntRect(left, 0, right, m_banner->getImageHeight()));
}

// 0x430CC0
void Blimp::updateOnMap(engine::UpdateContext& ctx)
{
	fly(ctx);
	updateBanner(ctx);
	engine::Container::update(ctx);
	m_dropTimer -= ctx.elapsed;
	if (m_dropTimer < 0.0f && !m_finished && getPosition().x > 50.0f && getPosition().x < 650.0f)
	{
		dropTips();
		m_dropTimer = s_tipDropInterval.random();
	}
}

// 0x430D50
bool Blimp::init()
{
	VehicleType* def = PizzaFrenzy::getTileManifest()->getVehicleType(getName());
	if (def)
	{
		engine::Bitmap* bitmap = engine::getApplication()->getImage("res\\vehicles\\blimp_dropshadow.jpg");
		bitmap->setPivotType(1);
		engine::Image* shadow = new engine::Image(bitmap);
		shadow->setPosition(-67.0f, 113.0f);
		addChild(shadow);
		setDefinition(def);
		m_layer = 4;
		setSpeed((float)def->m_speed);
		setBlendMode(1);
		addAnimator(new engine::Oscillator(0.0f, 6.0f, 0.0f, 15.0f, 0.0f));
		shadow->addAnimator(new engine::Oscillator(6.0f, 6.0f, -15.0f, -30.0f, 0.0f));
		engine::User* user = engine::UserManager::getInstance()->getCurrentUser();
		std::string text = engine::getApplication()->formatString(105, user->getName().c_str());
		engine::toUpper(text);
		engine::Font* font = engine::getApplication()->getFont("res\\fonts\\dotMatrix.xml");
		int width = font->getStringWidth(text.c_str());
		int height = font->getHeight();
		engine::Bitmap* image = engine::getApplication()->createBlankImage(width, height);
		engine::RefPtr<engine::Graphics> g = engine::getApplication()->createGraphics(image);
		image->fill(engine::Color(0, 0, 0, 255));
		image->setAlphaType(2, 255);
		g->setFont(font);
		g->drawString(text.c_str(), 0.0f, 0.0f);
		m_banner = new engine::Image(image);
		m_banner->setUseSourceRect(true);
		addChild(m_banner);
		m_bannerTime = 0.0f;
		(new BalloonTip())->release();
		return true;
	}
	return false;
}

// 0x431110
void Blimp::start(CityMap* map, const engine::Point& pos, BuildingTile* owner)
{
	m_owner = owner;
	m_map = map;
	updateBounds();
	setPosition((float)pos.x, (float)pos.y);
	m_map->addVehicle(this);
	m_zOrderDirty = false;
	m_dropTimer = 0.0f;
	m_engineSound = PizzaFrenzy::getSounds()->getSound("bonus_blimpEngine");
	if (m_engineSound)
	{
		m_engineSound->setLooping(true);
		m_engineSound->play();
	}
	m_finished = false;
	s_pendingTips = 0;
}

// 0x431220
void Blimp::dropTips()
{
	engine::Vector2 pos(getPosition());
	pos.x -= 15.0f;
	pos.y += 45.0f;
	for (int i = 0; i < 3; i++)
	{
		BalloonTip* tip = new BalloonTip();
		getGame()->getGameLogic()->placeTip(0, pos, tip);
	}
	PizzaFrenzy::getSounds()->playSound("bonus_blimpDropTip", 1.0f, 1.0f);
}

// 0x431330
Blimp::Blimp(const std::string& name)
	: Vehicle(name)
{
}

// 0x431450
Blimp::~Blimp()
{
}
