// engine::SimpleSoundDX: the DirectSound implementation of the sound system (.\SimpleSoundDX.cpp).
#pragma once

#include <map>
#include <string>
#include <vector>

#include <windows.h>
#include <dsound.h>

#include "SimpleSound.h"

namespace engine
{
	class OggDecoder;
	class SoundDataSource;
	class SoundHandle;
	class SoundHandleDX;
	class SoundStream;

	// Primary buffer 44.1 kHz 16-bit stereo, a name -> SoundDataSource map, the live sound handles and a master
	// volume. Made by the static create(hwnd); no destructor exists (the object is never deleted). Layout:
	// SimpleSound +0x00 (vfptr and the 64 category volumes), members +0x104 (0x134 bytes).
	class SimpleSoundDX : public SimpleSound
	{
	public:
		SimpleSoundDX();

		// line 31: new SimpleSoundDX on hwnd, init(); sets s_instance and g_simpleSoundDX (0 on failure)
		static SimpleSoundDX* create(HWND hwnd);
		// line 376: an Ogg decoder opened on stream, 0 if it cannot be opened (used by WaveBufferSource and FileStream)
		static OggDecoder* createDecoder(SoundStream* stream);

		// DirectSound device, cooperative level, primary buffer format; false on any failure
		bool init();
		// m_sources lookup, 0 if absent (the DirectSound handles use it through s_instance)
		SoundDataSource* findSource(const char* name);
		// erases the handle from m_handles (called by the handle when it is destroyed)
		void removeHandle(SoundHandleDX* handle);

		// SimpleSound
		virtual bool hasSound(const char* name);
		virtual bool loadStaticSound(const char* name, SoundStream* stream);
		virtual bool loadStreamingSound(const char* name, SoundStream* stream);
		virtual int getSoundCount();
		virtual void freeUnusedSounds();
		virtual void unloadSound(const char* name);
		virtual SoundHandle* createHandle(const char* name, bool softwareBuffer, int category);
		virtual void setMasterVolume(float volume);
		virtual float getMasterVolume();
		virtual void stopAll();
		virtual void update(int elapsedMs);
		virtual void dumpSounds();
		virtual void dumpHandles();

		IDirectSound* m_directSound;			// +0x104 from DirectSoundCreate
		IDirectSoundBuffer* m_primaryBuffer;	// +0x108 primary buffer, playing looped
		HWND m_hwnd;							// +0x10C cooperative-level window (DSSCL_PRIORITY)
		void* m_listener;						// +0x110 guess: zeroed by the constructor, never used
		std::vector<SoundHandleDX*> m_handles;	// +0x114 live sound handles created by createHandle
		std::map<std::string, SoundDataSource*> m_sources;	// +0x124 loaded sounds by name
		float m_masterVolume;					// +0x130 clamped to 0..1

		// set by create(); the DirectSound handles use it for the device and findSource
		static SimpleSoundDX* s_instance;
	};

	// second copy written by create(), never read
	extern SimpleSoundDX* g_simpleSoundDX;
}
