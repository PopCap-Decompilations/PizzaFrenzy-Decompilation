// BuildingTile: a map tile that shows a popup (order, kitchen or police bubble) and owns vehicles.
#include <algorithm>
#include <string>
#include <vector>

#include "engine/Container.h"
#include "engine/Image.h"

#include "BuildingTile.h"
#include "CityMap.h"
#include "GameLogic.h"
#include "PizzaFrenzy.h"
#include "PizzaPopup.h"
#include "Vehicle.h"

// 0x41BD20
void BuildingTile::onMouseDown()
{
	PizzaFrenzy::getGameLogic()->onTileClicked(this);
}

// 0x41BD40
engine::Point BuildingTile::getParkingCell()
{
	engine::Point cell = getCell() + m_parkingSpot;
	Tile* tile = m_map->getTile(cell);
	if (tile && tile->isDrivable(cell))
		return cell;
	Tile* road = m_map->findNearestRoad(getCell(), engine::Point(4, 4));
	if (road)
		return road->getCell();
	return engine::Point(-1, -1);
}

// 0x41BE10
bool BuildingTile::isAvailable() const
{
	return m_popup == NULL;
}

// 0x41BE20
void BuildingTile::showHighlight(bool visible)
{
	float scaleX = m_base.x / (m_highlight->getImageWidth() - 26.0f);
	float scaleY = m_base.y / (m_highlight->getImageHeight() - 26.0f);
	m_highlight->setScale(scaleX, scaleY);
	m_highlight->setPosition(m_width * 0.5f + m_offset.x - m_base.x * 0.5f, m_height * 0.5f + m_offset.y - m_base.y * 0.5f);
	m_highlight->setVisible(visible);
	m_highlight->setAlpha(1.0f);
}

// 0x41BF10
bool BuildingTile::isPopupActive() const
{
	if (m_popup)
		return m_popup->acceptsInput();
	return false;
}

// 0x41BF30
void BuildingTile::popupRelease()
{
	if (m_popup)
		m_popup->deselect();
}

// 0x41BF50
void BuildingTile::popupMouseOut()
{
	if (m_popup)
		m_popup->select();
}

// 0x41BF70
void BuildingTile::popupReleaseIfHovered()
{
	if (m_popup && m_popup->isActive())
		m_popup->deselect();
}

// 0x41BFA0
void BuildingTile::showPopup(PizzaPopup* popup)
{
	popup->setScale(0.0f);
	if (m_popup == NULL)
	{
		m_popup = popup;
		m_overlay->addChild(popup);
		int cellSize = PizzaFrenzy::getCityMap()->getCellSize();
		engine::Vector2 position = popup->getPosition() + m_position;
		float scale = 1.0f / cellSize;
		position.x = (float)(((int)(position.x * scale) + 1) * cellSize);
		position.y = (float)(((int)(position.y * scale) + 1) * cellSize);
		position = position - m_position;
		m_popup->setPosition(position);
	}
	else
		m_nextPopup = popup;
}

// 0x41C0E0
PizzaPopup* BuildingTile::getPopup() const
{
	return m_popup;
}

// 0x41C0F0
void BuildingTile::clearPopup()
{
	m_popup = NULL;
}

// 0x41C120
void BuildingTile::removePopup()
{
	if (m_popup)
		m_popup->remove();
}

// 0x41C140
void BuildingTile::dismissPopup()
{
	if (m_popup)
		m_popup->close();
}

// 0x41C160
bool BuildingTile::isPopupFinished() const
{
	if (m_popup)
		return m_popup->isClosing();
	return false;
}

// 0x41C180
void BuildingTile::update(const engine::UpdateContext& time)
{
	// PizzaPopup::updateState takes the engine's non-const UpdateContext&, Tile slot 89 a const one
	if (m_popup)
		m_popup->updateState(const_cast<engine::UpdateContext&>(time));
	else if (m_nextPopup)
	{
		showPopup(m_nextPopup);
		m_nextPopup = NULL;
	}
}

// 0x41C1E0
BuildingTile::~BuildingTile()
{
	removeAllChildren();
	m_overlay = NULL;
	m_popup = NULL;
}

// 0x41C360
void BuildingTile::detachVehicle(Vehicle* vehicle)
{
	vehicle->removeFromMap();
	// the one-iterator erase: removes the element std::remove returns, not the range
	m_vehicles.erase(std::remove(m_vehicles.begin(), m_vehicles.end(), vehicle));
}

// 0x41C3D0
void BuildingTile::removeAllVehicles()
{
	for (std::vector<engine::RefPtr<Vehicle> >::iterator it = m_vehicles.begin(); it != m_vehicles.end(); ++it)
		(*it)->removeFromMap();
	m_vehicles.clear();
}

// 0x41C460
void BuildingTile::removeVehicle(Vehicle* vehicle)
{
	detachVehicle(vehicle);
}

// 0x41C490
BuildingTile::BuildingTile(const std::string& typeName)
	: Tile(typeName)
	, m_parkingSpot(0, 0)
{
	getGame();		// the result is unused in the original
}

// 0x41C560
Vehicle* BuildingTile::addVehicle(const engine::Point& cell, Vehicle* vehicle)
{
	m_vehicles.push_back(vehicle);
	vehicle->placeOnMap(m_map, cell, this);
	PizzaFrenzy::getGameLogic()->addVehicle(vehicle);
	return vehicle;
}
