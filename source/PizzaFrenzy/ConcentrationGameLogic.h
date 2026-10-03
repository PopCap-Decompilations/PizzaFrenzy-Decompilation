// ConcentrationGameLogic: the GameLogic of game mode 2 ("newConcentrationGame").
#pragma once

#include <string>

#include "GameLogic.h"

namespace engine
{
	class SoundHandle;
}

class CityMap;
class GameProgress;
class GameScreen;
class HudScreen;
class Order;

// Game mode 2: a wave's orders arrive face up with a waiting loop (popup_wait) and then all flip over
// (popup_flipover), so the player has to remember them. 0x124 bytes: GameLogic up to +0x118, m_waitSound, vtordisp
// +0x11C, Interface +0x120.
class ConcentrationGameLogic : public GameLogic
{
public:
	ConcentrationGameLogic();
	virtual ~ConcentrationGameLogic();

	// GameLogic
	virtual void init(GameScreen* screen, CityMap* map, HudScreen* hud, GameProgress* player);	// slot 1
	virtual void handleEvent(const std::string& name);						// slot 3
	virtual void onOrdersChanged();											// slot 12
	virtual void updateOrders(engine::UpdateContext& ctx);					// slot 14
	virtual void onRightClick(const engine::Point& position);				// slot 18
	virtual bool allowsScrambling();										// slot 42 (0x4529D0, folded: return false)
	virtual bool placeOrder(Order* order);									// slot 45

	void flipOverAll();

	engine::SoundHandle* m_waitSound;										// +0x118 "popup_wait", looping (not counted; not initialised by the constructor)
};
