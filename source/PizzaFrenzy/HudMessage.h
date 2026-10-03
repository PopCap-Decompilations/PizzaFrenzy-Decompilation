// HudMessage: the HUD's pop-up message (titleFont.xml) that grows, rises and fades out within 2 s.
#pragma once

#include <string>

#include "engine/Container.h"
#include "engine/RefPtr.h"

namespace engine
{
	class TextItem;
}

class AnimatedContainer;

// Built by GameScreen::init (its m_message), shown by GameScreen::showMessage. Implicit destructor (0x42C090).
class HudMessage : public engine::Container
{
public:
	HudMessage();

	void setText(const std::string& text);
	void show();

	engine::RefPtr<AnimatedContainer> m_animation;		// +0x128 holds m_text; its "in" keys are the whole effect
	engine::RefPtr<engine::TextItem> m_text;			// +0x12C titleFont.xml, centred
};
