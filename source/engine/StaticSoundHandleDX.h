// engine::StaticSoundHandleDX: DirectSound handle whose buffer holds the whole sound (sounds of 2 s or less).
#pragma once

#include "SoundHandleDX.h"

namespace engine
{
	// Chosen by SimpleSoundDX::createHandle, which inlines the implicit constructor (m_bytesPerSecond is left to
	// create()); implicit destructor. Layout: SoundHandleDX +0x00, m_bytesPerSecond +0x5C (0x60 bytes).
	class StaticSoundHandleDX : public SoundHandleDX
	{
	public:
		// SoundHandle
		virtual void play();
		virtual void rewind();

		// SoundHandleDX
		virtual bool isStatic();
		virtual bool create(const char* soundName, bool softwareBuffer);
		virtual void updateStatus();
		virtual void updateData();
		virtual int getFadeOutStart();

		DWORD m_bytesPerSecond;					// +0x5C nAvgBytesPerSec of the source
	};
}
