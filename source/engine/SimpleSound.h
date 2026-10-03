// engine::SimpleSound: the abstract sound system (implemented by SimpleSoundDX).
#pragma once

namespace engine
{
	class SoundHandle;
	class SoundStream;

	// The sound system held by g_soundSystem: sound data registry, handle creation, master volume and 64 category
	// volumes. Not an engine::Object; no virtual destructor. Layout: vfptr +0x00, m_categoryVolumes +0x04
	// (0x104 bytes).
	class SimpleSound
	{
	public:
		// every category volume 1.0
		SimpleSound();

		// slot 0
		virtual bool hasSound(const char* name) = 0;
		// slot 1: decoded into memory (WaveBufferSource); true if already loaded
		virtual bool loadStaticSound(const char* name, SoundStream* stream) = 0;
		// slot 2: decoded on demand (FileStream)
		virtual bool loadStreamingSound(const char* name, SoundStream* stream) = 0;
		// slot 3
		virtual int getSoundCount() = 0;
		// slot 4: deletes the data sources no handle uses
		virtual void freeUnusedSounds() = 0;
		// slot 5
		virtual void unloadSound(const char* name) = 0;
		// slot 6
		virtual SoundHandle* createHandle(const char* name, bool softwareBuffer, int category) = 0;
		// slot 7
		virtual void setMasterVolume(float volume) = 0;
		// slot 8
		virtual float getMasterVolume() = 0;
		// slot 9: clamped to 0..1; categories outside 0..63 are ignored
		virtual void setCategoryVolume(int category, float volume);
		// slot 10: 1.0 for an invalid category
		virtual float getCategoryVolume(int category);
		// slot 11: stop() on every handle
		virtual void stopAll() = 0;
		// slot 12: update(elapsedMs) on every handle
		virtual void update(int elapsedMs) = 0;
		// slot 13: debug listing of the data sources (the log is compiled out)
		virtual void dumpSounds() = 0;
		// slot 14: debug listing of the handles (the log is compiled out)
		virtual void dumpHandles() = 0;

		float m_categoryVolumes[64];			// +0x04 all 1.0
	};
}
