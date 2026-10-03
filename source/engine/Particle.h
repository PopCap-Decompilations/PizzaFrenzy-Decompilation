// engine::Particle: one particle of a ParticleStream.
#pragma once

#include "Container.h"
#include "Point.h"

namespace engine
{
	class Component;
	class Graphics;
	class ParticleStream;

	// A Container holding the particle's Image or AnimImage: moved by its velocity, the acceleration along it, the
	// external force and the stream's black hole; scale and opacity follow its life. Made for the stream's pool by
	// ParticleStream::createParticles. Implicit destructor (its deleting destructor is folded with other
	// Containers'). Layout: Container +0x00, members +0x128, then the vtordisp and Interface (0x16C bytes).
	class Particle : public Container
	{
	public:
		// flags 4, no parent
		Particle();

		// Component
		// m_position.set(x, y), without Component's addTreeFlags(8) (body folded with Splat::setPosition, 0x4811D0)
		virtual void setPosition(float x, float y);
		// Component's, then (unless group-relative) moved to the world position relative to m_parent
		virtual void setupGraphics(Graphics& g);
		// life countdown and movement, scale and opacity from the life fraction, then Container::update
		virtual void update(UpdateContext& context);

		ParticleStream* m_stream;				// +0x128 the owning stream
		Vector2 m_velocity;						// +0x12C
		float m_acceleration;					// +0x134 along the velocity
		float m_startScale;						// +0x138
		float m_endScale;						// +0x13C
		float m_startOpacity;					// +0x140
		float m_endOpacity;						// +0x144
		Vector2 m_extForce;						// +0x148
		bool m_blackHole;						// +0x150
		float m_lifeTime;						// +0x154
		float m_timeLeft;						// +0x158 the stream drops the particle when it is negative
		Component* m_parent;					// +0x15C the stream's parent (positions are relative to it)
		int m_groupRelative;					// +0x160
	};
}
