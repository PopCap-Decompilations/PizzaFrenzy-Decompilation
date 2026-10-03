#include "ParticleStream.h"

#include "AnimImage.h"
#include "Application.h"
#include "Image.h"
#include "Oscillator.h"
#include "Particle.h"
#include "ParticleSystemDef.h"
#include "Range.h"
#include "Rect.h"

namespace engine
{
	// 0x493770
	void ParticleStream::start()
	{
		if (m_autoStop > 0.0f)
			m_stopTimer = m_autoStop;
		if (m_startDelay > 0.0f)
			m_delayTimer = m_startDelay;
		else
			m_emitting = true;
	}

	// 0x4937C0
	void ParticleStream::stop()
	{
		m_emitting = false;
	}

	// 0x4937D0
	bool ParticleStream::isFinished() const
	{
		return !m_emitting && m_delayTimer <= 0.0f && m_children.empty();
	}

	// 0x493810
	void ParticleStream::clear()
	{
		std::vector<Component*>::iterator it = m_children.begin();
		while (it != m_children.end())
		{
			(*it)->onRemovedFrom(this);
			it = m_children.erase(it);
		}
		for (unsigned int i = 0; i < m_particles.size(); i++)
			m_particles[i]->setVisible(false);
	}

	// 0x4938A0
	ParticleStream::~ParticleStream()
	{
		for (std::vector<Particle*>::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
			(*it)->release();
	}

	// 0x493A10
	void ParticleStream::setParent(std::string name)
	{
		if (!name.empty())
			m_parent = getApplication()->findComponent(name);
	}

	// 0x493AA0
	std::string ParticleStream::getTypeName() const
	{
		return "ParticleStream";
	}

	// 0x473AA0 (folded)
	void ParticleStream::updateBounds()
	{
		m_bounds.clear();
		removeTreeFlags(8);
	}

	// 0x493D90
	void ParticleStream::emitParticle(Vector2 position)
	{
		Particle* particle = NULL;
		for (int tries = 25; particle == NULL && tries > 0; tries--)
		{
			if (m_particles.empty())
				return;
			Particle* candidate = m_particles.at(randomInt(0, m_particles.size() - 1));
			if (candidate->getRefCount() == 1)
				particle = candidate;
		}
		if (particle == NULL)
			return;
		particle->m_stream = this;
		particle->m_blackHole = m_blackHole;
		particle->m_acceleration = m_acceleration.random();
		float x = m_positionX.random();
		float y = m_positionY.random();
		if (!m_groupRelative)
		{
			if (m_parent)
			{
				Vector2 pos;
				pos.set(x, y);
				pos += position;
				Vector2 parentPos = m_parent->getScreenPosition();
				x = pos.x - parentPos.x;
				particle->m_parent = m_parent;
				y = pos.y - parentPos.y;
			}
			else
			{
				x = position.x + x;
				y = position.y + y;
			}
		}
		particle->m_groupRelative = m_groupRelative;
		particle->setPosition(x, y);
		float scale = m_startScale.random();
		particle->m_startScale = scale;
		if (!m_constantScale)
			scale = m_endScale.random();
		particle->m_endScale = scale;
		float life = m_lifeTime.random();
		particle->m_lifeTime = life;
		particle->m_timeLeft = life;
		x = m_velocityX.random();
		y = m_velocityY.random();
		particle->m_velocity = Vector2(x, y);
		particle->m_velocity.rotate(m_rotation.random());
		float opacity = m_startOpacity.random();
		particle->m_startOpacity = opacity;
		if (!m_constantOpacity)
			opacity = m_endOpacity.random();
		particle->m_endOpacity = opacity;
		float amplitudeX = m_jitterAmplitudeX.random();
		float amplitudeY = m_jitterAmplitudeY.random();
		if (amplitudeX != 0.0f || amplitudeY != 0.0f)
		{
			particle->removeAllAnimators();
			// VS2003 evaluated the constructor's arguments right to left: the Y period is drawn first (0x494072,
			// then 0x494081; the original draws both after operator new, which draws nothing)
			float periodY = m_jitterPeriodY.random();
			float periodX = m_jitterPeriodX.random();
			particle->addAnimator(new Oscillator(periodX, periodY, amplitudeX, amplitudeY, 0.0f));
		}
		particle->setVisible(true);
		particle->m_extForce = m_extForce;
		addChild(particle);
	}

	// 0x494100
	void ParticleStream::update(UpdateContext& context)
	{
		std::vector<Component*>::iterator it = m_children.begin();
		while (it != m_children.end())
		{
			if (((Particle*)*it)->m_timeLeft < 0.0f)
			{
				(*it)->onRemovedFrom(this);
				it = m_children.erase(it);
			}
			else
			{
				++it;
			}
		}
		if (m_startDelay > 0.0f && m_delayTimer > 0.0f)
		{
			m_delayTimer -= context.elapsed;
			if (m_delayTimer < 0.0f)
				m_emitting = true;
		}
		if (m_emitting)
		{
			if (m_autoStop > 0.0f)
			{
				m_stopTimer -= context.elapsed;
				if (m_stopTimer < 0.0f)
					m_emitting = false;
			}
			float due = m_emitRate * context.elapsed + m_emitAccumulator;
			Vector2 origin;
			if (!m_groupRelative)
			{
				origin = context.origin;
				origin.x += m_position.x;
				origin.y += m_position.y;
			}
			while (due >= 1.0f)
			{
				if (m_maxParticles != 0 && (int)m_children.size() >= m_maxParticles)
					break;
				emitParticle(origin);
				due -= 1.0f;
			}
			m_emitAccumulator = due;
		}
		Container::update(context);
	}

	// 0x4942E0
	ParticleStream::ParticleStream()
		: m_emitAccumulator(1.0f)
	{
		setFlags(4);
		m_blackHolePos.x = 0.0f;
		m_autoStop = -1.0f;
		m_startDelay = -1.0f;
		m_emitting = false;
		m_delayTimer = 0.0f;
		m_parent = NULL;
		m_filter = 0;
		m_blackHole = false;
		m_blackHoleAccel = 0.0f;
		m_blackHolePos.y = 0.0f;
	}

	// 0x4944B0
	void ParticleStream::createParticles(const ParticleStreamDef* def)
	{
		int count = (int)(m_lifeTime.max * m_emitRate * 1.5f + 1.0f);
		int frame = 0;
		int frameCount = def->m_frames.size();
		while (m_particles.size() < (unsigned int)count)
		{
			Particle* particle = new Particle();
			if (def->m_frameMode != 2)
			{
				// the frame is looked up (at(), 0x494575..0x49459D) before operator new (0x4945A5)
				Bitmap* bitmap = def->m_frames.at(frame);
				Image* image = new Image(bitmap);
				particle->addChild(image);
			}
			else
			{
				AnimImage* anim = new AnimImage();
				anim->AnimImage::load(def->m_resourcePath.c_str(), NULL, 0);
				anim->setLooping(true);
				anim->setFrameRate(def->m_animRate);
				anim->play();
				particle->addChild(anim);
			}
			particle->addRef();
			particle->setBlendMode(def->m_filter);
			m_particles.push_back(particle);
			frame = (frame + 1) % frameCount;
		}
	}
}
