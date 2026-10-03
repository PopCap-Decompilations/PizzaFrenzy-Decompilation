// AnimatedLetter and AnimatedText: text whose letters pop in one after another (the end-of-day titles).
#pragma once

#include <string>
#include <vector>

#include "engine/Container.h"
#include "engine/Point.h"
#include "engine/RefPtr.h"

namespace engine
{
	class Font;
	class ParticleSystem;
	class ParticleSystemDef;
	class TextItem;
}

// One letter of an AnimatedText: waits m_delay, scales in from 0 to 1, optionally holds and scales out again; an
// optional particle burst. The constructor leaves m_char uninitialised (setup() sets it).
class AnimatedLetter : public engine::Container
{
public:
	AnimatedLetter();
	virtual ~AnimatedLetter();

	// overrides
	virtual void update(engine::UpdateContext& context);						// engine::Component slot 37

	void show(engine::Vector2 position);
	void hide();
	int setup(char c, engine::Font* font, engine::ParticleSystemDef* effect, float delay);

	engine::RefPtr<engine::ParticleSystem> m_particles;	// +0x128 made by setup() when an effect is given
	engine::RefPtr<engine::TextItem> m_text;			// +0x12C the letter ("%c")
	engine::RefPtr<engine::Object> m_unusedRef;			// +0x130 only ever nulled and released
	float m_timer;										// +0x134 countdown of the current state
	float m_duration;									// +0x138 scale in/out time (0.2 s)
	float m_delay;										// +0x13C delay before appearing (and before shrinking)
	float m_holdTime;									// +0x140 > 0: shrink again after this time
	char m_char;										// +0x144 the letter
	bool m_isSpace;										// +0x145 ' ': never shown
	int m_state;										// +0x148 0 grow, 1 shrink, 2 wait then grow, 3 wait then
														//        shrink, 4 idle, 5 hold, 7 hidden
};

// Text made of AnimatedLetters appearing one after another, centred on the position given to show().
class AnimatedText : public engine::Container
{
public:
	AnimatedText();
	virtual ~AnimatedText();

	void show(engine::Vector2 center);
	void hide();
	void setText(const std::string& text, float letterDelay, float spacing, engine::Font* font, engine::ParticleSystemDef* effect, float startDelay, float waveAmplitude);

	std::vector<AnimatedLetter*> m_letters;				// +0x128 addRef'd by hand, released by the destructor
	float m_spacing;									// +0x138 extra pixels between letters
	float m_width;										// +0x13C total width (show() centres it)
};
