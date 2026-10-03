#include "ParticleSystemLoader.h"

#include "ParticleSystemDef.h"
#include "Properties.h"

namespace engine
{
	// 0x47BC40
	ParticleSystemLoader::ParticleSystemLoader()
	{
		m_stream = 0;
		m_system = 0;
	}

	// 0x47BD30
	ParticleSystemLoader::~ParticleSystemLoader()
	{
		m_stream = 0;
		m_system = 0;
	}

	// 0x47BE40
	ParticleSystemDef* ParticleSystemLoader::getDefinition() const
	{
		return m_system;
	}

	// 0x47BE70
	void ParticleSystemLoader::endElement(const std::string& name)
	{
		if (name == "particleStream")
			m_stream = 0;
	}

	// 0x47C130
	void ParticleSystemLoader::parseStream(std::string element, Properties attributes)
	{
		m_stream = new ParticleStreamDef();
		m_system->addStream(m_stream);
		m_stream->m_emitRate = attributes.getFloat("emitRate", 5.0f);
		m_stream->m_maxParticles = attributes.getInt("maxParticles", 0);
		std::string sequence = attributes.getString("sequence", "random");
		if (sequence == "anim")
			m_stream->m_frameMode = 2;
		else if (sequence == "sequential")
			m_stream->m_frameMode = 1;
		else
			m_stream->m_frameMode = 0;
		m_stream->m_animRate = attributes.getFloat("animRate", 15.0f);
		std::string offsetType = attributes.getString("offsetType", "independent");
		if (offsetType == "groupRelative")
			m_stream->m_offsetType = 1;
		else
			m_stream->m_offsetType = 0;
		std::string parent = attributes.getString("parent", "");
		if (parent != "")
			m_stream->m_parent = parent;
		float autoStop = attributes.getFloat("autoStop", -1.0f);
		m_stream->m_autoStop = autoStop;
		float startDelay = attributes.getFloat("startDelay", -1.0f);
		m_stream->m_startDelay = startDelay;
		Vector2 offset(0.0f, 0.0f);
		offset = attributes.getPoint("offset", offset);
		m_stream->m_offset = offset;
	}

	// 0x47C820
	void ParticleSystemLoader::parseSystem(std::string element, Properties attributes)
	{
		m_system = new ParticleSystemDef();
		float autoStop = attributes.getFloat("autoStop", -1.0f);
		float warmup = attributes.getFloat("warmup", 0.0f);
		m_system->m_autoStop = autoStop;
		m_system->m_warmup = warmup;
	}

	// 0x47C9B0
	void ParticleSystemLoader::startElement(const std::string& name, const Properties& attributes)
	{
		if (name == "particleSystem")
		{
			parseSystem(name, attributes);
		}
		else if (name == "particleStream")
		{
			parseStream(name, attributes);
		}
		else if (name == "velocity" && m_stream != 0)
		{
			m_stream->m_velocityX = attributes.getRange("startX", Range(0.0f, 10.0f));
			m_stream->m_velocityY = attributes.getRange("startY", Range(0.0f, 10.0f));
		}
		else if (name == "acceleration" && m_stream != 0)
		{
			m_stream->m_acceleration = attributes.getRange("rate", Range(0.0f, 0.0f));
		}
		else if (name == "extForce" && m_stream != 0)
		{
			m_stream->m_extForce = attributes.getPoint("force", Vector2(0.0f, 0.0f));
		}
		else if (name == "position" && m_stream != 0)
		{
			m_stream->m_positionX = attributes.getRange("startX", Range(0.0f, 0.0f));
			m_stream->m_positionY = attributes.getRange("startY", Range(0.0f, 0.0f));
		}
		else if (name == "rotation" && m_stream != 0)
		{
			m_stream->m_rotation = attributes.getRange("degrees", Range(0.0f, 0.0f));
		}
		else if (name == "scale" && m_stream != 0)
		{
			m_stream->m_startScale = attributes.getRange("startScale", Range(1.0f, 1.0f));
			m_stream->m_endScale = attributes.getRange("endScale", Range(1.0f, 1.0f));
			std::string constant = attributes.getString("constant", "false");
			if (constant == "true")
				m_stream->m_constantScale = true;
			else
				m_stream->m_constantScale = false;
		}
		else if (name == "particleLife" && m_stream != 0)
		{
			m_stream->m_lifeTime = attributes.getRange("lifeTime", Range(1.0f, 2.0f));
		}
		else if (name == "opacity" && m_stream != 0)
		{
			m_stream->m_startOpacity = attributes.getRange("startOp", Range(1.0f, 1.0f));
			m_stream->m_endOpacity = attributes.getRange("endOp", Range(1.0f, 1.0f));
			std::string constant = attributes.getString("constant", "false");
			if (constant == "true")
				m_stream->m_constantOpacity = true;
			else
				m_stream->m_constantOpacity = false;
		}
		else if (name == "resource" && m_stream != 0)
		{
			m_stream->m_resourcePath = attributes.getString("path", "INVALID_RES_PATH");
			std::string filter = attributes.getString("filter", "");
			if (filter == "X")
				m_stream->m_filter = 1;
			else if (filter == "Y")
				m_stream->m_filter = 1;
			else if (filter == "XY")
				m_stream->m_filter = 1;
			else
				m_stream->m_filter = 0;
		}
		else if (name == "jitter" && m_stream != 0)
		{
			std::string axis = attributes.getString("axis", "");
			if (axis == "X")
			{
				m_stream->m_jitterXPeriod = attributes.getRange("period", Range(0.0f, 0.0f));
				m_stream->m_jitterXAmplitude = attributes.getRange("amplitude", Range(0.0f, 0.0f));
			}
			else if (axis == "Y")
			{
				m_stream->m_jitterYPeriod = attributes.getRange("period", Range(0.0f, 0.0f));
				m_stream->m_jitterYAmplitude = attributes.getRange("amplitude", Range(0.0f, 0.0f));
			}
		}
		else if (name == "blackHole" && m_stream != 0)
		{
			m_stream->m_blackHole = true;
			m_stream->m_blackHolePos = attributes.getPoint("pos", Vector2(0.0f, 0.0f));
			m_stream->m_blackHoleAccel = attributes.getFloat("accel", 0.0f);
		}
	}
}
