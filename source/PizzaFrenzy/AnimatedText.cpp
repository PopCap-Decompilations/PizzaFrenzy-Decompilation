// AnimatedLetter and AnimatedText: text whose letters pop in one after another.
#include "AnimatedText.h"

#include "engine/Font.h"
#include "engine/Oscillator.h"
#include "engine/ParticleSystem.h"
#include "engine/StringUtil.h"
#include "engine/TextItem.h"

// 0x42AB80
void AnimatedLetter::update(engine::UpdateContext& context)
{
	switch (m_state)
	{
	case 2:
		m_timer -= context.elapsed;
		if (m_timer < 0.0f)
		{
			m_timer = m_duration;
			m_state = 0;
		}
		break;
	case 3:
		m_timer -= context.elapsed;
		if (m_timer < 0.0f)
		{
			m_timer = m_duration;
			m_state = 1;
		}
		break;
	case 0:
	{
		m_timer -= context.elapsed;
		if (m_timer < 0.0f)
			m_timer = 0.0f;
		float scale = 1.0f - m_timer / m_duration;
		setScale(scale, scale);
		if (m_timer == 0.0f)
		{
			if (m_holdTime > 0.0f)
			{
				m_state = 5;
				m_timer = m_holdTime;
			}
			else
			{
				m_state = 4;
			}
		}
		break;
	}
	case 1:
	{
		m_timer -= context.elapsed;
		if (m_timer < 0.0f)
			m_timer = 0.0f;
		float scale = m_timer / m_duration;
		setScale(scale, scale);
		if (m_timer == 0.0f)
		{
			m_state = 4;
			setVisible(false);
		}
		break;
	}
	case 5:
		m_timer -= context.elapsed;
		if (m_timer < 0.0f)
		{
			if (m_delay > 0.0f)
			{
				m_timer = m_delay;
				m_state = 3;
			}
			else
			{
				m_timer = m_duration;
				m_state = 1;
			}
		}
		break;
	}
	engine::Container::update(context);
}

// 0x42ADD0
AnimatedLetter::~AnimatedLetter()
{
	m_particles = NULL;
	m_text = NULL;
	m_unusedRef = NULL;
}

// 0x42AF20
void AnimatedLetter::show(engine::Vector2 position)
{
	if (m_isSpace)
		return;
	setPosition(position);
	setFlags(4);
	m_text->setVisible(true);
	setScale(0.0f, 0.0f);
	setVisible(true);
	if (m_delay > 0.0f)
	{
		m_timer = m_delay;
		m_state = 2;
	}
	else
	{
		m_timer = m_duration;
		m_state = 0;
	}
}

// 0x42AFB0
void AnimatedLetter::hide()
{
	if (m_state == 4 || m_state == 0)
	{
		if (m_particles)
		{
			m_particles->stop();
			m_particles->start();
		}
		m_text->setVisible(false);
		m_state = 7;
	}
}

// 0x42B020
int AnimatedLetter::setup(char c, engine::Font* font, engine::ParticleSystemDef* effect, float delay)
{
	m_char = c;
	m_text = new engine::TextItem();
	addChild(m_text);
	m_text->setFont(font);
	m_text->setXAlign(1);
	std::string text;
	engine::format(text, "%c", c);
	m_text->setText(text);
	m_text->setYAlign(1);
	m_text->setPosition(-3.0f, 0.0f);
	if (effect)
	{
		m_particles = new engine::ParticleSystem();
		m_particles->load(effect);
		addChild(m_particles);
	}
	setVisible(false);
	m_delay = delay;
	if (c == ' ')
		m_isSpace = true;
	return font->getCharWidth(c);
}

// 0x42B230
AnimatedLetter::AnimatedLetter()
{
	m_particles = NULL;
	m_text = NULL;
	m_unusedRef = NULL;
	m_timer = 0.0f;
	m_delay = 0.0f;
	m_holdTime = 0.0f;
	m_isSpace = false;
	m_duration = 0.2f;
	m_state = 7;
}

// 0x42B350
void AnimatedText::show(engine::Vector2 center)
{
	setPosition(center.x - m_width * 0.5f, center.y);
	engine::Vector2 position;
	position.set(0.0f, 0.0f);
	for (unsigned int i = 0; i < m_letters.size(); i++)
	{
		AnimatedLetter* letter = m_letters[i];
		position.x += m_font->getCharOffset(letter->m_char);
		m_letters[i]->show(position);
		position.x = position.x + m_font->getCharAdvance(letter->m_char) + m_spacing;
	}
}

// 0x42B460
void AnimatedText::hide()
{
	for (unsigned int i = 0; i < m_letters.size(); i++)
		m_letters[i]->hide();
}

// 0x42B4A0
AnimatedText::~AnimatedText()
{
	for (unsigned int i = 0; i < m_letters.size(); i++)
		m_letters[i]->release();
	m_letters.clear();
}

// 0x42B800
AnimatedText::AnimatedText()
{
	setBlendMode(1);
}

// 0x42B8A0
void AnimatedText::setText(const std::string& text, float letterDelay, float spacing, engine::Font* font, engine::ParticleSystemDef* effect, float startDelay, float waveAmplitude)
{
	removeAllChildren();
	m_letters.clear();
	m_font = font;
	m_width = 0.0f;
	for (unsigned int i = 0; i < text.size(); i++)
	{
		AnimatedLetter* letter = new AnimatedLetter();
		addChild(letter);
		m_width += letter->setup(text[i], font, effect, (float)i * letterDelay + startDelay) + spacing;
		if (waveAmplitude > 0.0f)
			letter->addAnimator(new engine::Oscillator(0.0f, 2.0f, 0.0f, font->getHeight() * waveAmplitude, (float)i * 0.1f));
		letter->addRef();
		m_letters.push_back(letter);
	}
	m_spacing = spacing;
}
