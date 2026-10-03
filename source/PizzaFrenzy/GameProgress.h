// GameProgress: the player's running game state (score, cash, unlocked toppings and their levels, level stats).
#pragma once

#include <map>

#include "engine/Object.h"

class Level;
class Topping;
class ToppingUpgrade;
class UserProgress;

// Kept at PizzaFrenzy +0x120. Implicit destructor (0x41AA60).
class GameProgress : public engine::Object
{
public:
	GameProgress();

	void addCash(int amount);
	void resetLevelStats();
	int getTotalUpgradeLevel() const;
	void save(UserProgress* profile, int slot) const;
	Topping* getMostServedTopping() const;
	void assignRandomToppings(Level* level);
	void setToppingLevel(Topping* topping, int level);
	ToppingUpgrade* getToppingUpgrade(Topping* topping);
	int getToppingLevel(Topping* topping);
	void unlockPizzaToppings();
	void startLevel(Level* level);
	void reset();
	void load(UserProgress* profile, int slot);

	int m_score;										// +0x0C total score
	float m_playTime;									// +0x10 saved per profile slot
	std::map<Topping*, int> m_toppingLevels;			// +0x14 unlocked toppings and their upgrade level
	int m_pizzasDelivered;								// +0x20 per level (GameLogic::addPizzasDelivered)
	int m_pizzasOrdered;								// +0x24 per level: each order's pizza count; all delivered
														//       makes the day perfect (the decorating game sets 5)
	int m_satisfaction;									// +0x28 starts at g_startingSatisfaction
	int m_ordersCompleted;								// +0x2C per level; the daily report's "%d/%d" of m_levelOrders
	int m_ordersLeft;									// +0x30 Level::m_numOrders at level start
	int m_cash;											// +0x34 never below 0 (addCash)
	int m_bonus;										// +0x38 tips and awards
	int m_wavesSpawned;									// +0x3C per level
	int m_toppingCombo;									// +0x40 deliveries of one topping in a row (the memory
														//       game: right deliveries in a row)
	std::map<Topping*, int> m_bestToppingCombos;		// +0x44 per level, the longest combo of each topping
	int m_perfectToppings;								// +0x50 decorating game: toppings placed perfectly (100 each)
	int m_goodToppings;									// +0x54 decorating game: good (20 each)
	int m_okayToppings;									// +0x58 decorating game: okay (5 each)
	int m_missedToppings;								// +0x5C decorating game: missed (-50 each)
	int m_secondsLeft;									// +0x60 decorating game: whole seconds left (100 each)
	int m_levelOrders;									// +0x64 Level::m_numOrders at level start
	int m_cashGoal;										// +0x68 Level::m_goal: the level's cash target (tip-jar size)
	bool m_firstLevel;									// +0x6C set by reset and resetLevelStats
};
