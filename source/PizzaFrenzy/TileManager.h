// TileManager (the registry of res\manifests\tileManifest.xml), its element classes and the manifest's XML handlers.
#pragma once

#include <map>
#include <string>
#include <vector>

#include "engine/Object.h"
#include "engine/Point.h"
#include "engine/RefPtr.h"
#include "engine/XmlElementHandler.h"

namespace engine
{
	class Bitmap;
	class ParticleSystemDef;
	class Properties;
	class XmlHandlerStack;
}

class MusicTrack;

// A <tile> definition (also used as is for road tiles and decorations). Implicit destructor (0x41D610).
class TileType : public engine::Object
{
public:
	TileType(engine::Bitmap* image, int layer);

	engine::Point m_offset;								// +0x0C "BROffset"
	engine::Point m_base;								// +0x14 "base" (footprint size)
	engine::RefPtr<engine::Bitmap> m_image;				// +0x1C "path"
	int m_layer;										// +0x20 map layer (3 for tiles)
	engine::Point m_dimensions;							// +0x24 "dimensions", (1,1) by default
};

// A <tile class="kitchen"> definition. Implicit destructor (0x41D980).
class KitchenTileType : public TileType
{
public:
	KitchenTileType(engine::Bitmap* image, const engine::Point& parkingSpot);

	engine::Point m_parkingSpot;						// +0x2C "parkingspot"
	engine::Point m_unusedPoint;						// +0x34 default-constructed, never used
};

// A <tile class="bridge"> definition. Implicit destructor (0x41DB00).
class BridgeTileType : public TileType
{
public:
	BridgeTileType(engine::Bitmap* image, engine::Bitmap* roadImage, engine::Bitmap* unblockedImage,
		engine::Bitmap* unblockedRoadImage);

	engine::RefPtr<engine::Bitmap> m_roadImage;			// +0x2C "roadPath"
	engine::RefPtr<engine::Bitmap> m_unblockedImage;	// +0x30 "unblockedPath"
	engine::RefPtr<engine::Bitmap> m_unblockedRoadImage;	// +0x34 "unblockedRoadPath"
};

// A <tile class="customer"> definition. Implicit destructor (0x41EAF0).
class CustomerTileType : public TileType
{
public:
	CustomerTileType(engine::Bitmap* image, const engine::Point& parkingSpot, const std::string& character);

	engine::Point m_parkingSpot;						// +0x2C "parkingspot"
	std::string m_character;							// +0x34 "character"
};

// A <vehicle> definition, registered as tile class "vehicle"; the base image is "EWpath". Implicit destructor
// (0x41ED00).
class VehicleType : public TileType
{
public:
	VehicleType(engine::Bitmap* northImage, engine::Bitmap* southImage, engine::Bitmap* eastWestImage,
		const std::string& description, const std::string& vehicleClass, const std::string& fx);

	void setParkedImages(const std::string& parkedPath, const std::string& hoverPath);

	std::string m_description;							// +0x2C "description"
	std::string m_fx;									// +0x48 "fx"
	std::string m_class;								// +0x64 "class": copter, canoe, else a van
	int m_speed;										// +0x80 "speed" (set by the manifest handler)
	engine::RefPtr<engine::Bitmap> m_northImage;		// +0x84 "Npath"
	engine::RefPtr<engine::Bitmap> m_southImage;		// +0x88 "Spath"
	engine::RefPtr<engine::Bitmap> m_parkedImage;		// +0x8C "ParkedPath"
	engine::RefPtr<engine::Bitmap> m_hoverImage;		// +0x90 "HoverPath"
};

// A <particleFx> entry: the particle system read from "fxPath". Implicit destructor (0x41DFF0).
class ParticleFx : public engine::Object
{
public:
	ParticleFx(const std::string& name, const std::string& fxPath, int layer);

	engine::RefPtr<engine::ParticleSystemDef> m_effect;	// +0x0C
	int m_layer;										// +0x10 "layer" (default 4)
};

// A <character> entry (registered as "<name>_<theme>"). Implicit destructor (0x41EF60).
class Character : public engine::Object
{
public:
	Character(engine::Bitmap* image, int numPizzas, float patience, float generosity, const std::string& name,
		const std::string& greeting);

	engine::RefPtr<engine::Bitmap> m_image;				// +0x0C "path"
	int m_numPizzas;									// +0x10 "numPizzas"
	float m_patience;									// +0x14 "patience"
	float m_generosity;									// +0x18 "generosity"
	std::string m_name;									// +0x1C "name"
	std::string m_greeting;								// +0x38 "greeting"
	int m_special;										// +0x54 "special": 0 none, 1 chameleon, 2 police
};

// A <title> entry: the rank title shown from a score. Implicit destructor (its vtable and destructor were
// merged with MusicTrack's: 0x41F0B0, 0x4505A0).
class Title : public engine::Object
{
public:
	Title(const std::string& text, int points);

	std::string m_text;									// +0x0C "text"
	int m_points;										// +0x28 "points": minimum score for the title
};

// An <upgrade> of a topping. Implicit destructor (0x41F590).
class ToppingUpgrade : public engine::Object
{
public:
	ToppingUpgrade(const std::string& name, const std::string& description, int bonusMultiplier, int combo);

	std::string m_name;									// +0x0C "name"
	std::string m_description;							// +0x28 "description"
	int m_bonusMultiplier;								// +0x44 "bonusMultiplier"
	int m_combo;										// +0x48 "combo"
};

// An <event> entry: a special customer event. Implicit destructor (0x41F7C0).
class SpecialEvent : public engine::Object
{
public:
	SpecialEvent(const std::string& character, const std::string& name, const std::string& type,
		const std::string& portrait, const std::string& sound, const std::string& description, bool deliver);

	engine::RefPtr<engine::Bitmap> m_portrait;			// +0x0C "portrait"
	std::string m_type;									// +0x10 "type"
	std::string m_name;									// +0x2C "name"
	std::string m_character;							// +0x48 "character" (themed lookup)
	std::string m_sound;								// +0x64 "sound"
	std::string m_description;							// +0x80 "description"
	bool m_deliver;										// +0x9C "deliver" ("no" gives false)
};

// A <topping> entry, or a user pizza turned into a topping named "pizza%d". Implicit destructor (0x420C10).
class Topping : public engine::Object
{
public:
	Topping(const std::string& name, const std::string& display, engine::Bitmap* image,
		engine::Bitmap* couponImage, bool isPizza);

	void setImage(engine::Bitmap* image);

	engine::RefPtr<engine::Bitmap> m_image;				// +0x0C "path"
	engine::RefPtr<engine::Bitmap> m_couponImage;		// +0x10 "couponPath"
	engine::RefPtr<engine::Bitmap> m_smallImage;		// +0x14 m_image scaled 0.55 (0.65 for pizzas)
	engine::RefPtr<engine::Bitmap> m_pizzaImage;		// +0x18 the topping drawn on cheesePizza.jpg
	std::vector<engine::RefPtr<ToppingUpgrade> > m_upgrades;	// +0x1C indexed by upgrade level
	std::string m_name;									// +0x2C "name" (or "pizza%d")
	std::string m_display;								// +0x48 "display"
	bool m_isPizza;										// +0x64 a user pizza
};

// Handler of a <topping> element's <upgrade> children. Implicit destructor (0x41D670, shared by the XML handler
// classes of the folded vtable 0x4FFC54).
class ToppingUpgradeHandler : public engine::XmlElementHandler
{
public:
	ToppingUpgradeHandler(Topping* topping, const std::string& name, engine::XmlHandlerStack* parser);

	// overrides
	virtual void startElement(const std::string& name, const engine::Properties& attributes);	// engine::XmlHandler slot 0

	Topping* m_topping;									// +0x34 receives the upgrades
};

// The registry: created by the loading state, kept at PizzaFrenzy +0x124.
class TileManager : public engine::Object
{
public:
	TileManager();
	virtual ~TileManager();

	Title* getTitle(int points) const;
	const std::string& getRandomFunFact() const;
	bool loadManifest(std::string path);
	void addTitle(const std::string& text, int points);
	void registerCharacter(const std::string& name, Character* character);
	void registerTopping(const std::string& name, Topping* topping);
	void registerFx(const std::string& name, ParticleFx* fx);
	void registerEvent(const std::string& name, SpecialEvent* event);
	void registerMusicClip(const std::string& name, MusicTrack* clip);
	void registerRoadTiles(const std::string& name, const std::string& path);
	TileType* getTile(const std::string& name, const std::string& className);
	VehicleType* getVehicleType(const std::string& name);
	TileType* getRoadTile(const std::string& set, int connections);
	Character* getCharacter(const std::string& name);
	Topping* getTopping(const std::string& name);
	ParticleFx* getFx(const std::string& name);
	SpecialEvent* getEvent(const std::string& name);
	MusicTrack* getMusicClip(const std::string& name);
	void registerTile(const std::string& name, const std::string& className, TileType* type);
	Character* getThemedCharacter(const std::string& name);
	void addDecoration(const std::string& name, const std::string& type, const std::string& path, int layer,
		const std::string& hsvShift);

	std::map<std::string, std::map<std::string, engine::RefPtr<TileType> >*> m_tiles;	// +0x0C by tile class, heap maps by name
	std::map<std::string, engine::RefPtr<TileType>*> m_roadTiles;	// +0x18 new[]'d arrays of 16 pieces (neighbour mask)
	std::map<std::string, engine::RefPtr<Character> > m_characters;	// +0x24 by "<name>_<theme>"
	std::map<std::string, engine::RefPtr<Topping> > m_toppings;		// +0x30
	std::vector<engine::RefPtr<Topping> > m_toppingList;			// +0x3C in manifest order
	std::map<std::string, engine::RefPtr<ParticleFx> > m_particleFx;	// +0x4C
	std::map<std::string, engine::RefPtr<SpecialEvent> > m_events;	// +0x58
	std::map<std::string, engine::RefPtr<MusicTrack> > m_music;		// +0x64
	std::vector<engine::RefPtr<Title> > m_titles;					// +0x70 ascending points
	std::vector<std::string> m_funFacts;							// +0x80
};

// Root handler of the manifest ("TileManifest"). Its constructor is inlined in TileManager::loadManifest; implicit
// destructor (0x41D670, shared).
class TileManifestHandler : public engine::XmlElementHandler
{
public:
	TileManifestHandler(TileManager* manager, const std::string& name, engine::XmlHandlerStack* parser)
		: engine::XmlElementHandler(name, parser)
		, m_manifest(manager)
	{
	}

	// overrides
	virtual void startElement(const std::string& name, const engine::Properties& attributes);	// engine::XmlHandler slot 0

	TileManager* m_manifest;							// +0x34 the manager being filled
};
