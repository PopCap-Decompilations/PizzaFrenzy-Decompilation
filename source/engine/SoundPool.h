// engine::SoundPool: one named sound of the sound manager, a pool of handles so the sound can overlap.
#pragma once

#include <vector>

namespace engine
{
	class SimpleSound;
	class SoundHandle;

	// A sound-manifest entry of engine::SoundMgr (which deletes it through the non-virtual destructor). Not an
	// engine::Object. Layout: vfptr +0x00, members from +0x04 (0x24 bytes).
	class SoundPool
	{
	public:
		SoundPool(SimpleSound* soundSystem, const char* name, int count, float volume, float pitch, int channel);
		~SoundPool();

		// slot 0: first handle not playing (volume and pitch reset to the pool's); if none and create, a new one
		// from the sound system kept in the pool; else 0
		virtual SoundHandle* getHandle(bool create);
		// slot 1: plays a free handle at volume * m_volume and pitch * m_pitch; nothing when every handle is busy
		virtual void play(float volume, float pitch);
		// slot 2: handles whose isPlaying() is true
		virtual int getPlayingCount();
		// slot 3 (body folded with SoundHandle::setListener)
		virtual void setVolume(float volume);
		// slot 4 (body folded with SoundHandle::setCategory)
		virtual void setPitch(float pitch);

		float m_volume;							// +0x04 base volume multiplier
		float m_pitch;							// +0x08 base pitch multiplier
		const char* m_name;						// +0x0C sound name; points into the owner's map key
		std::vector<SoundHandle*> m_handles;	// +0x10 preallocated and grown handles
		SimpleSound* m_soundSystem;				// +0x20 the sound system (g_soundSystem)
	};
}
