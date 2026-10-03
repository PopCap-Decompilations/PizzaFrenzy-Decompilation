// GameLogic (a day's gameplay controller) and the phase actions it runs: intro, play, prize blimp, end of the day.
#pragma once

#include <deque>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include "engine/Object.h"
#include "engine/Point.h"
#include "engine/Range.h"
#include "engine/RefPtr.h"
#include "engine/State.h"
#include "engine/StateMachine.h"
#include "engine/sigslot.h"
#include "SpecialEventPopups.h"

namespace engine
{
	class Component;
	class SoundHandle;
}

struct WaveInfo;

class AnimatedText;
class Blimp;
class BuildingTile;
class CityMap;
class CouponFrame;
class CustomerTile;
class GameProgress;
class GameScreen;
class HudScreen;
class KitchenTile;
class Level;
class MusicTrack;
class Order;
class PizzaOrderPopup;
class SpecialEventPopup;
class SpecialEventTrigger;
class Tip;
class Topping;
class Vehicle;

// The gameplay controller of a day (PizzaFrenzy::m_gameLogic): waves and orders, order selection and delivery,
// combos and frenzy, tips, satisfaction, coupons, police, special-event popups, the time effect, the intro and the
// end of the day. Used as-is for Speed mode (mode 0); ConcentrationGameLogic, MemoryGame and DecoratePizzaGame
// derive from it. A signal receiver (has_slots<> at +0xC: the application's right clicks, a splat's finish).
// 0x120 bytes: Object +0x0, has_slots<> +0xC, the members below, vtordisp +0x118, Interface +0x11C.
class GameLogic : public engine::Object, public sigslot::has_slots<>
{
public:
	GameLogic();
	virtual ~GameLogic();

	virtual void init(GameScreen* screen, CityMap* map, HudScreen* hud, GameProgress* player);	// slot 1
	virtual void update(engine::UpdateContext& ctx);						// slot 2
	virtual void handleEvent(const std::string& name);						// slot 3: "customerHangup", "falseArrest", "specialHangup"
	virtual void startLevel(Level* level);									// slot 4
	virtual void unloadLevel();												// slot 5
	virtual void resetLevel();												// slot 6
	virtual void startPlaying();											// slot 7
	virtual void endLevel();												// slot 8
	virtual void clearMap();												// slot 9
	virtual void hangUpAll(bool silent);									// slot 10
	virtual void setPaused(bool paused);									// slot 11
	virtual void onOrdersChanged();											// slot 12 (0x4D0470, folded: empty)
	virtual void deselectAll();												// slot 13
	virtual void updateOrders(engine::UpdateContext& ctx);					// slot 14
	virtual void updateMap(engine::UpdateContext& ctx);						// slot 15
	virtual void checkLevelEnd();											// slot 16
	virtual void onTileClicked(BuildingTile* building);						// slot 17
	virtual void onRightClick(const engine::Point& position);				// slot 18: Application::m_rightMouseDownSignal
	virtual void deliverPizza(KitchenTile* kitchen, CustomerTile* customer, bool sendVehicle);	// slot 19
	virtual void startSpecialEvent(KitchenTile* kitchen, CustomerTile* customer);	// slot 20
	virtual void spawnTip(BuildingTile* building, Order* order, float waitTime);	// slot 21
	virtual void onUnknown22(void* a, void* b, void* c);					// slot 22: empty, no caller found
	virtual void placeTip(Order* order, const engine::Vector2& position, Tip* tip);	// slot 23
	virtual void addTip(Tip* tip, const engine::Vector2& position);			// slot 24
	virtual void removeTip(Tip* tip);										// slot 25
	virtual void collectMoney(int amount);									// slot 26
	virtual void showEventPopup(SpecialEventPopup* popup);					// slot 27
	virtual void removeEventPopup(SpecialEventPopup* popup);				// slot 28
	virtual void clearEventPopups();										// slot 29
	virtual void removeVehicle(Vehicle* vehicle);							// slot 30
	virtual void addVehicle(Vehicle* vehicle);								// slot 31
	virtual void setTimeEffect(float duration, float timeScale);			// slot 32
	virtual void setLowSatisfaction(bool low);								// slot 33
	virtual void useCoupon(CouponFrame* coupon);							// slot 34
	virtual void applyHeldCoupon(PizzaOrderPopup* popup);					// slot 35
	virtual void dispatchPolice(BuildingTile* policeStation, CustomerTile* criminal);	// slot 36
	virtual void updatePoliceStation();										// slot 37
	virtual bool hasCriminals();											// slot 38
	virtual bool isPoliceOrderPending();									// slot 39
	virtual void setupTitles();												// slot 40
	virtual void cheatWinLevel();											// slot 41
	virtual bool allowsScrambling();										// slot 42 (0x451610, folded: return true)
	virtual void spawnWave();												// slot 43
	virtual bool prepareOrder(Order* order);								// slot 44
	virtual bool placeOrder(Order* order);									// slot 45
	virtual void onAllOrdersDone();											// slot 46

	void notifyPopupRightClicked();
	void addOrderInPlay();
	int getSelectedCount() const;
	void addMoney(int amount);
	void adjustSatisfaction(int delta);										// also connected to a splat's finish signal
	void addPizzasDelivered(int count);
	void resetCombo();
	void onOrderFinished();
	void updateOrderCursor();
	std::string getFrenzyText(int combo);
	void clearSelection();
	void shuffleOrders(std::list<engine::RefPtr<Order> >& orders);
	void selectOrder(CustomerTile* customer);

	engine::RefPtr<CityMap> m_map;											// +0x1C city map
	engine::RefPtr<GameScreen> m_screen;									// +0x20 game screen (vehicle layers, order cursor, coupons, titles)
	engine::RefPtr<HudScreen> m_hud;										// +0x24 HUD (satisfaction meter)
	engine::RefPtr<engine::Component> m_titles;								// +0x28 "titles" of the game screen's layout (day intro)
	engine::RefPtr<GameProgress> m_player;									// +0x2C the running game's stats: satisfaction, money, combo...
	engine::StateMachine m_action;											// +0x30 current phase action (intro, play, blimp, end)
	std::vector<engine::RefPtr<SpecialEventPopup> > m_eventPopups;			// +0x48 queue of special-event popups; the first one is updated
	std::vector<engine::RefPtr<SpecialEventTrigger> > m_specialEvents;		// +0x58 the level's special events (Level::createEvents)
	engine::RefPtr<MusicTrack> m_music;										// +0x68 the level's music track
	float m_musicVolume;													// +0x6C the track's volume (restored after the time effect)
	Level* m_level;															// +0x70 current level (raw pointer)
	std::vector<WaveInfo*>::iterator m_wave;								// +0x74 next wave of Level::getWaves()
	engine::Range m_orderGap;												// +0x78 seconds between two orders of a wave
	engine::Range m_patienceRange;											// +0x80 customer patience
	SpecialEventPopupFactory m_eventFactory;								// +0x88 held only to call its create(name)
	std::vector<engine::RefPtr<CustomerTile> > m_selectedOrders;			// +0x9C customers whose orders are selected
	bool m_allOrdersSpawned;												// +0xAC no waves left
	engine::RefPtr<CouponFrame> m_heldCoupon;								// +0xB0 coupon on the cursor
	float m_timeScale;														// +0xB4 1.0 or the time-effect factor
	float m_timeEffectLeft;													// +0xB8 seconds left of the time effect
	engine::SoundHandle* m_timeEffectSound;									// +0xBC "special_timeEffect", looping (not counted)
	std::list<engine::RefPtr<Order> > m_pendingOrders;						// +0xC0 orders of the current wave not yet placed
	std::list<engine::RefPtr<Order> > m_activeOrders;						// +0xCC placed orders (popped on delivery or hang-up)
	float m_waveTimer;														// +0xD8 time until the next wave
	float m_orderTimer;														// +0xDC time until the next order (Concentration: memorising countdown)
	float m_deliveryTimer;													// +0xE0 time since the last delivery or wave start (tip stars)
	float m_speedBonusTime;													// +0xE4 speedy-delivery window (orders x 1.5 s)
	int m_ordersInPlay;														// +0xE8 orders spawned and not yet finished
	int m_ordersSpawned;													// +0xEC orders spawned this level
	int m_deliveryStreak;													// +0xF0 deliveries since the last hang-up (every 10: +10 satisfaction)
	bool m_popupRightClicked;												// +0xF4 set by PizzaOrderPopup::onRightMouseDown, consumed by onRightClick
	Topping* m_comboTopping;												// +0xF8 topping of the current combo
	engine::RefPtr<engine::Object> m_unknownFC;								// +0xFC released by the destructor only
	std::deque<std::pair<CustomerTile*, KitchenTile*> > m_deliveries;		// +0x100 queued (customer, kitchen) deliveries
	float m_deliveryDelay;													// +0x114 0.4 s between two queued deliveries
};

// The main phase of a day: each frame updateOrders(), updateMap() and checkLevelEnd() of the owner. Adds no
// members: 0x18 bytes (owner +0xC, vtordisp +0x10, Interface +0x14).
class GamePlayAction : public engine::State<GameLogic>
{
public:
	GamePlayAction(GameLogic* game);

	// engine::StateBase
	virtual void update(engine::UpdateContext& context);					// slot 3
};

// End of the day: the BurstFx title (strings 208/209/210: day over, perfect day, game over), music_levelComplete or
// music_gameOver, then the game state's "levelEnd" or "gameOver" event. Its destructor is implicit (0x456810 and
// the deleting 0x4567F0 are folded with PrizeBlimpAction's). 0x24 bytes (vtordisp +0x1C, Interface +0x20).
class LevelEndAction : public engine::State<GameLogic>
{
public:
	LevelEndAction(GameLogic* game, bool gameOver);

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void exit();													// slot 2 (0x45C240, folded with DecoratePizzaEndState's)
	virtual void update(engine::UpdateContext& context);					// slot 3

	engine::RefPtr<AnimatedText> m_title;									// +0x10 BurstFx title text
	float m_time;															// +0x14 2.5 s on screen, then 2 s leaving
	bool m_leaving;															// +0x18 second phase
	bool m_gameOver;														// +0x19 game over (true) or day complete
};

// Perfect day: the "PrizeBlimp" vehicle flies over (music_blimpFlyby), then LevelEndAction. Its destructor is
// implicit (folded with LevelEndAction's). 0x20 bytes (vtordisp +0x18, Interface +0x1C).
class PrizeBlimpAction : public engine::State<GameLogic>
{
public:
	PrizeBlimpAction(GameLogic* game);

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void update(engine::UpdateContext& context);					// slot 3

	engine::RefPtr<Blimp> m_blimp;											// +0x10 the "PrizeBlimp" vehicle
	float m_time;															// +0x14 7.5 s minimum
};

// The day intro: fades the "titles" in (3 s) and out (1 s), then GameLogic::startPlaying(); a click
// (Application::m_mouseDownSignal) skips ahead. Its destructor is implicit (0x45A290, compiler-generated). 0x34 bytes:
// State +0x0 (owner +0xC), has_slots<> +0x10, the members below, vtordisp +0x2C, Interface +0x30.
class LevelIntroAction : public engine::State<GameLogic>, public sigslot::has_slots<>
{
public:
	LevelIntroAction(GameLogic* game);

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void exit();													// slot 2
	virtual void update(engine::UpdateContext& context);					// slot 3

	void onSkipClick(const engine::Point& position);						// Application::m_mouseDownSignal

	float m_time;															// +0x20 phase countdown (0.5, 3, 1 s)
	int m_unused24;															// +0x24 never touched
	int m_phase;															// +0x28 0 wait, 1 titles in, 2 titles out
};
