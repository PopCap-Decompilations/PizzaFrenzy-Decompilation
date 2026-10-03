// MusicPlayer, the game's music player, and its states (stopped, playing, fading out).
#pragma once

#include "engine/Object.h"
#include "engine/RefPtr.h"
#include "engine/State.h"
#include "PizzaPopup.h"

namespace engine
{
	class SoundHandle;
}

class MusicState;
class MusicTrack;

// The game's music player (PizzaFrenzy::m_music): plays MusicTracks with fades through a state machine of
// MusicStates, and can be switched off. The members end at +0x38; the vtordisp (+0x38) and the Interface subobject
// (+0x3C) follow them (0x40 bytes).
class MusicPlayer : public engine::Object
{
public:
	MusicPlayer();
	virtual ~MusicPlayer();

	virtual void setState(MusicState* state);									// slot 1
	virtual void update(engine::UpdateContext& info);							// slot 2

	void stop();
	void setVolume(float volume);
	void setEnabled(bool enabled);
	void play(MusicTrack* track);

	engine::RefPtr<MusicTrack> m_track;											// +0x0C current track
	engine::SoundHandle* m_sound;												// +0x10 raw, destroyed by its slot 4
	engine::RefPtr<MusicTrack> m_nextTrack;										// +0x14 queued track
	bool m_fadingOut;															// +0x18 MusicFadeOutState waits
	GameStateMachine m_states;													// +0x1C the current MusicState
	bool m_enabled;																// +0x34 music on (starts true)
};

// The states: engine::State<MusicPlayer> (owner at +0x0C), then the vtordisp (+0x10) and the Interface subobject
// (+0x14); 0x18 bytes, no members of their own, implicit destructors.

// Abstract music state; it makes update (slot 3) pure again.
class MusicState : public engine::State<MusicPlayer>
{
public:
	MusicState(MusicPlayer* player);

	virtual void play(MusicTrack* track) = 0;									// slot 5
	virtual bool isPlaying() = 0;												// slot 6
	virtual void disable() = 0;													// slot 7
	virtual void enable() = 0;													// slot 8
	virtual void stop() = 0;													// slot 9

	virtual void update(engine::UpdateContext& context) = 0;					// slot 3 (engine::StateBase)
};

// Nothing playing (or paused while the music is off): play() queues the track and starts playing if enabled.
class MusicStoppedState : public MusicState
{
public:
	MusicStoppedState(MusicPlayer* player);

	virtual void play(MusicTrack* track);										// slot 5
	virtual bool isPlaying();													// slot 6: false
	virtual void disable();														// slot 7: empty
	virtual void enable();														// slot 8
	virtual void stop();														// slot 9

	virtual void enter();														// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);						// slot 3 (engine::StateBase): empty
};

// Music playing: fades out when a next track is queued, stops when the sound ends or the music is switched off.
// Constructor always inlined.
class MusicPlayingState : public MusicState
{
public:
	MusicPlayingState(MusicPlayer* player)
		: MusicState(player)
	{
	}

	virtual void play(MusicTrack* track);										// slot 5
	virtual bool isPlaying();													// slot 6: true
	virtual void disable();														// slot 7
	virtual void enable();														// slot 8: same body as enter
	virtual void stop();														// slot 9

	virtual void enter();														// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);						// slot 3 (engine::StateBase)
};

// Stops (fades out) the current sound, waits for it to end, then starts the queued track. Constructor always inlined.
class MusicFadeOutState : public MusicState
{
public:
	MusicFadeOutState(MusicPlayer* player)
		: MusicState(player)
	{
	}

	virtual void startNextTrack();												// slot 10

	virtual void play(MusicTrack* track);										// slot 5
	virtual bool isPlaying();													// slot 6
	virtual void disable();														// slot 7
	virtual void enable();														// slot 8: empty
	virtual void stop();														// slot 9

	virtual void enter();														// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);						// slot 3 (engine::StateBase)
};
