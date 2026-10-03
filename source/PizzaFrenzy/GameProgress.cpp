// GameProgress: the player's running game state.
#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "engine/Range.h"
#include "engine/RefPtr.h"

#include "Constants.h"
#include "GameProgress.h"
#include "Level.h"
#include "PizzaDesign.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"
#include "UserProgress.h"

// 0x419B90
void GameProgress::addCash(int amount)
{
	m_cash += amount;
	if (m_cash < 0)
		m_cash = 0;
}

// 0x419C90
void GameProgress::resetLevelStats()
{
	m_pizzasDelivered = 0;
	m_satisfaction = g_startingSatisfaction;
	m_ordersCompleted = 0;
	m_pizzasOrdered = 0;
	m_cash = 0;
	m_bonus = 0;
	m_wavesSpawned = 0;
	m_toppingCombo = 0;
	m_bestToppingCombos.clear();
	m_perfectToppings = 0;
	m_goodToppings = 0;
	m_okayToppings = 0;
	m_missedToppings = 0;
	m_secondsLeft = 0;
	m_firstLevel = true;
}

// 0x419CF0
int GameProgress::getTotalUpgradeLevel() const
{
	int total = 0;
	for (std::map<Topping*, int>::const_iterator it = m_toppingLevels.begin(); it != m_toppingLevels.end(); ++it)
	{
		if (!it->first->m_isPizza)
			total += it->second + 1;
	}
	return total;
}

// 0x419D30
void GameProgress::save(UserProgress* profile, int slot) const
{
	profile->setLastScore(slot, m_score);
	profile->setElapsedTime(slot, m_playTime);
	profile->clearToppingLevels();
	for (std::map<Topping*, int>::const_iterator it = m_toppingLevels.begin(); it != m_toppingLevels.end(); ++it)
	{
		if (it->first)
			profile->setToppingLevel(it->first, it->second);
	}
}

// 0x419DA0
Topping* GameProgress::getMostServedTopping() const
{
	int most = 0;
	Topping* topping = NULL;
	for (std::map<Topping*, int>::const_iterator it = m_bestToppingCombos.begin(); it != m_bestToppingCombos.end(); ++it)
	{
		if (it->second > most)
		{
			topping = it->first;
			most = it->second;
		}
	}
	return topping;
}

// 0x41A220
void GameProgress::assignRandomToppings(Level* level)
{
	for (std::vector<KitchenInfo*>::iterator it = level->getKitchens().begin(); it != level->getKitchens().end(); ++it)
		(*it)->topping = NULL;

	std::vector<engine::RefPtr<Topping> > available;
	for (std::map<Topping*, int>::iterator it = m_toppingLevels.begin(); it != m_toppingLevels.end(); ++it)
		available.push_back(it->first);

	UserProgress* profile = PizzaFrenzy::getProfile();
	profile->getPickedToppings().clear();
	for (std::vector<KitchenInfo*>::iterator it = level->getKitchens().begin(); it != level->getKitchens().end(); ++it)
	{
		KitchenInfo* kitchen = *it;
		if (kitchen->topping == NULL && !available.empty())
		{
			int index = engine::randomInt(0, available.size() - 1);
			kitchen->topping = available.at(index);
			if (kitchen->topping)
				profile->getPickedToppings().push_back(kitchen->topping);
			// the one-iterator erase: removes the element std::remove returns, not the range
			available.erase(std::remove(available.begin(), available.end(), kitchen->topping));
		}
	}
	profile->save();
}

// 0x41A5F0
void GameProgress::setToppingLevel(Topping* topping, int level)
{
	m_toppingLevels[topping] = level;
}

// 0x41A610
ToppingUpgrade* GameProgress::getToppingUpgrade(Topping* topping)
{
	int level = m_toppingLevels[topping];
	if (level >= (int)topping->m_upgrades.size())
		level = (int)topping->m_upgrades.size() - 1;
	return topping->m_upgrades[level];
}

// 0x41A670
int GameProgress::getToppingLevel(Topping* topping)
{
	if (getGame()->m_mode == 2)
		return 1;
	return m_toppingLevels[topping] + 1;
}

// 0x41A6A0
void GameProgress::unlockPizzaToppings()
{
	std::vector<engine::RefPtr<PizzaDesign> >& pizzas = getGame()->m_userPizzas;
	for (std::vector<engine::RefPtr<PizzaDesign> >::iterator it = pizzas.begin(); it != pizzas.end(); ++it)
	{
		Topping* topping = getGame()->m_userPizzaCombos[(*it)->getId()];
		int level = PizzaFrenzy::getProfile()->getToppingLevel(topping);
		if (level >= 0)
			m_toppingLevels[topping] = level;
		else
			m_toppingLevels[topping] = 0;
	}
}

// 0x41A740
void GameProgress::startLevel(Level* level)
{
	m_ordersLeft = m_levelOrders = level->m_numOrders;
	m_cashGoal = level->m_goal;
	resetLevelStats();
	bool full = true;
	for (std::vector<KitchenInfo*>::iterator it = level->getKitchens().begin(); it != level->getKitchens().end(); ++it)
	{
		if ((*it)->topping == NULL)
			full = false;
	}
	if (!full)
		assignRandomToppings(level);
}

// 0x41A7B0
void GameProgress::reset()
{
	m_score = 0;
	m_playTime = 0.0f;
	m_levelOrders = 0;
	m_toppingLevels.clear();
	std::map<std::string, engine::RefPtr<Topping> >& toppings = PizzaFrenzy::getTileManifest()->m_toppings;
	for (std::map<std::string, engine::RefPtr<Topping> >::iterator it = toppings.begin(); it != toppings.end(); ++it)
	{
		int level = PizzaFrenzy::getProfile()->getToppingLevel(it->second);
		if (level >= 0)
		{
			m_toppingLevels[it->second] = level;
			PizzaFrenzy::getProfile()->removeNewTopping(it->second);
		}
	}
	if ((int)m_toppingLevels.size() >= g_pizzaEditorUnlock)
		unlockPizzaToppings();
	m_cashGoal = 0;
	resetLevelStats();
}

// 0x41A900
void GameProgress::load(UserProgress* profile, int slot)
{
	reset();
	m_score = profile->getLastScore(slot);
	m_playTime = profile->getElapsedTime(slot);
}

// 0x41A930
GameProgress::GameProgress()
{
	reset();
}
