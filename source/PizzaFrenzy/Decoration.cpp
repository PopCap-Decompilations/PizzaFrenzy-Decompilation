#include "Decoration.h"

#include "engine/Image.h"
#include "engine/ParticleSystem.h"
#include "engine/XmlWriter.h"

#include "CityMap.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x411210
void Decoration::setPosition(const engine::Vector2& pos)
{
	if (m_image)
		m_image->setPosition(pos);
	else if (m_particles)
		m_particles->setPosition(pos);
	m_position = pos;
}

// 0x411260
void Decoration::addToCity(CityMap* city)
{
	m_city = city;
	city->addDecoration(this);
}

// 0x4112B0
void Decoration::removeFromCity()
{
	m_city->removeDecoration(this);
	if (m_image)
		m_image->setFlags(0x10);
	else if (m_particles)
		m_particles->setFlags(0x10);
}

// 0x4112E0
Decoration::~Decoration()
{
	m_image = NULL;
	m_particles = NULL;
	m_city = NULL;
	m_bitmap = NULL;
}

// 0x411470
const engine::Vector2& Decoration::getPosition() const
{
	return m_position;
}

// 0x411480 (folded)
const std::string& Decoration::getTileName() const
{
	return m_tileName;
}

// 0x4114B0
Decoration::Decoration(const std::string& tileName, int layer)
	: m_layer(layer)
	, m_tileName(tileName)
{
}

// 0x411570
bool Decoration::load()
{
	TileType* type = PizzaFrenzy::getTileManifest()->getTile(m_tileName, "decoration");
	if (type)
	{
		m_image = new engine::Image(type->m_image);
		m_bitmap = type->m_image;
		m_layer = type->m_layer;
		return true;
	}
	else
	{
		ParticleFx* fx = PizzaFrenzy::getTileManifest()->getFx(m_tileName);
		if (fx)
		{
			m_particles = new engine::ParticleSystem();
			m_particles->load(fx->m_effect);
			m_layer = fx->m_layer;
			m_particles->start();
			return true;
		}
		else
		{
			return false;
		}
	}
}

// 0x411750
engine::Component* Decoration::getComponent(int layer)
{
	if (layer == m_layer)
	{
		if (m_image)
		{
			m_image->setName("decoration");
			return m_image;
		}
		return m_particles;
	}
	return NULL;
}

// 0x411810
void Decoration::save(engine::XmlWriter* writer)
{
	writer->startElement("decoration");
	writer->writeAttribute("tile", m_tileName);
	writer->writeAttribute("pos", engine::Point(m_position));
	writer->endElement();
}

// 0x41AFA0 (folded)
bool Decoration::readAttributes(const engine::Properties& attrs)
{
	return true;
}
