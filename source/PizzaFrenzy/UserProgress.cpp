// UserProgress: the current user's saved progress, kept in the user's properties.
#include "UserProgress.h"

#include <algorithm>
#include <stdlib.h>

#include "engine/Properties.h"
#include "engine/StringUtil.h"
#include "engine/User.h"
#include "engine/UserManager.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x4277B0
int UserProgress::getLastLevel(int mode)
{
	return m_modes[mode].m_lastLevel;
}

// 0x4277C0
void UserProgress::setLastLevel(int mode, int level)
{
	m_modes[mode].m_lastLevel = level;
}

// 0x4277E0
int UserProgress::getLastCity(int mode)
{
	return m_modes[mode].m_lastCity;
}

// 0x4277F0
void UserProgress::setLastCity(int mode, int city)
{
	m_modes[mode].m_lastCity = city;
}

// 0x427810
int UserProgress::getLastDay(int mode)
{
	return m_modes[mode].m_lastDay;
}

// 0x427820
void UserProgress::setLastDay(int mode, int day)
{
	m_modes[mode].m_lastDay = day;
}

// 0x427840
int UserProgress::getLastScore(int mode)
{
	return m_modes[mode].m_lastScore;
}

// 0x427860
void UserProgress::setLastScore(int mode, int score)
{
	m_modes[mode].m_lastScore = score;
}

// 0x427880
int UserProgress::getHighestDay(int mode)
{
	return m_modes[mode].m_highestDay;
}

// 0x4278A0
void UserProgress::updateHighestDay(int mode, int day)
{
	if (day > m_modes[mode].m_highestDay)
		m_modes[mode].m_highestDay = day;
}

// 0x4278D0
float UserProgress::getElapsedTime(int mode)
{
	return m_modes[mode].m_elapsedTime;
}

// 0x4278F0
void UserProgress::setElapsedTime(int mode, float time)
{
	m_modes[mode].m_elapsedTime = time;
}

// 0x427910
bool UserProgress::hasGameInProgress(int mode)
{
	return m_modes[mode].m_lastDay > -1 && m_modes[mode].m_lastScore > -1;
}

// 0x427940
std::vector<engine::RefPtr<Topping> >& UserProgress::getNewToppings()
{
	return m_newToppings;
}

// 0x427950
std::vector<engine::RefPtr<Topping> >& UserProgress::getPickedToppings()
{
	return m_pickedToppings;
}

// 0x427960
bool UserProgress::getUnlockTopping()
{
	return m_unlockTopping;
}

// 0x427970
void UserProgress::setUnlockTopping(bool unlock)
{
	m_unlockTopping = unlock;
}

// 0x4504B0 (folded)
int UserProgress::getMaxCity()
{
	return m_maxCity;
}

// 0x427980
void UserProgress::updateMaxCity(int city)
{
	m_maxCity = city < m_maxCity ? m_maxCity : city;
}

// 0x427AF0
void UserProgress::clearToppingLevels()
{
	m_toppingLevels.clear();
}

// 0x427B20
std::string UserProgress::getToppingLevelsString()
{
	std::string text;
	for (std::map<Topping*, int>::iterator it = m_toppingLevels.begin(); it != m_toppingLevels.end(); ++it)
	{
		std::string entry;
		engine::format(entry, "%s=%d,", it->first->m_name.c_str(), it->second);
		text += entry.c_str();
	}
	return text;
}

// 0x427C80
std::string UserProgress::getNewToppingsString()
{
	std::string text;
	for (std::vector<engine::RefPtr<Topping> >::iterator it = m_newToppings.begin(); it != m_newToppings.end(); ++it)
	{
		text += (*it)->m_name.c_str();
		text += ",";
	}
	return text;
}

// 0x427D80
std::string UserProgress::getPickedToppingsString()
{
	std::string text;
	for (std::vector<engine::RefPtr<Topping> >::iterator it = m_pickedToppings.begin(); it != m_pickedToppings.end(); ++it)
	{
		text += (*it)->m_name.c_str();
		text += ",";
	}
	return text;
}

// 0x427E80
void UserProgress::reset()
{
	for (int i = 0; i < 3; i++)
	{
		m_modes[i].m_mode = i;
		m_modes[i].m_lastLevel = -1;
		m_modes[i].m_lastCity = -1;
		m_modes[i].m_lastDay = -1;
		m_modes[i].m_lastScore = -1;
		m_modes[i].m_highestDay = -1;
	}
	m_toppingLevels.clear();
	m_newToppings.clear();
	m_pickedToppings.clear();
	m_maxCity = 0;
	m_userName.clear();
	m_unlockTopping = false;
}

// 0x427FF0
bool UserProgress::save()
{
	engine::UserManager* manager = engine::UserManager::getInstance();
	engine::User* user = manager->getCurrentUser();
	if (!user)
		return false;

	std::string name;
	user->getAttributes().setInt("maxCity", m_maxCity);
	user->getAttributes().setBool("unlockTopping", m_unlockTopping);
	user->getAttributes().setString("ToppingLevels", getToppingLevelsString());
	user->getAttributes().setString("NewToppings", getNewToppingsString());
	user->getAttributes().setString("PickToppings", getPickedToppingsString());
	for (int i = 0; i < 3; i++)
	{
		name.clear();
		engine::format(name, "mode%dLastLevel", i);
		user->getAttributes().setInt(name, m_modes[i].m_lastLevel);
		name.clear();
		engine::format(name, "mode%dLastCity", i);
		user->getAttributes().setInt(name, m_modes[i].m_lastCity);
		name.clear();
		engine::format(name, "mode%dHighestDay", i);
		user->getAttributes().setInt(name, m_modes[i].m_highestDay);
		name.clear();
		engine::format(name, "mode%dLastDay", i);
		user->getAttributes().setInt(name, m_modes[i].m_lastDay);
		name.clear();
		engine::format(name, "mode%dLastPizza", i);
		user->getAttributes().setInt(name, m_modes[i].m_lastPizza);
		name.clear();
		engine::format(name, "mode%dLastScore", i);
		user->getAttributes().setInt(name, m_modes[i].m_lastScore);
		name.clear();
		engine::format(name, "mode%dLastTopping", i);
		user->getAttributes().setString(name, m_modes[i].m_lastTopping);
		name.clear();
		engine::format(name, "mode%dElapsedTime", i);
		user->getAttributes().setFloat(name, m_modes[i].m_elapsedTime);
	}
	manager->saveUser(user);
	return true;
}

// 0x428670
bool UserProgress::clearGameInProgress(int mode)
{
	m_modes[mode].m_lastLevel = -1;
	m_modes[mode].m_lastCity = -1;
	m_modes[mode].m_lastDay = -1;
	m_modes[mode].m_lastScore = -1;
	return save();
}

// 0x4286A0
void UserProgress::removeNewTopping(Topping* topping)
{
	if (std::find(m_newToppings.begin(), m_newToppings.end(), topping) != m_newToppings.end())
		m_newToppings.erase(std::remove(m_newToppings.begin(), m_newToppings.end(), topping));
}

// 0x4287C0
int UserProgress::getToppingLevel(Topping* topping)
{
	std::map<Topping*, int>::iterator it = m_toppingLevels.find(topping);
	if (it == m_toppingLevels.end())
		return -1;
	return m_toppingLevels[topping];
}

// 0x428800
void UserProgress::setToppingLevel(Topping* topping, int level)
{
	m_toppingLevels[topping] = level;
}

// 0x428820
void UserProgress::parseToppingLevels(const std::string& text)
{
	m_toppingLevels.clear();
	std::string::size_type start = 0;
	// the next find runs after each entry is destroyed (0x42898B-0x42899A, then the find at 0x4289BA)
	for (std::string::size_type pos = text.find(","); pos != std::string::npos; pos = text.find(",", start))
	{
		std::string entry = text.substr(start, pos - start);
		start = pos + 1;
		std::string::size_type equals = entry.find("=");
		if (equals != std::string::npos)
		{
			std::string name = entry.substr(0, equals);
			std::string value = entry.substr(equals + 1);
			int level = atoi(value.c_str());
			Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(name);
			if (topping)
				m_toppingLevels[topping] = level;
		}
	}
}

// 0x4289F0
void UserProgress::parseNewToppings(const std::string& text)
{
	m_newToppings.clear();
	std::string::size_type start = 0;
	// the next find runs after each name is destroyed (0x428B2A-0x428B39, then the find at 0x428B60)
	for (std::string::size_type pos = text.find(","); pos != std::string::npos; pos = text.find(",", start))
	{
		std::string name = text.substr(start, pos - start);
		start = pos + 1;
		Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(name);
		if (topping)
			m_newToppings.push_back(engine::RefPtr<Topping>(topping));
	}
}

// 0x428B90
void UserProgress::parsePickedToppings(const std::string& text)
{
	m_pickedToppings.clear();
	std::string::size_type start = 0;
	// the next find runs after each name is destroyed (0x428CC5-0x428CDE, then the find at 0x428CE1)
	for (std::string::size_type pos = text.find(","); pos != std::string::npos; pos = text.find(",", start))
	{
		std::string name = text.substr(start, pos - start);
		start = pos + 1;
		Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(name);
		if (topping)
			m_pickedToppings.push_back(engine::RefPtr<Topping>(topping));
	}
}

// 0x428D30
void UserProgress::addNewTopping(Topping* topping)
{
	if (m_toppingLevels.find(topping) == m_toppingLevels.end() || m_toppingLevels[topping] < 0)
	{
		if (std::find(m_newToppings.begin(), m_newToppings.end(), topping) == m_newToppings.end())
			m_newToppings.push_back(engine::RefPtr<Topping>(topping));
	}
}

// 0x428E00
UserProgress::UserProgress()
{
	reset();
}

// 0x4290C0
bool UserProgress::load()
{
	engine::User* user = engine::UserManager::getInstance()->getCurrentUser();
	if (!user)
		return false;

	reset();
	m_userName = user->getName();
	m_maxCity = user->getAttributes().getInt("maxCity", 0);
	m_unlockTopping = user->getAttributes().getBool("unlockTopping", false);
	parseToppingLevels(user->getAttributes().getString("ToppingLevels", "Cheese=0,Pepperoni=0,"));
	parseNewToppings(user->getAttributes().getString("NewToppings", ""));
	parsePickedToppings(user->getAttributes().getString("PickToppings", ""));
	std::string name;
	for (int i = 0; i < 3; i++)
	{
		engine::format(name, "mode%dLastLevel", i);
		m_modes[i].m_lastLevel = user->getAttributes().getInt(name, -1);
		engine::format(name, "mode%dLastCity", i);
		m_modes[i].m_lastCity = user->getAttributes().getInt(name, -1);
		engine::format(name, "mode%dHighestDay", i);
		m_modes[i].m_highestDay = user->getAttributes().getInt(name, -1);
		engine::format(name, "mode%dLastDay", i);
		m_modes[i].m_lastDay = user->getAttributes().getInt(name, -1);
		engine::format(name, "mode%dLastPizza", i);
		m_modes[i].m_lastPizza = user->getAttributes().getInt(name, -1);
		engine::format(name, "mode%dLastScore", i);
		m_modes[i].m_lastScore = user->getAttributes().getInt(name, -1);
		engine::format(name, "mode%dLastTopping", i);
		m_modes[i].m_lastTopping = user->getAttributes().getString(name, "");
		engine::format(name, "mode%dElapsedTime", i);
		m_modes[i].m_elapsedTime = user->getAttributes().getFloat(name, 0.0f);
	}
	return true;
}

// 0x429710
void UserProgress::onUserChanged()
{
	load();
}

// 0x429720
bool UserProgress::init()
{
	reset();
	engine::UserManager* manager = engine::UserManager::getInstance();
	if (!manager)
		return false;
	manager->m_changedSignal.connect(this, &UserProgress::onUserChanged);
	load();
	return true;
}
