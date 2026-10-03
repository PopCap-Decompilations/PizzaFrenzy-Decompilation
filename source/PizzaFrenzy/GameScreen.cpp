// GameScreen: the in-game city view (map layers, order cursor, HUD, background baked from the city map).
#include <stdio.h>

#include <list>
#include <map>
#include <string>
#include <vector>

#include "engine/Application.h"
#include "engine/Graphics.h"
#include "engine/HsvFilter.h"
#include "engine/Image.h"
#include "engine/Rect.h"
#include "engine/ScreenLayout.h"
#include "engine/Surface.h"

#include "CityMap.h"
#include "CouponFrame.h"
#include "CustomerTile.h"
#include "Decoration.h"
#include "GameScreen.h"
#include "HudMessage.h"
#include "HudScreen.h"
#include "KitchenTile.h"
#include "PizzaFrenzy.h"
#include "PizzaPopup.h"
#include "SpeedBonusBanner.h"
#include "Tile.h"
#include "Tip.h"
#include "Vehicle.h"

// 0x434BD0
void GameScreen::onKeyDown(int keyCode)
{
	getGame()->onKeyPress(keyCode);
}

// 0x434BE0
GameScreen::GameScreen()
{
	m_map = NULL;
	m_hud = NULL;
}

// 0x434D30
GameScreen::~GameScreen()
{
	m_map = NULL;
	m_hud = NULL;
	m_roadGroup = NULL;
	m_groundGroup = NULL;
	m_airGroup = NULL;
	m_sfxGroup = NULL;
	m_tipsGroup = NULL;
	m_orderCursor = NULL;
	m_invalidOrderMarker = NULL;
}

// 0x4350C0
void GameScreen::clearLayers()
{
	m_roadGroup->removeAllChildren();
	m_groundGroup->removeAllChildren();
	m_airGroup->removeAllChildren();
	m_sfxGroup->removeAllChildren();
	m_tipsGroup->removeAllChildren();
	m_orderCursor->removeAllChildren();
}

// 0x435120
void GameScreen::changeLayer(engine::Component* item, int fromLayer, int toLayer)
{
	engine::Container* from = NULL;
	switch (fromLayer)
	{
	case 3:
		from = m_groundGroup;
		break;
	case 4:
		from = m_airGroup;
		break;
	}
	if (from)
		from->removeChild(item);

	engine::Container* to = NULL;
	switch (toLayer)
	{
	case 3:
		to = m_groundGroup;
		break;
	case 4:
		to = m_airGroup;
		break;
	}
	if (to)
		to->addChild(item);
}

// 0x435180
void GameScreen::clearOrderCursor()
{
	m_orderCursor->removeAllChildren();
	m_hoverTarget = NULL;
}

// 0x4351C0
void GameScreen::setInvalidOrderMarker(engine::Component* marker)
{
	m_invalidOrderMarker = marker;
	marker->setVisible(false);
}

// 0x435210
void GameScreen::flashInvalidOrderMarker()
{
	if (m_invalidOrderMarker)
	{
		m_invalidOrderMarker->setVisible(true);
		m_invalidMarkerTimer = 0.3f;
	}
}

// 0x435230
void GameScreen::addTip(engine::Component* tip)
{
	m_tipsGroup->addChild(tip);
}

// 0x435240
void GameScreen::removeVehicle(Vehicle* vehicle)
{
	(vehicle->m_layer == 4 ? m_airGroup.get() : m_groundGroup.get())->removeChild(vehicle);
}

// 0x435270
void GameScreen::addVehicle(Vehicle* vehicle)
{
	(vehicle->m_layer == 4 ? m_airGroup.get() : m_groundGroup.get())->addChild(vehicle);
}

// 0x4352A0
void GameScreen::setOrderCursorItem(engine::Component* item)
{
	m_orderCursor->removeAllChildren();
	if (item)
	{
		item->setPosition(0.0f, 0.0f);
		m_orderCursor->addChild(item);
	}
}

// 0x4352E0
void GameScreen::showMessage(const std::string& text, const engine::Vector2& pos)
{
	m_message->setText(text);
	m_message->setPosition(pos);
	m_message->show();
}

// 0x435320
void GameScreen::onMouseDown()
{
	if (m_hoverTarget)
		m_hoverTarget->onMouseDown();
}

// 0x435340
void GameScreen::onRightMouseDown()
{
	if (m_hoverTarget)
		m_hoverTarget->onRightMouseDown();
}

// 0x435380: moves the order cursor to the mouse and picks the object under it: carrying an order, the open popup,
// coupon or tip the cursor overlaps most; otherwise the first whose bounds contain the cursor's.
void GameScreen::updateOrderCursor(engine::UpdateContext& context)
{
	engine::Point mouse = engine::getApplication()->getMousePosition();
	m_orderCursor->setPosition(engine::Vector2((float)mouse.x, (float)mouse.y));

	if (m_invalidMarkerTimer > 0.0f)
	{
		m_invalidMarkerTimer -= context.elapsed;
		if (m_invalidMarkerTimer < 0.0f && m_invalidOrderMarker)
		{
			m_invalidMarkerTimer = 0.0f;
			m_invalidOrderMarker->setVisible(false);
		}
	}

	engine::Component* target = NULL;
	float bestArea = 0.0f;
	if (m_orderCursor->getChildCount() > 0)
	{
		std::vector<engine::RefPtr<KitchenTile> >& kitchens = m_map->m_kitchens;
		for (std::vector<engine::RefPtr<KitchenTile> >::iterator it = kitchens.begin(); it != kitchens.end(); ++it)
		{
			KitchenTile* kitchen = *it;
			if (kitchen->isPopupActive())
			{
				PizzaPopup* popup = kitchen->getPopup();
				engine::Rect bounds = popup->getBounds();
				bounds.offset(kitchen->getPosition().x, kitchen->getPosition().y);
				if (m_orderCursor->getBounds().intersects(bounds))
				{
					engine::Rect overlap;
					overlap.intersect(m_orderCursor->getBounds(), bounds);
					if (overlap.getArea() > bestArea)
					{
						target = popup;
						bestArea = overlap.getArea();
					}
				}
			}
		}

		std::vector<engine::RefPtr<CustomerTile> >& customers = m_map->m_customers;
		for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = customers.begin(); it != customers.end(); ++it)
		{
			CustomerTile* customer = *it;
			if (customer->isPopupActive())
			{
				PizzaPopup* popup = customer->getPopup();
				engine::Rect bounds = popup->getBounds();
				bounds.offset(customer->getPosition().x, customer->getPosition().y);
				if (m_orderCursor->getBounds().intersects(bounds))
				{
					engine::Rect overlap;
					overlap.intersect(m_orderCursor->getBounds(), bounds);
					if (overlap.getArea() > bestArea)
					{
						target = popup;
						bestArea = overlap.getArea();
					}
				}
			}
		}

		std::list<engine::RefPtr<CouponFrame> >& coupons = m_hud->getCoupons();
		for (std::list<engine::RefPtr<CouponFrame> >::iterator it = coupons.begin(); it != coupons.end(); ++it)
		{
			CouponFrame* coupon = *it;
			engine::Rect bounds = coupon->getBounds();
			if (m_orderCursor->getBounds().intersects(bounds))
			{
				engine::Rect overlap;
				overlap.intersect(m_orderCursor->getBounds(), bounds);
				if (overlap.getArea() > bestArea)
				{
					target = coupon;
					bestArea = overlap.getArea();
				}
			}
		}

		std::vector<engine::RefPtr<Tip> >& tips = m_map->m_tips;
		for (std::vector<engine::RefPtr<Tip> >::iterator it = tips.begin(); it != tips.end(); ++it)
		{
			Tip* tip = *it;
			if (tip->m_clickable)
			{
				engine::Rect bounds = tip->getBounds();
				if (m_orderCursor->getBounds().intersects(bounds))
				{
					engine::Rect overlap;
					overlap.intersect(m_orderCursor->getBounds(), bounds);
					if (overlap.getArea() > bestArea)
					{
						target = tip;
						bestArea = overlap.getArea();
					}
				}
			}
		}
	}
	else
	{
		std::vector<engine::RefPtr<KitchenTile> >& kitchens = m_map->m_kitchens;
		for (std::vector<engine::RefPtr<KitchenTile> >::iterator it = kitchens.begin(); it != kitchens.end(); ++it)
		{
			KitchenTile* kitchen = *it;
			if (kitchen->isPopupActive())
			{
				PizzaPopup* popup = kitchen->getPopup();
				engine::Rect bounds = popup->getBounds();
				bounds.offset(kitchen->getPosition().x, kitchen->getPosition().y);
				if (bounds.contains(m_orderCursor->getBounds()))
				{
					target = popup;
					break;
				}
			}
		}

		std::vector<engine::RefPtr<CustomerTile> >& customers = m_map->m_customers;
		for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = customers.begin(); it != customers.end(); ++it)
		{
			CustomerTile* customer = *it;
			if (customer->isPopupActive())
			{
				PizzaPopup* popup = customer->getPopup();
				engine::Rect bounds = popup->getBounds();
				bounds.offset(customer->getPosition().x, customer->getPosition().y);
				if (bounds.contains(m_orderCursor->getBounds()))
				{
					target = popup;
					break;
				}
			}
		}

		std::list<engine::RefPtr<CouponFrame> >& coupons = m_hud->getCoupons();
		for (std::list<engine::RefPtr<CouponFrame> >::iterator it = coupons.begin(); it != coupons.end(); ++it)
		{
			CouponFrame* coupon = *it;
			if (coupon->getBounds().contains(m_orderCursor->getBounds()))
			{
				target = coupon;
				break;
			}
		}

		std::vector<engine::RefPtr<Tip> >& tips = m_map->m_tips;
		for (std::vector<engine::RefPtr<Tip> >::iterator it = tips.begin(); it != tips.end(); ++it)
		{
			Tip* tip = *it;
			if (tip->getBounds().contains(m_orderCursor->getBounds()) && tip->m_clickable)
			{
				target = tip;
				break;
			}
		}
	}

	if (m_hoverTarget && m_hoverTarget != target)
	{
		m_hoverTarget->onMouseLeave();
		m_hoverTarget = NULL;
	}
	if (target && target != m_hoverTarget)
	{
		m_hoverTarget = target;
		target->onMouseEnter();
	}
}

// 0x435BA0
void GameScreen::tick(engine::UpdateContext& context)
{
	m_speedBonus->update(context);

	std::vector<engine::RefPtr<Vehicle> >& vehicles = m_map->m_vehicles;
	for (std::vector<engine::RefPtr<Vehicle> >::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
	{
		Vehicle* vehicle = *it;
		if (vehicle->isZOrderDirty() && vehicle->m_layer == 3)
		{
			m_groundGroup->updateChildOrder(vehicle);
			vehicle->clearZOrderDirty();
		}
	}

	updateOrderCursor(context);
}

// 0x435C60: fills the layers from the city map's tiles, decorations and vehicles, and bakes the background image,
// map layers 0 and 1 and the free-standing decorations into m_backgroundSurface.
void GameScreen::buildMap()
{
	int width = m_map->getCellSize() * m_map->getCols();
	int height = m_map->getCellSize() * m_map->getRows();
	if (!m_backgroundSurface)
		m_backgroundSurface = engine::getApplication()->createBlankImage(width, height);

	engine::RefPtr<engine::Graphics> g = engine::getApplication()->createGraphics(m_backgroundSurface);
	if (!m_map->getBackground().empty())
	{
		engine::Bitmap* background = engine::getApplication()->getImage(m_map->getBackground().c_str());
		std::string hsvShift = m_map->getHsvShift();
		if (background)
		{
			g->drawImage(background, 0.0f, 0.0f);
			float hue;
			float saturation;
			float value;
			sscanf(hsvShift.c_str(), "%f,%f,%f", &hue, &saturation, &value);
			if (hue != 0.0f || saturation != 0.0f || value != 0.0f)
			{
				engine::HsvFilter filter;
				filter.set(hue, saturation, value);
				m_backgroundSurface->applyFilter(&filter);
			}
		}
	}

	m_roadGroup->removeAllChildren();
	m_groundGroup->removeAllChildren();
	m_airGroup->removeAllChildren();
	m_sfxGroup->removeAllChildren();

	std::map<std::string, engine::RefPtr<Tile> >& tiles = m_map->m_tilesByName;
	for (std::map<std::string, engine::RefPtr<Tile> >::iterator it = tiles.begin(); it != tiles.end(); ++it)
	{
		Tile* tile = it->second;
		tile->updateConnections();
		engine::Component* component = tile->getLayerComponent(2);
		if (component)
			m_roadGroup->addChild(component);
		component = tile->getLayerComponent(3);
		if (component)
			m_groundGroup->addChild(component);
		component = tile->getLayerComponent(4);
		if (component)
			m_airGroup->addChild(component);
		component = tile->getLayerComponent(5);
		if (component)
			m_sfxGroup->addChild(component);
	}

	std::vector<engine::RefPtr<Decoration> >& decorations = m_map->m_decorations;
	for (std::vector<engine::RefPtr<Decoration> >::iterator it = decorations.begin(); it != decorations.end(); ++it)
	{
		Decoration* decoration = *it;
		engine::Component* component = decoration->getComponent(3);
		if (component)
			m_groundGroup->addChild(component);
		component = decoration->getComponent(4);
		if (component)
			m_airGroup->addChild(component);
	}

	for (std::map<std::string, engine::RefPtr<Tile> >::iterator it = tiles.begin(); it != tiles.end(); ++it)
	{
		engine::Component* component = it->second->getLayerComponent(0);
		if (component)
		{
			g->pushState();
			component->setupGraphics(*g);
			component->draw(*g);
			g->popState();
		}
	}
	for (std::vector<engine::RefPtr<Decoration> >::iterator it = decorations.begin(); it != decorations.end(); ++it)
	{
		engine::Component* component = (*it)->getComponent(0);
		if (component)
		{
			g->pushState();
			component->setupGraphics(*g);
			component->draw(*g);
			g->popState();
		}
	}
	for (std::map<std::string, engine::RefPtr<Tile> >::iterator it = tiles.begin(); it != tiles.end(); ++it)
	{
		engine::Component* component = it->second->getLayerComponent(1);
		if (component)
		{
			g->pushState();
			component->setupGraphics(*g);
			component->draw(*g);
			g->popState();
		}
	}
	for (std::vector<engine::RefPtr<Decoration> >::iterator it = decorations.begin(); it != decorations.end(); ++it)
	{
		engine::Component* component = (*it)->getComponent(1);
		if (component)
		{
			g->pushState();
			component->setupGraphics(*g);
			component->draw(*g);
			g->popState();
		}
	}

	// ground-layer decorations that overlap no tile are baked into the background and dropped from the layer
	std::vector<engine::Component*>& children = m_groundGroup->getChildren();
	for (std::vector<engine::Component*>::iterator child = children.begin(); child != children.end(); )
	{
		bool overTile = false;
		for (std::map<std::string, engine::RefPtr<Tile> >::iterator it = tiles.begin(); it != tiles.end(); ++it)
		{
			Tile* tile = it->second;
			tile->updateBounds();
			engine::Component* component = *child;
			component->updateBounds();
			if (tile->getBounds().intersects(component->getBounds()))
			{
				overTile = true;
				break;
			}
		}
		if (!overTile && (*child)->getName() == "decoration")
		{
			engine::Image* image = (engine::Image*)*child;
			g->drawImage(image->getImage(), image->getPosition().x, image->getPosition().y);
			child = m_groundGroup->eraseChild(child);
		}
		else
			++child;
	}

	std::vector<engine::RefPtr<Vehicle> >& vehicles = m_map->m_vehicles;
	for (std::vector<engine::RefPtr<Vehicle> >::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
		((*it)->m_layer == 4 ? m_airGroup.get() : m_groundGroup.get())->addChild(*it);

	m_background->setImage(m_backgroundSurface);
}

// 0x436440
void GameScreen::init(HudScreen* hud, CityMap* map)
{
	m_map = map;

	m_background = new engine::Image();
	addChild(m_background);

	m_roadGroup = new engine::Container();
	m_roadGroup->setName("RoadGroup");
	addChild(m_roadGroup);

	m_groundGroup = new DepthSortedContainer();
	m_groundGroup->setName("GroundGroup");
	addChild(m_groundGroup);

	m_airGroup = new engine::Container();
	m_airGroup->setName("AirGroup");
	addChild(m_airGroup);

	m_tipsGroup = new engine::Container();
	m_tipsGroup->setName("TipsGroup");
	addChild(m_tipsGroup);

	m_sfxGroup = new engine::Container();
	m_sfxGroup->setName("SfxGroup");
	addChild(m_sfxGroup);

	m_speedBonus = new SpeedBonusBanner();
	m_speedBonus->init();
	addChild(m_speedBonus);

	m_hud = hud;
	if (m_hud)
		addChild(m_hud);

	m_message = new HudMessage();
	m_message->setPosition(400.0f, 500.0f);
	addChild(m_message);

	m_orderCursor = new engine::Container();
	m_orderCursor->setName("OrderCursor");
	addChild(m_orderCursor);

	m_layout = new engine::ScreenLayout();
	m_layout->load("res/screenLayouts/gameScreen.xml");
	m_titles = m_layout->getComponent("titles");
	addChild(m_layout);

	setFlags(2);
}
