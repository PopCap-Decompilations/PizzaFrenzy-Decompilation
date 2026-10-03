#include "SplatManifestHandler.h"

#include "Properties.h"
#include "SplatFactory.h"

namespace engine
{
	// 0x492300
	void SplatManifestHandler::setFactory(SplatFactory* factory)
	{
		m_factory = factory;
	}

	// 0x492320
	void SplatManifestHandler::endElement(const std::string& name)
	{
		if (name == "splat")
		{
			m_factory->addSplatDef(m_splatName, m_splat);
			m_splat = NULL;
		}
	}

	// 0x492360
	SplatManifestHandler::SplatManifestHandler()
	{
		m_splat = NULL;
		m_factory = NULL;
	}

	// 0x492400
	SplatManifestHandler::~SplatManifestHandler()
	{
	}

	// 0x492480
	SplatDef::SplatDef()
	{
	}

	// 0x4925A0
	void SplatManifestHandler::startSplat(std::string name, Properties attributes)
	{
		m_splat = new SplatDef();
		m_splat->m_lifeTime = attributes.getRange("lifeTime", Range(1.0f, 1.0f));
		m_splatName = attributes.getString("name", "");
		m_splat->m_velocityX = m_splat->m_velocityY = Range(0.0f, 0.0f);
		m_splat->m_acceleration = Range(0.0f, 0.0f);
		m_splat->m_startScale = Range(1.0f, 1.0f);
		m_splat->m_endScale = Range(1.0f, 1.0f);
		m_splat->m_startAlpha = 0.0f;
		m_splat->m_endAlpha = 0.0f;
		m_splat->m_unknown30 = Range(1.0f, 1.0f);
		m_splat->m_rotation = Range(0.0f, 0.0f);
		m_splat->m_startOpacity = m_splat->m_endOpacity = 1.0f;
		m_splat->m_extForce.set(0.0f, 0.0f);
		m_splat->m_jitterPeriodX = m_splat->m_jitterAmplitudeX = Range(0.0f, 0.0f);
		m_splat->m_jitterPeriodY = m_splat->m_jitterAmplitudeY = Range(0.0f, 0.0f);
		m_splat->m_destination.set(0.0f, 0.0f);
	}

	// 0x4928C0
	void SplatManifestHandler::startElement(const std::string& name, const Properties& attributes)
	{
		if (name == "splat")
		{
			startSplat(name, attributes);
		}
		else if (name == "velocity" && m_splat)
		{
			m_splat->m_velocityX = attributes.getRange("startX", Range(0.0f, 10.0f));
			m_splat->m_velocityY = attributes.getRange("startY", Range(0.0f, 10.0f));
		}
		else if (name == "acceleration" && m_splat)
		{
			m_splat->m_acceleration = attributes.getRange("rate", Range(0.0f, 0.0f));
		}
		else if (name == "extForce" && m_splat)
		{
			m_splat->m_extForce = attributes.getPoint("force", Vector2(0.0f, 0.0f));
		}
		else if (name == "scale" && m_splat)
		{
			m_splat->m_startAlpha = attributes.getFloat("startAlpha", 0.0f);
			m_splat->m_endAlpha = attributes.getFloat("endAlpha", 1.0f);
			m_splat->m_startScale = attributes.getRange("startScale", Range(1.0f, 1.0f));
			m_splat->m_endScale = attributes.getRange("endScale", Range(1.0f, 1.0f));
		}
		else if (name == "rotation" && m_splat)
		{
			m_splat->m_rotation = attributes.getRange("degrees", Range(0.0f, 0.0f));
		}
		else if (name == "opacity" && m_splat)
		{
			m_splat->m_startOpacity = attributes.getFloat("startOp", 1.0f);
			m_splat->m_endOpacity = attributes.getFloat("endOp", 1.0f);
		}
		else if (name == "resource" && m_splat)
		{
			std::string type = attributes.getString("type", "");
			if (type == "image")
			{
				m_splat->m_imagePath = attributes.getString("path", "INVALID_RES_PATH");
				std::string pivot = attributes.getString("pivot", "center");
				if (pivot == "center")
					m_splat->m_pivot = 1;
				else
					m_splat->m_pivot = 0;
			}
			else if (type == "text")
			{
				m_splat->m_fontPath = attributes.getString("font", "INVALID_FONT_PATH");
				m_splat->m_text = attributes.getString("text", "");
				m_splat->m_textColor = attributes.getColor("color", Color(255, 255, 255));
				std::string textStyle = attributes.getString("textStyle", "");
				if (textStyle.find("shadow") != std::string::npos)
				{
					m_splat->m_textShadow = true;
					m_splat->m_textShadowOffset = attributes.getPoint("textShadowOffset", Vector2(1.0f, 1.0f));
					m_splat->m_textShadowOpacity = attributes.getFloat("textShadowOpacity", 1.0f);
				}
				else
				{
					m_splat->m_textShadow = false;
				}
			}
		}
		else if (name == "destination" && m_splat)
		{
			m_splat->m_destination = attributes.getPoint("pos", Vector2(0.0f, 0.0f));
		}
		else if (name == "jitter" && m_splat)
		{
			std::string axis = attributes.getString("axis", "");
			if (axis == "X")
			{
				m_splat->m_jitterPeriodX = attributes.getRange("period", Range(1.0f, 1.0f));
				m_splat->m_jitterAmplitudeX = attributes.getRange("amplitude", Range(0.0f, 0.0f));
			}
			else if (axis == "Y")
			{
				m_splat->m_jitterPeriodY = attributes.getRange("period", Range(1.0f, 1.0f));
				m_splat->m_jitterAmplitudeY = attributes.getRange("amplitude", Range(0.0f, 0.0f));
			}
		}
	}
}
