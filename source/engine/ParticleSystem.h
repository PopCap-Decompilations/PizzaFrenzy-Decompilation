// engine::ParticleSystem: a container of particle streams built from a <particleSystem> definition.
#pragma once

#include <string>

#include "Container.h"

namespace engine
{
	class ParticleStreamDef;
	class ParticleSystemDef;

	// Its children are ParticleStreams (E05). start/stop, an optional duration (autoStop), a warm-up and removal
	// when every stream has finished. Container +0x00, members from +0x128, then the vtordisp (+0x138) and the
	// Interface subobject (+0x13C): 0x140 bytes. Implicit destructor (the deleting destructor is the folded
	// Container one, 0x47BBF0).
	class ParticleSystem : public Container
	{
	public:
		ParticleSystem();

		virtual std::string getTypeName() const;								// slot 24 (engine::Component)
		virtual void update(UpdateContext& context);							// slot 37 (engine::Component)
		virtual void updateBounds();											// slot 38 (engine::Component): folded 0x473AA0

		void advance(float dt);
		void stop();
		bool isFinished() const;
		void start();
		bool addStream(const ParticleStreamDef& def);
		bool load(ParticleSystemDef* def);
		bool load(std::string fileName);

		bool m_active;							// +0x128 started
		bool m_autoRemove;						// +0x129 set removal flag 0x10 when every stream has finished
		float m_duration;						// +0x12C from autoStop; -1 runs until stopped
		float m_timeLeft;						// +0x130
		float m_warmup;							// +0x134 seconds pre-simulated by start()
	};
}
