// engine::ParticleSystemLoader: the XML handler that reads a particle file into a ParticleSystemDef.
#pragma once

#include <string>

#include "RefPtr.h"
#include "XmlHandlerBase.h"

namespace engine
{
	class ParticleStreamDef;
	class ParticleSystemDef;
	class Properties;

	// <particleSystem autoStop warmup> and <particleStream ...> with its children velocity, acceleration, extForce,
	// position, rotation, scale, particleLife, opacity, resource, jitter and blackHole. A stack object of
	// ParticleSystem::load and of the game's loader. XmlHandlerBase +0x00 (XmlHandler at +0x0C), members from +0x14,
	// then the vtordisp (+0x1C) and the Interface subobject (+0x20): 0x24 bytes.
	class ParticleSystemLoader : public XmlHandlerBase
	{
	public:
		ParticleSystemLoader();
		virtual ~ParticleSystemLoader();

		virtual void startElement(const std::string& name, const Properties& attributes);	// slot 0 (engine::XmlHandler)
		virtual void endElement(const std::string& name);									// slot 1 (engine::XmlHandler)

		virtual void parseStream(std::string element, Properties attributes);				// slot 1
		virtual void parseSystem(std::string element, Properties attributes);				// slot 2

		ParticleSystemDef* getDefinition() const;

		RefPtr<ParticleStreamDef> m_stream;		// +0x14 <particleStream> being read (released at its end tag)
		RefPtr<ParticleSystemDef> m_system;		// +0x18 the result
	};
}
