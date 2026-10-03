// engine::SoundHandle (a playable sound instance) and engine::SoundListener (its loop and stop callbacks).
#pragma once

namespace engine
{
	class SoundHandle;

	// Notified by a handle (SoundHandle::setListener): slot 0 when a looping sound wraps around, slot 1 when a
	// sound stops. Nothing in the program implements it.
	class SoundListener
	{
	public:
		virtual void onSoundLooped(SoundHandle* handle) = 0;		// slot 0
		virtual void onSoundStopped(SoundHandle* handle) = 0;		// slot 1
	};

	// Abstract playable sound instance, made by SimpleSound::createHandle. Not an engine::Object, and no virtual
	// destructor: a handle deletes itself in destroy(). Layout: vfptr +0x00, members +0x04 (0x0C bytes).
	class SoundHandle
	{
	public:
		// no listener, category 0
		SoundHandle();

		virtual void play() = 0;								// slot 0: starts (fading in if a fade-in time is set)
		virtual void pause() = 0;								// slot 1: stops without rewinding
		virtual void stop() = 0;								// slot 2: stops (or starts the fade-out), rewinds, notifies
		virtual void rewind() = 0;								// slot 3: back to the start
		virtual void destroy() = 0;								// slot 4: unregisters and deletes the handle
		virtual void setPitch(float pitch) = 0;					// slot 5: frequency factor
		virtual float getPitch() = 0;							// slot 6
		virtual void setPan(float pan) = 0;						// slot 7: -1..1
		virtual float getPan() = 0;								// slot 8
		virtual void setVolume(float volume) = 0;				// slot 9: 0..1
		virtual float getVolume() = 0;							// slot 10
		virtual void setLooping(bool looping) = 0;				// slot 11
		virtual bool isLooping() = 0;							// slot 12
		virtual bool isPlaying() = 0;							// slot 13
		virtual void setFadeInTime(float seconds) = 0;			// slot 14: negative -> 0
		virtual void setFadeOutTime(float seconds) = 0;			// slot 15: negative -> 0
		// slot 16 (body folded with SoundPool::setVolume)
		virtual void setListener(SoundListener* listener);
		// slot 17: clears m_listener; the argument is ignored
		virtual void removeListener(SoundListener* listener);
		// slot 18 (body folded with SoundPool::setPitch)
		virtual void setCategory(int category);
		// slot 19: m_category; the argument is ignored
		virtual int getCategory(int unused);

		SoundListener* m_listener;				// +0x04
		int m_category;							// +0x08 volume category (0..63, see SimpleSound)
	};
}
