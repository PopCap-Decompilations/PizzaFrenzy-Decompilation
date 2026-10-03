#include "Particle.h"

#include "Graphics.h"
#include "ParticleStream.h"

namespace engine
{
	// 0x4811D0 (folded)
	void Particle::setPosition(float x, float y)
	{
		m_position.set(x, y);
	}

	// 0x497E40
	void Particle::setupGraphics(Graphics& g)
	{
		Component::setupGraphics(g);
		if (!m_groupRelative)
		{
			Vector2 offset;
			if (m_parent)
			{
				offset = g.getTranslation();
				offset.x = m_position.x - offset.x + m_parent->getScreenPosition().x;
				offset.y = m_parent->getScreenPosition().y + m_position.y - offset.y;
			}
			else
			{
				offset = g.getTranslation();
				offset.x = m_position.x - offset.x;
				offset.y = m_position.y - offset.y;
			}
			g.translate(offset.x, offset.y);
		}
	}

	// 0x497F00
	void Particle::update(UpdateContext& context)
	{
		float elapsed = context.elapsed;
		m_timeLeft -= elapsed;
		Vector2 velocity(m_velocity);
		Vector2 position(getX(), getY());
		Vector2 delta(velocity);
		delta.normalize();
		delta *= elapsed * m_acceleration;
		velocity += delta;
		delta = m_extForce;
		delta *= elapsed;
		velocity += delta;
		if (m_blackHole)
		{
			Vector2 toHole(m_stream->m_blackHolePos);
			if (!m_groupRelative)
				toHole += m_stream->getScreenPosition();
			toHole -= m_position;
			toHole.normalize();
			toHole *= m_stream->m_blackHoleAccel;
			toHole *= elapsed;
			velocity += toHole;
		}
		m_velocity = velocity;
		velocity *= elapsed;
		position += velocity;
		setPosition(position.x, position.y);
		float life = (m_lifeTime - m_timeLeft) / m_lifeTime;
		float scale = (m_endScale - m_startScale) * life + m_startScale;
		float opacity = (m_endOpacity - m_startOpacity) * life + m_startOpacity;
		if (opacity <= 0.0f)
			opacity = 0.0f;
		else if (opacity >= 1.0f)
			opacity = 1.0f;
		if (scale <= 0.0f)
			scale = 0.0f;
		setScale(scale, scale);
		setAlpha(opacity);
		Container::update(context);
	}

	// 0x498140
	Particle::Particle()
	{
		setFlags(4);
		m_parent = NULL;
	}
}
