#include "ParticleSystem.h"

#include "Application.h"
#include "ParticleStream.h"
#include "ParticleSystemDef.h"
#include "ParticleSystemLoader.h"

namespace engine
{
	// 0x473AA0 (folded)
	void ParticleSystem::updateBounds()
	{
		m_bounds.clear();
		removeTreeFlags(8);
	}

	// 0x478EC0
	void ParticleSystem::advance(float dt)
	{
		UpdateContext context;
		context.elapsed = dt;
		Container::update(context);
	}

	// 0x478EF0
	void ParticleSystem::stop()
	{
		for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
			static_cast<ParticleStream*>(*it)->stop();
		m_active = false;
	}

	// 0x478F20
	bool ParticleSystem::isFinished() const
	{
		for (std::vector<Component*>::const_iterator it = m_children.begin(); it != m_children.end(); ++it)
		{
			if (!static_cast<ParticleStream*>(*it)->isFinished())
				return false;
		}
		return true;
	}

	// 0x478F60
	void ParticleSystem::update(UpdateContext& context)
	{
		if (m_active && m_duration > 0.0f)
		{
			m_timeLeft -= context.elapsed;
			if (m_timeLeft < 0.0f)
				stop();
		}
		Container::update(context);
		if (m_autoRemove && isFinished())
			setFlags(0x10);
	}

	// 0x478FE0
	void ParticleSystem::start()
	{
		if (!m_active)
		{
			for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
				static_cast<ParticleStream*>(*it)->clear();
		}
		for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
			static_cast<ParticleStream*>(*it)->start();
		if (m_duration > 0.0f)
			m_timeLeft = m_duration;
		if (m_warmup > 0.0f)
			advance(m_warmup);
		m_active = true;
	}

	// 0x4790A0
	bool ParticleSystem::addStream(const ParticleStreamDef& def)
	{
		ParticleStream* stream = new ParticleStream();
		addChild(stream);
		stream->setParent(def.m_parent);
		stream->m_emitRate = def.m_emitRate;
		stream->m_maxParticles = def.m_maxParticles;
		stream->m_velocityX = def.m_velocityX;
		stream->m_velocityY = def.m_velocityY;
		stream->m_acceleration = def.m_acceleration;
		stream->m_positionX = def.m_positionX;
		stream->m_positionY = def.m_positionY;
		stream->m_startScale = def.m_startScale;
		stream->m_endScale = def.m_endScale;
		stream->m_rotation = def.m_rotation;
		stream->m_jitterPeriodX = def.m_jitterXPeriod;
		stream->m_jitterAmplitudeX = def.m_jitterXAmplitude;
		stream->m_jitterPeriodY = def.m_jitterYPeriod;
		stream->m_jitterAmplitudeY = def.m_jitterYAmplitude;
		stream->m_constantScale = def.m_constantScale;
		stream->m_constantOpacity = def.m_constantOpacity;
		stream->m_blackHole = def.m_blackHole;
		stream->m_blackHoleAccel = def.m_blackHoleAccel;
		stream->m_blackHolePos = def.m_blackHolePos;
		stream->m_lifeTime = def.m_lifeTime;
		stream->m_startOpacity = def.m_startOpacity;
		stream->m_endOpacity = def.m_endOpacity;
		stream->m_extForce = def.m_extForce;
		stream->m_filter = def.m_filter;
		stream->m_groupRelative = def.m_offsetType;
		stream->m_animMode = def.m_frameMode;
		stream->m_autoStop = def.m_autoStop;
		stream->m_startDelay = def.m_startDelay;
		stream->m_animPath = def.m_resourcePath;
		stream->createParticles(&def);
		return true;
	}

	// 0x479330
	bool ParticleSystem::load(ParticleSystemDef* def)
	{
		removeAllChildren();
		m_duration = def->m_autoStop;
		m_warmup = def->m_warmup;
		for (std::vector<RefPtr<ParticleStreamDef> >::iterator it = def->m_streams.begin(); it != def->m_streams.end(); ++it)
		{
			if (!addStream(**it))
				return false;
		}
		return true;
	}

	// 0x479390
	bool ParticleSystem::load(std::string fileName)
	{
		ParticleSystemLoader loader;
		setName(fileName);
		try
		{
			getApplication()->loadXml(fileName, &loader);
		}
		catch (...)
		{
			return false;
		}
		ParticleSystemDef* def = loader.getDefinition();
		def->loadFrames();
		return load(def);
	}

	// 0x479470: m_autoRemove is stored after m_duration (0x4794B8, 0x4794BF, 0x4794C9)
	ParticleSystem::ParticleSystem()
		: m_active(false), m_duration(-1.0f)
	{
		m_autoRemove = false;
	}

	// 0x479510
	std::string ParticleSystem::getTypeName() const
	{
		return "ParticleSystem";
	}
}
