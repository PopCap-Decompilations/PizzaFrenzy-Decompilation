// Level and City (the level manifest's days and cities), their records and the XML handlers that read them.
#pragma once

#include <string>
#include <vector>

#include "engine/Object.h"
#include "engine/Point.h"
#include "engine/Range.h"
#include "engine/RefPtr.h"
#include "engine/XmlElementHandler.h"

namespace engine
{
	class Properties;
	class XmlHandlerStack;
}

class Character;
class CityMap;
class PizzaDesign;
class SpecialEventTrigger;
class Topping;

// A <kitchen vanType> of a level (Level::m_kitchens, owned; implicit constructor, inlined by addKitchen). The
// topping is chosen before the level (GameProgress, ToppingSelectionScreen) and handed to the kitchen by
// Level::setupKitchens.
struct KitchenInfo
{
	Topping* topping;						// +0x00 NULL at load; setupKitchens gives it to the kitchen and clears it
	std::string vanType;					// +0x04 "vanType"
	int unused;								// +0x20 never read or written
};

// A <boat name pos delay direction> of a level: the route of a BoatSpawner (Level::m_boats, owned).
struct BoatInfo
{
	BoatInfo();

	std::string name;						// +0x00 "name"
	engine::Point pos;						// +0x1C "pos" (0,0)
	engine::Range delay;					// +0x24 "delay" (10,20): seconds between boats
	engine::Point direction;				// +0x2C "direction" (0,1)
};

// An <event name delay firstWave> of a level, run by a SpecialEventTrigger (Level::m_events, owned).
struct EventInfo
{
	EventInfo();

	std::string name;						// +0x00 "name"
	engine::IntRange delay;					// +0x1C "delay" (2,10): waves before it fires
	bool firstWave;							// +0x24 "firstWave"
};

// A <wave waveSize spawnDelay waitTime> of a level (Level::m_waves, owned; implicit constructor, inlined by
// addWave).
struct WaveInfo
{
	engine::IntRange waveSize;				// +0x00 "waveSize" (1,1): orders in the wave
	engine::Range spawnDelay;				// +0x08 "spawnDelay" (1,1): seconds between its orders
	engine::Range waitTime;					// +0x10 "waitTime" (10,10)
};

// A <level> of the level manifest (a day of a city): city file, music, name, goal, number of orders (the time of a
// bonus level), wave gap, a story per game mode, the bonus flag, and the level's toppings, bonus pizzas, kitchens,
// boats, special events and waves. Created by LevelHandler. The members end at +0x140; the vtordisp (+0x140) and the
// Interface subobject (+0x144) follow them (0x148 bytes).
class Level : public engine::Object
{
public:
	Level(const std::string& cityFile, const std::string& name, const std::string& music, int goal, int numOrders, float waveGap);
	virtual ~Level();

	// 0x4041E0 (folded)
	std::string getCityFile() const
	{
		return m_cityFile;
	}

	// 0x404210
	std::string getStoryFile(int mode) const
	{
		return m_stories[mode];
	}

	std::vector<engine::RefPtr<Topping> >& getToppings();
	std::vector<engine::RefPtr<PizzaDesign> >& getPizzas();
	std::vector<KitchenInfo*>& getKitchens();
	std::vector<WaveInfo*>& getWaves();
	Character* getDeliverOnlyCharacter() const;
	void setupKitchens(CityMap* city) const;
	void createBoatSpawners(CityMap* city) const;
	void setStory(const std::string& story, int mode);
	void addTopping(Topping* topping);
	void addPizza(PizzaDesign* pizza);
	void addKitchen(const std::string& vanType);
	void addBoat(const std::string& name, engine::Point pos, const engine::Range& delay, engine::Point direction);
	void addEvent(const std::string& name, const engine::IntRange& delay, bool firstWave);
	void addWave(const engine::IntRange& waveSize, const engine::Range& spawnDelay, const engine::Range& waitTime);
	void createEvents(std::vector<engine::RefPtr<SpecialEventTrigger> >& events) const;

	// 0x4578A0
	std::string getMusic() const
	{
		return m_music;
	}

	// 0x4578D0
	std::string getName() const
	{
		return m_name;
	}

	std::string m_cityFile;									// +0x0C "city": res\cities\*.xml ("" for bonus levels)
	std::string m_music;									// +0x28 "music"
	std::string m_name;										// +0x44 "name"
	bool m_bonus;											// +0x60 type="bonus"
	std::string m_stories[3];								// +0x64 per game mode: storySpeed, storyMemory, storyConcentration
	int m_goal;												// +0xB8 "goal"
	float m_waveGap;										// +0xBC "waveGap" (3.0)
	int m_numOrders;										// +0xC0 "numOrders" ("time" for bonus levels)
	std::vector<engine::RefPtr<Topping> > m_toppings;		// +0xC4 <topping name>
	std::vector<engine::RefPtr<PizzaDesign> > m_pizzas;		// +0xD4 <pizza pizzaID> (bonus levels)
	std::vector<KitchenInfo*> m_kitchens;					// +0xE4 owned
	std::vector<BoatInfo*> m_boats;							// +0xF4 owned
	std::vector<EventInfo*> m_events;						// +0x104 owned
	std::vector<WaveInfo*> m_waves;							// +0x114 owned
	std::string m_deliverOnly;								// +0x124 "deliverOnly": a character name
};

// A <city name theme> of the level manifest and its levels, in manifest order (the game's city list). Implicit
// destructor (0x4161D0 stores no vtables). The members end at +0x54; the vtordisp (+0x54) and the Interface
// subobject (+0x58) follow them (0x5C bytes).
class City : public engine::Object
{
public:
	City();

	// 0x404250 (folded)
	std::string getName() const
	{
		return m_name;
	}

	void addLevel(Level* level);

	// 0x41F120
	std::string getTheme() const
	{
		return m_theme;
	}

	std::vector<engine::RefPtr<Level> > m_levels;			// +0x0C
	std::string m_name;										// +0x1C "name" ("No Name")
	std::string m_theme;									// +0x38 "theme": Urban, Tiki, Snow, Space ("Urban")
};

// The handler of the level manifest's <LevelManifest>: for each <city>, a new City for the game (addCity) and a
// LevelHandler for its contents. Implicit destructor (0x414C30). The members end at +0x38; the vtordisp (+0x38) and
// the Interface subobject (+0x3C) follow them (0x40 bytes).
class LevelManifestHandler : public engine::XmlElementHandler
{
public:
	LevelManifestHandler(const std::string& elementName, engine::XmlHandlerStack* parser);

	virtual void startElement(const std::string& name, const engine::Properties& attrs);	// slot 0 (engine::XmlHandler)
	virtual void endElement(const std::string& name);										// slot 1 (engine::XmlHandler)

	engine::RefPtr<City> m_city;							// +0x34 the <city> being read
};

// The handler of a manifest <city>'s contents: <level> and its <topping>, <pizza>, <kitchen>, <boat>, <event> and
// <wave>. Implicit destructor (0x414DC0). The members end at +0x3C; the vtordisp (+0x3C) and the Interface
// subobject (+0x40) follow them (0x44 bytes).
class LevelHandler : public engine::XmlElementHandler
{
public:
	LevelHandler(const std::string& elementName, engine::XmlHandlerStack* parser, City* city);

	virtual void startElement(const std::string& name, const engine::Properties& attrs);	// slot 0 (engine::XmlHandler)
	virtual void endElement(const std::string& name);										// slot 1 (engine::XmlHandler)

	engine::RefPtr<Level> m_level;							// +0x34 the current <level>
	engine::RefPtr<City> m_city;							// +0x38 the city receiving the levels
};
