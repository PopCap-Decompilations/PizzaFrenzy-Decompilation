#include "Graphics.h"

#include <deque>

#include "Color.h"
#include "Font.h"
#include "Point.h"
#include "Rect.h"
#include "WaveEffect.h"

namespace engine
{
	// the bit fields of GraphicsState::m_flags (initialised data at 0x521300..0x52131C)
	unsigned int GraphicsState::s_hAlignMask = 0x3;
	unsigned int GraphicsState::s_vAlignMask = 0xC;
	unsigned int GraphicsState::s_colorModeMask = 0x30;
	unsigned int GraphicsState::s_smoothMask = 0x40;
	int GraphicsState::s_hAlignShift = 0;
	int GraphicsState::s_vAlignShift = 2;
	int GraphicsState::s_colorModeShift = 4;
	int GraphicsState::s_smoothShift = 6;

	// 0x4778A0
	void Graphics::drawString(const char* text)
	{
		drawString(text, 0, 0);
	}

	// 0x4778C0
	void Graphics::drawChar(char c)
	{
		char text[2];
		text[0] = c;
		text[1] = 0;
		drawString(text);
	}

	// 0x4778E0
	GraphicsState::~GraphicsState()
	{
	}

	// 0x477A70
	GraphicsState::GraphicsState()
		: m_translation(0, 0), m_scale(1.0f, 1.0f), m_rotation(0), m_alpha(1.0f), m_color(0, 0, 0), m_flags(0)
	{
	}

	// 0x477BC0
	void Graphics::setTranslation(float x, float y)
	{
		m_states.back().m_translation.x = x;
		m_states.back().m_translation.y = y;
	}

	// 0x477C10
	void Graphics::translate(float dx, float dy)
	{
		m_states.back().m_translation.x += dx;
		m_states.back().m_translation.y += dy;
	}

	// 0x477C60
	const Vector2& Graphics::getTranslation() const
	{
		return m_states.back().m_translation;
	}

	// 0x477C90
	void Graphics::setScale(float sx, float sy)
	{
		m_states.back().m_scale.x = sx;
		m_states.back().m_scale.y = sy;
	}

	// 0x477CE0
	void Graphics::scale(float sx, float sy)
	{
		m_states.back().m_scale.x *= sx;
		m_states.back().m_scale.y *= sy;
	}

	// 0x477D30
	float Graphics::getScaleX() const
	{
		return m_states.back().m_scale.x;
	}

	// 0x477D60
	float Graphics::getScaleY() const
	{
		return m_states.back().m_scale.y;
	}

	// 0x477D90
	void Graphics::rotate(float angle)
	{
		m_states.back().m_rotation += angle;
	}

	// 0x477DC0
	float Graphics::getRotation() const
	{
		return m_states.back().m_rotation;
	}

	// 0x477DF0
	void Graphics::setAlpha(float alpha)
	{
		m_states.back().m_alpha = alpha;
	}

	// 0x477E20
	void Graphics::multiplyAlpha(float factor)
	{
		m_states.back().m_alpha *= factor;
	}

	// 0x477E50
	float Graphics::getAlpha() const
	{
		return m_states.back().m_alpha;
	}

	// 0x477E80
	void Graphics::setColor(const Color& color)
	{
		m_states.back().m_color = color;
	}

	// 0x477EC0: the alpha is unchanged
	void Graphics::setColor(float r, float g, float b)
	{
		m_states.back().m_color.set(r, g, b);
	}

	// 0x477EF0
	void Graphics::setColor(float r, float g, float b, float a)
	{
		m_states.back().m_color.set(r, g, b, a);
	}

	// 0x477F20
	const Color& Graphics::getColor() const
	{
		return m_states.back().m_color;
	}

	// 0x477F50
	void Graphics::setColorMode(int mode)
	{
		GraphicsState& state = m_states.back();
		state.m_flags = ((mode << GraphicsState::s_colorModeShift) & GraphicsState::s_colorModeMask)
			| (state.m_flags & ~GraphicsState::s_colorModeMask);
	}

	// 0x477FA0
	int Graphics::getColorMode() const
	{
		return (m_states.back().m_flags & GraphicsState::s_colorModeMask) >> GraphicsState::s_colorModeShift;
	}

	// 0x477FD0
	void Graphics::setFont(Font* font)
	{
		m_states.back().m_font = font;
	}

	// 0x478030
	Font* Graphics::getFont() const
	{
		return m_states.back().m_font;
	}

	// 0x478060
	void Graphics::setWave(WaveEffect* wave)
	{
		m_states.back().m_wave = wave;
	}

	// 0x4780C0
	WaveEffect* Graphics::getWave() const
	{
		return m_states.back().m_wave;
	}

	// 0x4780F0
	void Graphics::setHAlign(int align)
	{
		GraphicsState& state = m_states.back();
		state.m_flags = ((align << GraphicsState::s_hAlignShift) & GraphicsState::s_hAlignMask)
			| (state.m_flags & ~GraphicsState::s_hAlignMask);
	}

	// 0x478140
	void Graphics::setVAlign(int align)
	{
		GraphicsState& state = m_states.back();
		state.m_flags = ((align << GraphicsState::s_vAlignShift) & GraphicsState::s_vAlignMask)
			| (state.m_flags & ~GraphicsState::s_vAlignMask);
	}

	// 0x478190
	void Graphics::setSmoothing(int smooth)
	{
		GraphicsState& state = m_states.back();
		state.m_flags = ((smooth << GraphicsState::s_smoothShift) & GraphicsState::s_smoothMask)
			| (state.m_flags & ~GraphicsState::s_smoothMask);
	}

	// 0x4781E0
	int Graphics::getSmoothing() const
	{
		return (m_states.back().m_flags & GraphicsState::s_smoothMask) >> GraphicsState::s_smoothShift;
	}

	// 0x478210
	void Graphics::setClipRect(const IntRect& rect)
	{
		m_states.back().m_clipRect = rect;
	}

	// 0x478240
	const IntRect& Graphics::getClipRect() const
	{
		return m_states.back().m_clipRect;
	}

	// 0x478280
	Graphics::~Graphics()
	{
		while (!m_states.empty())
			m_states.pop_back();
	}

	// 0x478320: glyph by glyph with the state's font, (x, y) placed by the alignment. The alignment factors have no
	// default case (alignment 3 leaves the factor unset), and a character without a glyph advances by the unscaled
	// space width, as in the original.
	void Graphics::drawString(const char* text, float x, float y)
	{
		Font* font = getFont();
		if (font == 0)
			return;

		float align;
		switch ((m_states.back().m_flags & GraphicsState::s_hAlignMask) >> GraphicsState::s_hAlignShift)
		{
		case 0:
			align = 0.0f;
			break;
		case 1:
			align = 0.5f;
			break;
		case 2:
			align = 1.0f;
			break;
		}
		float scaleX = getScaleX();
		x -= font->getStringWidth(text) * scaleX * align;

		switch ((m_states.back().m_flags & GraphicsState::s_vAlignMask) >> GraphicsState::s_vAlignShift)
		{
		case 0:
			align = 0.0f;
			break;
		case 1:
			align = 0.5f;
			break;
		case 2:
			align = 1.0f;
			break;
		}
		float scaleY = getScaleY();
		y -= font->getHeight() * scaleY * align;

		switch ((m_states.back().m_flags & GraphicsState::s_vAlignMask) >> GraphicsState::s_vAlignShift)
		{
		case 0:
			y += font->getTopOffset() * scaleY;
			break;
		case 2:
			y -= font->getTopOffset() * scaleY;
			break;
		}

		for (; *text != 0; ++text)
		{
			char c = *text;
			if (font->hasGlyph(c))
			{
				x += font->getCharOffset(c) * scaleX;
				drawImage(font->getImage(), font->getGlyphRect(c), x, y);
				x += font->getCharAdvance(c) * scaleX;
			}
			else
			{
				x += font->getSpaceWidth();
			}
		}
	}

	// 0x4785A0: the stack is not checked; the caller pairs it with pushState
	void Graphics::popState()
	{
		m_states.pop_back();
	}

	// 0x478830: one default state
	Graphics::Graphics()
	{
		m_states.push_back(GraphicsState());
	}

	// 0x4788D0: a copy of the current state
	void Graphics::pushState()
	{
		m_states.push_back(m_states.back());
	}
}
