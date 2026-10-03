// engine::ParticleStream: a particle emitter component (one <particleStream> of a particle system).
#pragma once

#include <string>
#include <vector>

#include "Container.h"
#include "Point.h"
#include "Range.h"

namespace engine
{
	class Component;
	class Particle;
	class ParticleStreamDef;

	// Configured by ParticleSystem::addStream, which copies a ParticleStreamDef's values into the members and calls
	// createParticles; keeps a pool of Particle children and emits m_emitRate of them per second. Layout:
	// Container +0x00, members +0x128, then the vtordisp and Interface (0x22C bytes).
	class ParticleStream : public Container
	{
	public:
		// flags 4; emit accumulator 1, no auto stop and no start delay (-1)
		ParticleStream();
		// releases the pooled particles
		virtual ~ParticleStream();

		// Component
		// "ParticleStream"
		virtual std::string getTypeName() const;
		// drops expired particles, runs the start delay and auto-stop timers, emits emitRate * elapsed particles
		// (at most m_maxParticles alive), then Container::update
		virtual void update(UpdateContext& context);
		// clears m_bounds and the dirty flag 8 (body folded with SplatFactory::updateBounds, 0x473AA0)
		virtual void updateBounds();

		// stop timer = m_autoStop if > 0; delay timer = m_startDelay if > 0, else emitting
		void start();
		void stop();
		// not emitting, no pending start delay and no live particle
		bool isFinished() const;
		// removes every child and hides every pooled particle
		void clear();
		// m_parent = the application's component of that name, if name is not empty
		void setParent(std::string name);
		// up to 25 random picks for a free pooled particle (reference count 1), set up from the ranges and added
		// (the position is a Vector2 by value: update copy-constructs it on the stack)
		void emitParticle(Vector2 position);
		// the pool: (int)(lifeTime.max * emitRate * 1.5 + 1) particles with an Image (cycling the definition's
		// frames) or a looping AnimImage
		void createParticles(const ParticleStreamDef* def);

		float m_emitRate;						// +0x128 particles per second
		int m_maxParticles;						// +0x12C 0 = unlimited
		Range m_velocityX;						// +0x130 <velocity startX>
		Range m_velocityY;						// +0x138 <velocity startY>
		Range m_acceleration;					// +0x140 <acceleration rate>
		Range m_positionX;						// +0x148 <position startX>
		Range m_positionY;						// +0x150 <position startY>
		Range m_startScale;						// +0x158
		Range m_endScale;						// +0x160
		Range m_rotation;						// +0x168 degrees; rotates the start velocity
		Range m_jitterPeriodX;					// +0x170
		Range m_jitterAmplitudeX;				// +0x178
		Range m_jitterPeriodY;					// +0x180
		Range m_jitterAmplitudeY;				// +0x188
		bool m_constantScale;					// +0x190 end scale = start scale
		bool m_constantOpacity;					// +0x191 end opacity = start opacity
		bool m_blackHole;						// +0x192
		float m_blackHoleAccel;					// +0x194 <blackHole accel>
		Vector2 m_blackHolePos;					// +0x198
		Range m_lifeTime;						// +0x1A0 <particleLife lifeTime>, seconds
		Range m_startOpacity;					// +0x1A8
		Range m_endOpacity;						// +0x1B0
		Vector2 m_extForce;						// +0x1B8
		std::string m_animPath;					// +0x1C0 the definition's frame path
		int m_filter;							// +0x1DC 1 if the resource filter is X, Y or XY
		std::vector<Particle*> m_unusedParticles;	// +0x1E0 never used (only freed by the destructor)
		std::vector<Particle*> m_particles;		// +0x1F0 the pool made by createParticles (each addRef'd)
		float m_emitAccumulator;				// +0x200 fractional particles due; starts at 1
		bool m_emitting;						// +0x204
		int m_groupRelative;					// +0x208 offsetType "groupRelative"
		int m_animMode;							// +0x20C 0 random frame, 1 sequential, 2 AnimImage particles
		float m_autoStop;						// +0x210 seconds, -1 = never
		float m_stopTimer;						// +0x214
		float m_startDelay;						// +0x218 seconds, -1 = none
		float m_delayTimer;						// +0x21C
		Component* m_parent;					// +0x220 the "parent" component (positions are relative to it)
	};
}
