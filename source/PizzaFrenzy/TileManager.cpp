// TileManager, the tile manifest's element classes and its XML handlers.
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "engine/Application.h"
#include "engine/Color.h"
#include "engine/Exception.h"
#include "engine/Graphics.h"
#include "engine/HsvFilter.h"
#include "engine/ParticleSystemDef.h"
#include "engine/ParticleSystemLoader.h"
#include "engine/Properties.h"
#include "engine/Range.h"
#include "engine/Rect.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "engine/Xml.h"

#include "Level.h"
#include "MusicTrack.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x41D680
ToppingUpgradeHandler::ToppingUpgradeHandler(Topping* topping, const std::string& name, engine::XmlHandlerStack* parser)
	: engine::XmlElementHandler(name, parser)
	, m_topping(topping)
{
}

// 0x41D7B0
TileType::TileType(engine::Bitmap* image, int layer)
	: m_offset(0, 0)
	, m_base(0, 0)
	, m_image(image)
	, m_layer(layer)
	, m_dimensions(1, 1)
{
	m_image->setPivotType(1);
}

// 0x41D8C0
KitchenTileType::KitchenTileType(engine::Bitmap* image, const engine::Point& parkingSpot)
	: TileType(image, 3)
	, m_parkingSpot(parkingSpot)
{
}

// 0x41D990
BridgeTileType::BridgeTileType(engine::Bitmap* image, engine::Bitmap* roadImage, engine::Bitmap* unblockedImage,
	engine::Bitmap* unblockedRoadImage)
	: TileType(image, 3)
	, m_roadImage(roadImage)
	, m_unblockedImage(unblockedImage)
	, m_unblockedRoadImage(unblockedRoadImage)
{
	m_roadImage->setPivotType(1);
	m_unblockedImage->setPivotType(1);
	m_unblockedRoadImage->setPivotType(1);
}

// 0x41DB90
void VehicleType::setParkedImages(const std::string& parkedPath, const std::string& hoverPath)
{
	m_parkedImage = engine::getApplication()->getImage(parkedPath.c_str());
	m_parkedImage->setPivotType(1);
	m_hoverImage = engine::getApplication()->getImage(hoverPath.c_str());
	m_hoverImage->setPivotType(1);
}

// 0x41DC60
void Topping::setImage(engine::Bitmap* image)
{
	m_image = image;
	m_image->setPivotType(1);
	if (!m_isPizza)
	{
		m_smallImage = m_image->copyScaledUniform(0.55f);
		engine::Bitmap* cheesePizza = engine::getApplication()->getImage("res\\pizza\\cheesePizza.jpg");
		m_pizzaImage = cheesePizza->copy();
		engine::RefPtr<engine::Graphics> graphics = engine::getApplication()->createGraphics(m_pizzaImage);
		graphics->drawImage(m_smallImage, cheesePizza->getWidth() * 0.5f, cheesePizza->getHeight() * 0.5f - 3.0f);
		m_pizzaImage->setPivotType(1);
	}
	else
	{
		m_smallImage = m_image->copyScaledUniform(0.65f);
		m_pizzaImage = m_image;
	}
	m_smallImage->setPivotType(1);
}

// 0x41DE80
ParticleFx::ParticleFx(const std::string& name, const std::string& fxPath, int layer)
{
	engine::ParticleSystemLoader loader;
	try
	{
		engine::getApplication()->loadXml(fxPath, &loader);
	}
	catch (...)
	{
		engine::getApplication()->log("WARNING: Tile Manager unable to load effect %s\n", fxPath.c_str());
	}
	m_effect = loader.getDefinition();
	m_effect->loadFrames();
	m_layer = layer;
}

// 0x41E120
Title* TileManager::getTitle(int points) const
{
	Title* title = m_titles[0];
	for (std::vector<engine::RefPtr<Title> >::const_iterator it = m_titles.begin() + 1; it != m_titles.end(); ++it)
	{
		if (points < (*it)->m_points)
			break;
		title = *it;
	}
	return title;
}

// 0x41E600
const std::string& TileManager::getRandomFunFact() const
{
	return m_funFacts[engine::randomInt(0, m_funFacts.size() - 1)];
}

// 0x41EA20
CustomerTileType::CustomerTileType(engine::Bitmap* image, const engine::Point& parkingSpot, const std::string& character)
	: TileType(image, 3)
	, m_parkingSpot(parkingSpot)
	, m_character(character)
{
}

// 0x41EB20
VehicleType::VehicleType(engine::Bitmap* northImage, engine::Bitmap* southImage, engine::Bitmap* eastWestImage,
	const std::string& description, const std::string& vehicleClass, const std::string& fx)
	: TileType(eastWestImage, 3)
	, m_description(description)
	, m_fx(fx)
	, m_class(vehicleClass)
	, m_northImage(northImage)
	, m_southImage(southImage)
{
	m_northImage->setPivotType(1);
	m_southImage->setPivotType(1);
}

// 0x41EE20
Character::Character(engine::Bitmap* image, int numPizzas, float patience, float generosity, const std::string& name,
	const std::string& greeting)
	: m_image(image)
	, m_numPizzas(numPizzas)
	, m_patience(patience)
	, m_generosity(generosity)
	, m_name(name)
	, m_greeting(greeting)
	, m_special(0)
{
	m_image->setPivotType(1);
}

// 0x41F000
Title::Title(const std::string& text, int points)
	: m_text(text)
	, m_points(points)
{
}

// 0x41F470
ToppingUpgrade::ToppingUpgrade(const std::string& name, const std::string& description, int bonusMultiplier, int combo)
{
	m_name = name;
	m_description = description;
	m_bonusMultiplier = bonusMultiplier;
	m_combo = combo;
}

// 0x41F5E0
SpecialEvent::SpecialEvent(const std::string& character, const std::string& name, const std::string& type,
	const std::string& portrait, const std::string& sound, const std::string& description, bool deliver)
{
	m_character = character;
	m_portrait = engine::getApplication()->getImage(portrait.c_str());
	m_portrait->setPivotType(1);
	m_type = type;
	m_name = name;
	m_sound = sound;
	m_description = description;
	m_deliver = deliver;
}

// 0x41F8C0
bool TileManager::loadManifest(std::string path)
{
	engine::RefPtr<engine::XmlHandlerStack> stack = new engine::XmlHandlerStack();
	engine::RefPtr<TileManifestHandler> handler = new TileManifestHandler(this, "TileManifest", stack);
	try
	{
		stack->pushHandler(handler);
		engine::getApplication()->loadXml(path, stack);
	}
	catch (...)
	{
		throw engine::Exception("Error parsing tiles manifest\n");
	}
	return true;
}

// 0x420A90
Topping::Topping(const std::string& name, const std::string& display, engine::Bitmap* image,
	engine::Bitmap* couponImage, bool isPizza)
{
	m_couponImage = couponImage;
	m_couponImage->setPivotType(1);
	m_name = name;
	m_display = display;
	m_isPizza = isPizza;
	setImage(image);
}

// 0x421970
void TileManager::addTitle(const std::string& text, int points)
{
	m_titles.push_back(new Title(text, points));
}

// 0x421A30
void ToppingUpgradeHandler::startElement(const std::string& name, const engine::Properties& attributes)
{
	if (name == "upgrade")
	{
		std::string upgradeName = attributes.getString("name", "");
		std::string description = attributes.getString("description", "");
		int bonusMultiplier = attributes.getInt("bonusMultiplier", 0);
		int combo = attributes.getInt("combo", 0);
		m_topping->m_upgrades.push_back(new ToppingUpgrade(upgradeName, description, bonusMultiplier, combo));
	}
}

// 0x422590
void TileManager::registerCharacter(const std::string& name, Character* character)
{
	if (m_characters[name] == 0)
		m_characters[name] = character;
	else
		throw engine::Exception("registering character which already exists!");
}

// 0x422660
void TileManager::registerTopping(const std::string& name, Topping* topping)
{
	if (m_toppings[name] == 0)
	{
		m_toppings[name] = topping;
		m_toppingList.push_back(topping);
	}
	else
	{
		throw engine::Exception("registering topping which already exists!");
	}
}

// 0x422780
void TileManager::registerFx(const std::string& name, ParticleFx* fx)
{
	if (m_particleFx[name] == 0)
		m_particleFx[name] = fx;
	else
		throw engine::Exception("registering fx which already exists!");
}

// 0x422850
void TileManager::registerEvent(const std::string& name, SpecialEvent* event)
{
	if (m_events[name] == 0)
		m_events[name] = event;
	else
		throw engine::Exception("registering event which already exists!");
}

// 0x422920
void TileManager::registerMusicClip(const std::string& name, MusicTrack* clip)
{
	if (m_music[name] == 0)
		m_music[name] = clip;
	else
		throw engine::Exception("registering music clip which already exists!");
}

// 0x4229F0
void TileManager::registerRoadTiles(const std::string& name, const std::string& path)
{
	if (m_roadTiles[name] == 0)
	{
		engine::RefPtr<TileType>* tiles = new engine::RefPtr<TileType>[16];
		m_roadTiles[name] = tiles;
		engine::Bitmap* image = engine::getApplication()->getImage(path.c_str());
		for (int i = 0; i < 16; i++)
		{
			// the original divides unsigned (div) while its loop test is signed (jl)
			engine::IntRect source(25 * (i / 4u), 25 * (i % 4u), 25 * (i / 4u) + 25, 25 * (i % 4u) + 25);
			engine::Bitmap* piece = engine::getApplication()->createBlankImage(25, 25);
			piece->fill(engine::Color(0, 0, 0, 0));
			piece->setAlphaType(2, 255);
			engine::RefPtr<engine::Graphics> graphics = engine::getApplication()->createGraphics(piece);
			graphics->drawImage(image, source, 0.0f, 0.0f);
			tiles[i] = new TileType(piece, 3);
		}
	}
}

// 0x422C00
TileType* TileManager::getTile(const std::string& name, const std::string& className)
{
	if (className == "roads")
	{
		if (m_roadTiles.find(name) != m_roadTiles.end())
		{
			engine::RefPtr<TileType>* tiles = m_roadTiles[name];
			if (tiles)
				return tiles[10];
		}
	}
	else
	{
		std::map<std::string, engine::RefPtr<TileType> >* tiles = m_tiles[className];
		if (tiles && tiles->find(name) != tiles->end())
			return (*tiles)[name];
	}
	return 0;
}

// 0x422CA0
VehicleType* TileManager::getVehicleType(const std::string& name)
{
	return static_cast<VehicleType*>(getTile(name, "vehicle"));
}

// 0x422D30
TileType* TileManager::getRoadTile(const std::string& set, int connections)
{
	engine::RefPtr<TileType>* tiles = m_roadTiles[set];
	if (tiles && connections <= 15)
		return tiles[connections];
	return 0;
}

// 0x422D60
Character* TileManager::getCharacter(const std::string& name)
{
	return m_characters[name];
}

// 0x422D80
Topping* TileManager::getTopping(const std::string& name)
{
	if (name.empty())
		return 0;
	if (m_toppings.find(name) == m_toppings.end())
		return 0;
	return m_toppings[name];
}

// 0x422DD0
ParticleFx* TileManager::getFx(const std::string& name)
{
	return m_particleFx[name];
}

// 0x422DF0
SpecialEvent* TileManager::getEvent(const std::string& name)
{
	return m_events[name];
}

// 0x422E10
MusicTrack* TileManager::getMusicClip(const std::string& name)
{
	return m_music[name];
}

// 0x422E60
void TileManager::registerTile(const std::string& name, const std::string& className, TileType* type)
{
	std::map<std::string, engine::RefPtr<TileType> >* tiles = m_tiles[className];
	if (tiles == 0)
	{
		tiles = new std::map<std::string, engine::RefPtr<TileType> >;
		m_tiles[className] = tiles;
	}
	if ((*tiles)[name] == 0)
		(*tiles)[name] = type;
	else
		throw engine::Exception("registering tile which already exists");
}

// 0x422F90
Character* TileManager::getThemedCharacter(const std::string& name)
{
	std::string theme = getGame()->getCurrentCity()->getTheme();
	std::string key;
	engine::format(key, "%s_%s", name.c_str(), theme.c_str());
	return m_characters[key];
}

// 0x423090
TileManager::TileManager()
{
}

// 0x423280
TileManager::~TileManager()
{
	for (std::map<std::string, std::map<std::string, engine::RefPtr<TileType> >*>::iterator it = m_tiles.begin();
		it != m_tiles.end(); ++it)
	{
		it->second->clear();
		delete it->second;
	}
	m_tiles.clear();
	for (std::map<std::string, engine::RefPtr<TileType>*>::iterator it = m_roadTiles.begin(); it != m_roadTiles.end();
		++it)
	{
		delete[] it->second;
	}
	m_roadTiles.clear();
	m_characters.clear();
	m_toppings.clear();
	m_toppingList.clear();
	m_particleFx.clear();
	m_events.clear();
}

// 0x423980
void TileManager::addDecoration(const std::string& name, const std::string& type, const std::string& path, int layer,
	const std::string& hsvShift)
{
	// uninitialised as in the original (read only if sscanf fills fewer than three; every hsvShift in the
	// manifest, and the "0,0,0" default, has three numbers)
	float hue;
	float saturation;
	float value;
	sscanf(hsvShift.c_str(), "%f,%f,%f", &hue, &saturation, &value);
	engine::Bitmap* image = engine::getApplication()->getImage(path.c_str());
	TileType* tile;
	if (hue == 0.0f && saturation == 0.0f && value == 0.0f)
	{
		tile = new TileType(image, layer);
	}
	else
	{
		engine::HsvFilter filter;
		filter.set(hue, saturation, value);
		image->setPivotType(0);
		engine::Bitmap* shifted = engine::getApplication()->createBlankImage(image->getWidth(), image->getHeight());
		engine::RefPtr<engine::Graphics> graphics = engine::getApplication()->createGraphics(shifted);
		shifted->fill(engine::Color(0.0f, 0.0f, 0.0f, 0.0f));
		shifted->setAlphaType(2, 255);
		graphics->drawImage(image);
		shifted->applyFilter(&filter);
		image->setPivotType(1);
		tile = new TileType(shifted, layer);
	}
	registerTile(name, type, tile);
}

// 0x423BD0
void TileManifestHandler::startElement(const std::string& name, const engine::Properties& attributes)
{
	if (name == "tile")
	{
		std::string tileName = attributes.getString("name", "");
		std::string path = attributes.getString("path", "");
		std::string tileClass = attributes.getString("class", "");
		std::string hsvShift = attributes.getString("hsvShift", "0,0,0");
		engine::Bitmap* image = engine::getApplication()->getImage(path.c_str());
		engine::Point offset = attributes.getIntPoint("BROffset", engine::Point(0, 0));
		engine::Point base = attributes.getIntPoint("base", engine::Point(0, 0));
		engine::Point parkingSpot = attributes.getIntPoint("parkingspot", engine::Point(0, 0));
		engine::Point dimensions = attributes.getIntPoint("dimensions", engine::Point(1, 1));
		TileType* tile;
		if (tileClass == "kitchen")
		{
			tile = new KitchenTileType(image, parkingSpot);
		}
		else if (tileClass == "customer")
		{
			std::string character = attributes.getString("character", "");
			tile = new CustomerTileType(image, parkingSpot, character);
		}
		else if (tileClass == "bridge")
		{
			std::string roadPath = attributes.getString("roadPath", "");
			engine::Bitmap* roadImage = engine::getApplication()->getImage(roadPath.c_str());
			std::string unblockedPath = attributes.getString("unblockedPath", "");
			engine::Bitmap* unblockedImage = engine::getApplication()->getImage(unblockedPath.c_str());
			std::string unblockedRoadPath = attributes.getString("unblockedRoadPath", "");
			engine::Bitmap* unblockedRoadImage = engine::getApplication()->getImage(unblockedRoadPath.c_str());
			tile = new BridgeTileType(image, roadImage, unblockedImage, unblockedRoadImage);
		}
		else
		{
			throw engine::Exception("Error: invalid tile type");
		}
		tile->m_offset = offset;
		tile->m_base = base;
		tile->m_dimensions = dimensions;
		m_manifest->registerTile(tileName, tileClass, tile);
	}
	else if (name == "decoration")
	{
		std::string decorationName = attributes.getString("name", "");
		std::string path = attributes.getString("path", "");
		std::string hsvShift = attributes.getString("hsvShift", "0,0,0");
		int layer = attributes.getInt("layer", 3);
		m_manifest->addDecoration(decorationName, "decoration", path, layer, hsvShift);
	}
	else if (name == "roadTiles")
	{
		std::string setName = attributes.getString("name", "");
		std::string path = attributes.getString("path", "");
		m_manifest->registerRoadTiles(setName, path);
	}
	else if (name == "vehicle")
	{
		std::string vehicleName = attributes.getString("name", "");
		std::string northPath = attributes.getString("Npath", "");
		engine::Bitmap* northImage = engine::getApplication()->getImage(northPath.c_str());
		std::string southPath = attributes.getString("Spath", "");
		engine::Bitmap* southImage = engine::getApplication()->getImage(southPath.c_str());
		std::string eastWestPath = attributes.getString("EWpath", "");
		engine::Bitmap* eastWestImage = engine::getApplication()->getImage(eastWestPath.c_str());
		std::string vehicleClass = attributes.getString("class", "");
		std::string fx = attributes.getString("fx", "");
		int speed = attributes.getInt("speed", 0);
		std::string description = attributes.getString("description", "");
		std::string parkedPath = attributes.getString("ParkedPath", "");
		std::string hoverPath = attributes.getString("HoverPath", "");
		VehicleType* vehicle = new VehicleType(northImage, southImage, eastWestImage, description, vehicleClass, fx);
		vehicle->m_speed = speed;
		if (!parkedPath.empty())
			vehicle->setParkedImages(parkedPath, hoverPath);
		m_manifest->registerTile(vehicleName, "vehicle", vehicle);
	}
	else if (name == "character")
	{
		std::string characterName = attributes.getString("name", "");
		std::string theme = attributes.getString("theme", "");
		std::string path = attributes.getString("path", "");
		engine::Bitmap* image = engine::getApplication()->getImage(path.c_str());
		int numPizzas = attributes.getInt("numPizzas", 0);
		float patience = attributes.getFloat("patience", 0.0f);
		float generosity = attributes.getFloat("generosity", 0.0f);
		std::string greeting = attributes.getString("greeting", "");
		std::string special = attributes.getString("special", "");
		Character* character = new Character(image, numPizzas, patience, generosity, characterName, greeting);
		if (special == "chameleon")
			character->m_special = 1;
		else if (special == "police")
			character->m_special = 2;
		characterName.push_back('_');
		characterName.insert(characterName.end(), theme.begin(), theme.end());
		m_manifest->registerCharacter(characterName, character);
	}
	else if (name == "topping")
	{
		std::string toppingName = attributes.getString("name", "");
		std::string couponPath = attributes.getString("couponPath", "");
		std::string path = attributes.getString("path", "");
		std::string display = attributes.getString("display", "");
		engine::Bitmap* image = engine::getApplication()->getImage(path.c_str());
		engine::Bitmap* couponImage = engine::getApplication()->getImage(couponPath.c_str());
		Topping* topping = new Topping(toppingName, display, image, couponImage, false);
		m_manifest->registerTopping(toppingName, topping);
		m_handlerStack->pushHandler(new ToppingUpgradeHandler(topping, "topping", m_handlerStack));
	}
	else if (name == "particleFx")
	{
		std::string fxName = attributes.getString("name", "");
		std::string fxPath = attributes.getString("fxPath", "");
		int layer = attributes.getInt("layer", 4);
		m_manifest->registerFx(fxName, new ParticleFx(fxName, fxPath, layer));
	}
	else if (name == "event")
	{
		std::string eventName = attributes.getString("name", "");
		std::string character = attributes.getString("character", "");
		std::string portrait = attributes.getString("portrait", "");
		std::string type = attributes.getString("type", "");
		std::string sound = attributes.getString("sound", "");
		std::string description = attributes.getString("description", "");
		std::string deliverText = attributes.getString("deliver", "yes");
		bool deliver = true;
		if (deliverText == "no")
			deliver = false;
		m_manifest->registerEvent(eventName,
			new SpecialEvent(character, eventName, type, portrait, sound, description, deliver));
	}
	else if (name == "funfact")
	{
		std::string text = attributes.getString("text", "");
		m_manifest->m_funFacts.push_back(text);
	}
	else if (name == "title")
	{
		std::string text = attributes.getString("text", "");
		int points = attributes.getInt("points", 0);
		m_manifest->addTitle(text, points);
	}
	else if (name == "music")
	{
		std::string clipName = attributes.getString("name", "");
		std::string file = attributes.getString("file", "");
		float volume = attributes.getFloat("vol", 1.0f);
		float fadeInTime = attributes.getFloat("fadeInTime", 0.1f);
		float fadeOutTime = attributes.getFloat("fadeOutTime", 0.2f);
		std::string loopText = attributes.getString("loop", "true");
		bool loop = attributes.getBool("loop", true);
		int category = attributes.getInt("category", 0);
		m_manifest->registerMusicClip(clipName,
			new MusicTrack(file, fadeInTime, fadeOutTime, volume, loop, category));
	}
}
