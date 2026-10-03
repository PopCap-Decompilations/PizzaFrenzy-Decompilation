#include "Level.h"

#include "engine/Properties.h"
#include "engine/Xml.h"

#include "BoatSpawner.h"
#include "CityMap.h"
#include "KitchenTile.h"
#include "PizzaDesign.h"
#include "PizzaFrenzy.h"
#include "SpecialEventTrigger.h"
#include "TileManager.h"

// 0x414B80
LevelManifestHandler::LevelManifestHandler(const std::string& elementName, engine::XmlHandlerStack* parser)
	: engine::XmlElementHandler(elementName, parser)
{
}

// 0x414C90
LevelHandler::LevelHandler(const std::string& elementName, engine::XmlHandlerStack* parser, City* city)
	: engine::XmlElementHandler(elementName, parser)
{
	m_city = city;
}

// 0x414E40
void LevelManifestHandler::endElement(const std::string& name)
{
	if (name == "city")
		m_city = NULL;
}

// 0x414E80
void LevelHandler::endElement(const std::string& name)
{
	if (name == "level")
		m_level = NULL;
}

// 0x414F50
void LevelHandler::startElement(const std::string& name, const engine::Properties& attrs)
{
	std::string itemName = attrs.getString("name", "");
	if (name == "level")
	{
		std::string cityFile = attrs.getString("city", "");
		std::string levelName = attrs.getString("name", "");
		std::string music = attrs.getString("music", "");
		std::string storySpeed = attrs.getString("storySpeed", "");
		std::string storyMemory = attrs.getString("storyMemory", "");
		std::string storyConcentration = attrs.getString("storyConcentration", "");
		std::string story = attrs.getString("story", "");
		if (!story.empty())
			storySpeed = storyMemory = storyConcentration = story;
		int goal = attrs.getInt("goal", 0);
		int numOrders = attrs.getInt("numOrders", 0);
		float waveGap = attrs.getFloat("waveGap", 3.0f);
		int time = attrs.getInt("time", 0);
		std::string deliverOnly = attrs.getString("deliverOnly", "");
		std::string type = attrs.getString("type", "");
		if (type == "bonus")
		{
			m_level = new Level("", "", music, 0, time, 0.0f);
			m_level->m_bonus = true;
		}
		else
		{
			m_level = new Level(cityFile, levelName, music, goal, numOrders, waveGap);
		}
		if (!storySpeed.empty())
			m_level->setStory(storySpeed, 0);
		if (!storyMemory.empty())
			m_level->setStory(storyMemory, 1);
		if (!storyConcentration.empty())
			m_level->setStory(storyConcentration, 2);
		if (!deliverOnly.empty())
			m_level->m_deliverOnly = deliverOnly;
		m_city->addLevel(m_level);
	}
	else if (name == "topping")
	{
		m_level->addTopping(PizzaFrenzy::getTileManifest()->getTopping(itemName));
	}
	else if (name == "pizza")
	{
		int pizzaId = attrs.getInt("pizzaID", 0);
		m_level->addPizza(getGame()->getPizza(pizzaId));
	}
	else if (name == "kitchen")
	{
		std::string vanType = attrs.getString("vanType", "");
		m_level->addKitchen(vanType);
	}
	else if (name == "boat")
	{
		engine::Point pos = attrs.getIntPoint("pos", engine::Point(0, 0));
		engine::Point direction = attrs.getIntPoint("direction", engine::Point(0, 1));
		engine::Range delay = attrs.getRange("delay", engine::Range(10.0f, 20.0f));
		m_level->addBoat(itemName, pos, delay, direction);
	}
	else if (name == "event")
	{
		engine::IntRange delay = attrs.getIntRange("delay", engine::IntRange(2, 10));
		bool firstWave = attrs.getBool("firstWave", false);
		m_level->addEvent(itemName, delay, firstWave);
	}
	else if (name == "wave")
	{
		engine::IntRange waveSize = attrs.getIntRange("waveSize", engine::IntRange(1, 1));
		engine::Range spawnDelay = attrs.getRange("spawnDelay", engine::Range(1.0f, 1.0f));
		engine::Range waitTime = attrs.getRange("waitTime", engine::Range(10.0f, 10.0f));
		m_level->addWave(waveSize, spawnDelay, waitTime);
	}
}

// 0x4160F0
City::City()
{
}

// 0x416290
void LevelManifestHandler::startElement(const std::string& name, const engine::Properties& attrs)
{
	if (name == "city")
	{
		std::string cityName = attrs.getString("name", "No Name");
		std::string theme = attrs.getString("theme", "Urban");
		m_city = new City();
		m_city->m_name = cityName;
		m_city->m_theme = theme;
		getGame()->addCity(m_city);
		m_handlerStack->pushHandler(new LevelHandler(name, m_handlerStack, m_city));
	}
}

// 0x4165B0
std::vector<engine::RefPtr<Topping> >& Level::getToppings()
{
	return m_toppings;
}

// 0x4165C0
std::vector<engine::RefPtr<PizzaDesign> >& Level::getPizzas()
{
	return m_pizzas;
}

// 0x4165D0
std::vector<KitchenInfo*>& Level::getKitchens()
{
	return m_kitchens;
}

// 0x4165E0
std::vector<WaveInfo*>& Level::getWaves()
{
	return m_waves;
}

// 0x4165F0
Character* Level::getDeliverOnlyCharacter() const
{
	if (!m_deliverOnly.empty())
		return PizzaFrenzy::getTileManifest()->getCharacter(m_deliverOnly);
	return NULL;
}

// 0x416730
void Level::setupKitchens(CityMap* city) const
{
	for (std::vector<engine::RefPtr<KitchenTile> >::iterator it = city->m_kitchens.begin(); it != city->m_kitchens.end(); ++it)
		(*it)->setVisible(false);
	std::vector<engine::RefPtr<KitchenTile> >::iterator tile = city->m_kitchens.begin();
	for (std::vector<KitchenInfo*>::const_iterator it = m_kitchens.begin(); it != m_kitchens.end() && tile != city->m_kitchens.end(); ++it, ++tile)
	{
		KitchenTile* kitchen = *tile;
		kitchen->removePopup();
		if ((*it)->topping)
		{
			kitchen->setTopping((*it)->topping);
			(*it)->topping = NULL;
		}
		kitchen->setVehicleType((*it)->vanType);
		kitchen->removeAllVehicles();
	}
}

// 0x4167D0
void Level::createBoatSpawners(CityMap* city) const
{
	for (std::vector<BoatInfo*>::const_iterator it = m_boats.begin(); it != m_boats.end(); ++it)
	{
		BoatSpawner* spawner = new BoatSpawner(city, *it);
		if (spawner)
			city->addBoatSpawner(spawner);
	}
}

// 0x416900
BoatInfo::BoatInfo()
{
}

// 0x416960
EventInfo::EventInfo()
{
}

// 0x416A10
void Level::setStory(const std::string& story, int mode)
{
	m_stories[mode] = story;
}

// 0x417200
Level::~Level()
{
	for (std::vector<KitchenInfo*>::iterator it = m_kitchens.begin(); it != m_kitchens.end(); ++it)
		delete *it;
	m_kitchens.clear();
	for (std::vector<BoatInfo*>::iterator it = m_boats.begin(); it != m_boats.end(); ++it)
		delete *it;
	m_boats.clear();
	for (std::vector<EventInfo*>::iterator it = m_events.begin(); it != m_events.end(); ++it)
		delete *it;
	m_events.clear();
	for (std::vector<WaveInfo*>::iterator it = m_waves.begin(); it != m_waves.end(); ++it)
		delete *it;
	m_waves.clear();
}

// 0x417620
Level::Level(const std::string& cityFile, const std::string& name, const std::string& music, int goal, int numOrders, float waveGap)
	: m_cityFile(cityFile)
	, m_music(music)
	, m_name(name)
	, m_bonus(false)
	, m_goal(goal)
	, m_waveGap(waveGap)
	, m_numOrders(numOrders)
{
}

// 0x417830
void Level::addTopping(Topping* topping)
{
	m_toppings.push_back(topping);
}

// 0x4178B0
void Level::addPizza(PizzaDesign* pizza)
{
	m_pizzas.push_back(pizza);
}

// 0x417930
void Level::addKitchen(const std::string& vanType)
{
	KitchenInfo* kitchen = new KitchenInfo;
	kitchen->topping = NULL;
	kitchen->vanType = vanType;
	m_kitchens.push_back(kitchen);
}

// 0x4179D0
void Level::addBoat(const std::string& name, engine::Point pos, const engine::Range& delay, engine::Point direction)
{
	BoatInfo* boat = new BoatInfo();
	boat->name = name;
	boat->pos = pos;
	boat->direction = direction;
	boat->delay = delay;
	m_boats.push_back(boat);
}

// 0x417AD0
void Level::addEvent(const std::string& name, const engine::IntRange& delay, bool firstWave)
{
	EventInfo* event = new EventInfo();
	event->name = name;
	event->delay = delay;
	event->firstWave = firstWave;
	m_events.push_back(event);
}

// 0x417BB0
void Level::addWave(const engine::IntRange& waveSize, const engine::Range& spawnDelay, const engine::Range& waitTime)
{
	WaveInfo* wave = new WaveInfo;
	wave->waveSize = waveSize;
	wave->spawnDelay = spawnDelay;
	wave->waitTime = waitTime;
	m_waves.push_back(wave);
}

// 0x417CB0
void Level::createEvents(std::vector<engine::RefPtr<SpecialEventTrigger> >& events) const
{
	for (std::vector<EventInfo*>::const_iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		SpecialEventTrigger* trigger = new SpecialEventTrigger(*it);
		if (trigger)
			events.push_back(trigger);
	}
}

// 0x417DF0
void City::addLevel(Level* level)
{
	m_levels.push_back(level);
}
