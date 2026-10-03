// engine::SoundHandleDX: a DirectSound sound instance streaming from a SoundDataSource.
#pragma once

#include <windows.h>

#include "SoundHandle.h"

struct IDirectSoundBuffer;

namespace engine
{
	class SoundDataSource;

	// Plays through a 2-second IDirectSoundBuffer refilled half by half from its data source, with fades and the
	// category volume (.\SoundHandleDX.cpp); StaticSoundHandleDX holds the whole sound in its buffer instead.
	// Created by SimpleSoundDX::createHandle, which calls the non-virtual destructor when create() fails;
	// destroy() deletes the handle. Layout: SoundHandle +0x00, members +0x0C (0x5C bytes).
	class SoundHandleDX : public SoundHandle
	{
	public:
		// pitch and volume 1, fade timer -1, fade state 1; the buffer sizes and positions are left to create()
		SoundHandleDX();
		// not virtual (SoundHandle has no virtual destructor): delete[] m_name
		~SoundHandleDX();

		// SoundHandle
		virtual void play();
		virtual void pause();
		virtual void stop();
		virtual void rewind();
		virtual void destroy();
		virtual void setPitch(float pitch);
		// m_pitch (body folded with MusicTrack::getFadeInTime, 0x450470)
		virtual float getPitch();
		virtual void setPan(float pan);
		// m_pan (body folded with MusicTrack::getVolume, 0x450490)
		virtual float getPan();
		virtual void setVolume(float volume);
		// m_volume (body folded with MusicTrack::getFadeOutTime, 0x450480)
		virtual float getVolume();
		virtual void setLooping(bool looping);
		// m_looping (body folded with MusicTrack::getLoop, 0x4504A0)
		virtual bool isLooping();
		virtual bool isPlaying();
		virtual void setFadeInTime(float seconds);
		virtual void setFadeOutTime(float seconds);

		// slot 20: m_name (body folded, 0x469DC0)
		virtual const char* getName();
		// slot 21: false; StaticSoundHandleDX returns true (body folded, 0x4529D0)
		virtual bool isStatic();
		// slot 22: finds and attaches to the data source, creates the buffer, copies the name, fills the first half
		virtual bool create(const char* soundName, bool softwareBuffer);
		// slot 23: while playing: updateData(), updateFade(elapsedMs), updateStatus()
		virtual void update(int elapsedMs);
		// slot 24: empty here (body folded, 0x4D0470); StaticSoundHandleDX reads the buffer status
		virtual void updateStatus();
		// slot 25: fade-in / fade-out state machine; stops when faded out
		virtual void updateFade(int elapsedMs);
		// slot 26: refills the half the play cursor left; stops once the last half has played
		virtual void updateData();
		// slot 27: locks the buffer and fills it from the data source
		virtual void fillBuffer(int offset, int bytes);
		// slot 28: locks the buffer and fills it with silence
		virtual void zeroBuffer(int offset, int bytes);
		// slot 29: byte position where the automatic fade-out starts
		virtual int getFadeOutStart();
		// slot 30: volume * category volume * master volume, given to SetVolume in hundredths of a dB (2000 ln v,
		// at least -10000) when it changed
		virtual void applyVolume(float volume);

		SoundDataSource* m_source;				// +0x0C
		IDirectSoundBuffer* m_buffer;			// +0x10
		int m_bufferBytes;						// +0x14 2 s of audio (StaticSoundHandleDX: the whole sound)
		int m_halfBufferBytes;					// +0x18
		int m_nextFillOffset;					// +0x1C 0 or m_halfBufferBytes: the half refilled next
		int m_sourcePosition;					// +0x20 position passed to SoundDataSource::getBytes
		char* m_name;							// +0x24 copy of the sound name (new[])
		float m_pitch;							// +0x28 1.0
		float m_volume;							// +0x2C 1.0
		float m_pan;							// +0x30
		bool m_looping;							// +0x34
		bool m_isPlaying;						// +0x35 Sprout's own name (the "m_isPlaying == true" log)
		bool m_dataEnded;						// +0x36 the end of the data was written (not looping)
		int m_endOffset;						// +0x38 buffer half holding the end of the data
		DWORD m_frequency;						// +0x3C nSamplesPerSec of the source
		DWORD m_playPosition;					// +0x40 bytes played (StaticSoundHandleDX: the last play cursor)
		DWORD m_lastPlayCursor;					// +0x44
		float m_appliedVolume;					// +0x48 last volume given to SetVolume; -1 forces an update
		float m_fadeInTime;						// +0x4C
		float m_fadeOutTime;					// +0x50
		float m_fadeTimer;						// +0x54 -1
		int m_fadeState;						// +0x58 0 fading in, 1 playing, 2 fading out (3 is handled as 2)
	};
}
