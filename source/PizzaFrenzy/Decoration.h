// Decoration: a <decoration tile pos> of a city file.
#pragma once

#include <string>

#include "engine/Object.h"
#include "engine/Point.h"
#include "engine/RefPtr.h"

namespace engine
{
	class Bitmap;
	class Component;
	class Image;
	class ParticleSystem;
	class Properties;
	class XmlWriter;
}

class CityMap;

// An engine::Image (a tile-manifest <decoration>) or an engine::ParticleSystem (a tile-manifest <particleFx>) drawn
// on one layer of the city view; created by CityHandler. The members end at +0x44; the vtordisp (+0x44) and the
// Interface subobject (+0x48) follow them (0x4C bytes).
class Decoration : public engine::Object
{
public:
	Decoration(const std::string& tileName, int layer);
	virtual ~Decoration();

	virtual bool readAttributes(const engine::Properties& attrs);	// slot 1: folded body 0x41AFA0 (return true)
	virtual bool load();											// slot 2: the Image or ParticleSystem named m_tileName
	virtual void addToCity(CityMap* city);							// slot 3
	virtual void removeFromCity();									// slot 4
	virtual engine::Component* getComponent(int layer);				// slot 5: its component if drawn on that layer
	virtual void save(engine::XmlWriter* writer);					// slot 6: <decoration tile pos/> (city editor)
	virtual void setPosition(const engine::Vector2& pos);			// slot 7
	virtual const engine::Vector2& getPosition() const;				// slot 8
	virtual const std::string& getTileName() const;					// slot 9: folded body 0x411480

	engine::RefPtr<CityMap> m_city;								// +0x0C set by addToCity
	int m_layer;												// +0x10 city-view layer: the ctor's, then the manifest entry's
	engine::RefPtr<engine::Image> m_image;						// +0x14 for a manifest <decoration>
	engine::RefPtr<engine::Bitmap> m_bitmap;					// +0x18 the <decoration>'s image, shown by m_image
	engine::RefPtr<engine::ParticleSystem> m_particles;			// +0x1C for a manifest <particleFx>
	engine::Vector2 m_position;									// +0x20 "pos"
	std::string m_tileName;										// +0x28 "tile": the manifest entry's name
};
