// AnimatedContainer: a container animated by keyframe tracks and sound cues; SoundCue and SoundCueTrack.
#pragma once

#include <map>
#include <string>
#include <utility>

#include "engine/Container.h"
#include "engine/FadeTimer.h"
#include "engine/KeyframeCurve.h"
#include "engine/Object.h"
#include "engine/Point.h"
#include "engine/PropertyTrack.h"

// A sound played when the animation time passes its time (the value type of SoundCueTrack::m_cues). Its constructor
// takes the time by value, so it is not std::pair's own.
class SoundCue : public std::pair<const float, std::string>
{
public:
	SoundCue(float time, const std::string& sound);
};

// Time-sorted sound cues, played by the game's sound manager as the animation time passes them.
class SoundCueTrack : public engine::Object
{
public:
	SoundCueTrack();
	virtual ~SoundCueTrack();

	void update(float time);
	void addCue(const SoundCue& cue);

	std::map<float, std::string> m_cues;				// +0x0C time to sound name
	std::map<float, std::string>::iterator m_next;		// +0x18 next cue to play
};

// A container that animates itself with keyframe tracks and sound cues: an "in" set and an "out" set, run by a
// small state machine (m_state). Used by HudMessage and SpeedBonusBanner. The constructor leaves m_unknown374,
// m_state and m_removeWhenDone uninitialised (init() sets the last two once the keys are added).
class AnimatedContainer : public engine::Container
{
public:
	AnimatedContainer();
	virtual ~AnimatedContainer();

	// overrides
	virtual void update(engine::UpdateContext& context);						// engine::Component slot 37

	void addScaleInKey(const engine::Keyframe<float>& key);
	void addAlphaInKey(const engine::Keyframe<float>& key);
	void addScaleOutKey(const engine::Keyframe<float>& key);
	void addAlphaOutKey(const engine::Keyframe<float>& key);
	void addPositionInKey(const engine::Keyframe<engine::Vector2>& key);
	void addPositionOutKey(const engine::Keyframe<engine::Vector2>& key);
	void addSoundInCue(const SoundCue& cue);
	void applyIn();
	void applyOut();
	bool isFinished();
	void startIn();
	void startOut();
	void rewind();
	void playIn();
	void init(bool removeWhenDone);

	engine::ScaleTrack m_scaleIn;						// +0x128
	engine::ScaleTrack m_scaleOut;						// +0x178
	engine::AlphaTrack m_alphaIn;						// +0x1C8
	engine::AlphaTrack m_alphaOut;						// +0x218
	engine::PositionTrack m_positionIn;					// +0x268
	engine::PositionTrack m_positionOut;				// +0x2B8
	SoundCueTrack m_soundsIn;							// +0x308
	SoundCueTrack m_soundsOut;							// +0x32C
	engine::FadeTimer m_timer;							// +0x350 time of the running in or out animation
	int m_unknown374;									// +0x374 never accessed by any code found
	int m_state;										// +0x378 0 ready, 1 playing in, 2 shown, 3 playing in then
														//        out, 4 playing out, 5 finished
	bool m_removeWhenDone;								// +0x37C set flag 16 (remove) when the out animation ends
};
