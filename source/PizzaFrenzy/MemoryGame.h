// MemoryGame: the GameLogic of game mode 1 ("newMemoryGame", shown as "Simon Says Mode").
#pragma once

#include "GameLogic.h"

class BuildingTile;
class CustomerTile;
class KitchenTile;
class Order;

// Game mode 1: customers must be served in the order they called (the front of m_activeOrders). A wrong customer or
// pizza fails the order; consecutive correct deliveries raise the pitch of bonus_toppingCombo. Adds no members:
// 0x120 bytes like GameLogic (vtordisp +0x118, Interface +0x11C).
class MemoryGame : public GameLogic
{
public:
	MemoryGame();
	virtual ~MemoryGame();

	virtual void startPlaying();													// slot 7 (GameLogic)
	virtual void updateOrders(engine::UpdateContext& ctx);						// slot 14 (GameLogic)
	virtual void onTileClicked(BuildingTile* building);							// slot 17 (GameLogic)
	virtual void deliverPizza(KitchenTile* kitchen, CustomerTile* customer, bool sendVehicle);	// slot 19 (GameLogic)
	virtual void startSpecialEvent(KitchenTile* kitchen, CustomerTile* customer);	// slot 20 (GameLogic)
	virtual void dispatchPolice(BuildingTile* policeStation, CustomerTile* criminal);	// slot 36 (GameLogic)
	virtual void spawnWave();														// slot 43 (GameLogic)
	virtual bool placeOrder(Order* order);										// slot 45 (GameLogic)
	virtual void onAllOrdersDone();												// slot 46 (GameLogic)
};
