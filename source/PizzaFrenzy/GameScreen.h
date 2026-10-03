// GameScreen: the in-game city view (map layers, order cursor, HUD, background baked from the city map).
#pragma once

#include <string>

#include "engine/Point.h"
#include "engine/RefPtr.h"
#include "engine/Screen.h"

namespace engine
{
	class Bitmap;
	class Component;
	class Container;
	class Image;
	class ScreenLayout;
}

class CityMap;
class DepthSortedContainer;
class HudMessage;
class HudScreen;
class SpeedBonusBanner;
class Vehicle;

// The screen the city map is played on (game+0x114): owns the map layers RoadGroup/GroundGroup/AirGroup/SfxGroup/
// TipsGroup/OrderCursor, the HUD, the background baked from the city map and a child layout
// res/screenLayouts/gameScreen.xml; forwards keys to the game and mouse-downs to the object under the order cursor.
// The game also instantiates it for decoratePizza.xml (game+0xF4) and pizzaDesignerScreen.xml (game+0xE8). The members
// end at +0x218; the vtordisp (+0x218) and the Interface subobject (+0x21C) follow them (size 0x220).
class GameScreen : public engine::Screen
{
public:
	GameScreen();
	virtual ~GameScreen();

	virtual void onMouseDown();										// slot 42 (engine::Component)
	virtual void onRightMouseDown();								// slot 48 (engine::Component)
	virtual void onKeyDown(int keyCode);							// slot 86 (engine::ScreenLayout)

	void init(HudScreen* hud, CityMap* map);
	void clearLayers();
	void buildMap();

	// 0x40DE30
	void rebuild()
	{
		clearLayers();
		buildMap();
	}

	void changeLayer(engine::Component* item, int fromLayer, int toLayer);
	void clearOrderCursor();
	void setInvalidOrderMarker(engine::Component* marker);
	void flashInvalidOrderMarker();
	void addTip(engine::Component* tip);
	void removeVehicle(Vehicle* vehicle);
	void addVehicle(Vehicle* vehicle);
	void setOrderCursorItem(engine::Component* item);
	void showMessage(const std::string& text, const engine::Vector2& pos);
	void updateOrderCursor(engine::UpdateContext& context);
	void tick(engine::UpdateContext& context);

	engine::RefPtr<CityMap> m_map;										// +0x1D4 the city map given to init
	engine::RefPtr<HudScreen> m_hud;									// +0x1D8 the HUD, added as a child
	engine::RefPtr<engine::Image> m_background;							// +0x1DC shows m_backgroundSurface
	engine::RefPtr<engine::Container> m_roadGroup;						// +0x1E0 "RoadGroup": tile layer 2
	engine::RefPtr<DepthSortedContainer> m_groundGroup;					// +0x1E4 "GroundGroup": layer 3, ground vehicles, decorations
	engine::RefPtr<engine::Container> m_airGroup;						// +0x1E8 "AirGroup": layer 4 and flying vehicles (type 4)
	engine::RefPtr<engine::Container> m_sfxGroup;						// +0x1EC "SfxGroup": layer 5
	engine::RefPtr<engine::Container> m_tipsGroup;						// +0x1F0 "TipsGroup": tips (addTip)
	engine::RefPtr<engine::Container> m_orderCursor;					// +0x1F4 "OrderCursor": follows the mouse, holds the carried order
	engine::RefPtr<engine::Component> m_invalidOrderMarker;				// +0x1F8 shown 0.3 s by flashInvalidOrderMarker
	engine::RefPtr<engine::Component> m_hoverTarget;					// +0x1FC map object or coupon under the order cursor
	engine::RefPtr<engine::Bitmap> m_backgroundSurface;					// +0x200 map-sized image; buildMap bakes layers 0/1 into it
	engine::RefPtr<SpeedBonusBanner> m_speedBonus;						// +0x204 "SPEED BONUS!" banner, ticked each frame
	engine::RefPtr<HudMessage> m_message;								// +0x208 text popup at (400,500) used by showMessage
	engine::RefPtr<engine::ScreenLayout> m_layout;						// +0x20C child layout res/screenLayouts/gameScreen.xml
	engine::RefPtr<engine::Component> m_titles;							// +0x210 item "titles" of m_layout
	float m_invalidMarkerTimer;											// +0x214 seconds until m_invalidOrderMarker is hidden (not set
																		//        by the ctor)
};
