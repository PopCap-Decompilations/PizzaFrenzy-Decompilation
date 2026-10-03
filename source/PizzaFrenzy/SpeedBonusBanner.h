// SpeedBonusBanner: the "SPEED BONUS!" banner of the game screen.
#pragma once

#include "engine/FadeContainer.h"
#include "engine/RefPtr.h"

namespace engine
{
	class TextItem;
}

class AnimatedContainer;

// Dark strip at the bottom of the game screen with a keyframed "SPEED BONUS!" title that swooshes across
// ("bonus_speedySwish") and a second text line; fades out when the title's animation ends. Created by
// GameScreen::init, which ticks it every frame (GameScreen::tick). The constructor leaves m_bonusText and m_showing
// unset. The members end at +0x154; the vtordisp (+0x154) and the Interface subobject (+0x158) follow them
// (0x15C bytes), implicit destructor.
class SpeedBonusBanner : public engine::FadeContainer
{
public:
	SpeedBonusBanner();

	virtual bool isFadeFinished() const;										// slot 78 (engine::FadeContainer): always true

	void hide();
	// Not an override: slot 37 stays engine::FadeContainer::update, and GameScreen::tick calls this one directly.
	// The parameter is const so that the name does not override Component::update (the original's parameter type
	// is unknown: it is not used).
	void update(const engine::UpdateContext& context);
	void init();

	engine::TextItem* m_bonusText;												// +0x144 titleFont at (400,550); raw
	engine::RefPtr<AnimatedContainer> m_animation;								// +0x148 carries the title
	engine::RefPtr<engine::TextItem> m_titleText;								// +0x14C "SPEED BONUS!", titleFont
	bool m_showing;																// +0x150 fade out when m_animation ends
};
