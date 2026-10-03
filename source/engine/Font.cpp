#include "Font.h"

#include "Application.h"
#include "InputStreamReader.h"
#include "Properties.h"
#include "Surface.h"
#include "Xml.h"

namespace engine
{
	// 0x411970 (folded)
	Bitmap* Font::getImage() const
	{
		return m_image;
	}

	// 0x47E630
	bool Font::hasGlyph(char c) const
	{
		return !m_glyphRects[c].isEmpty();
	}

	// 0x47E650
	int Font::getStringWidth(const char* text) const
	{
		int width = 0;
		for (; *text != '\0'; text++)
			width += getCharWidth(*text);
		return width;
	}

	// 0x47E6C0
	int Font::getCharWidth(char c) const
	{
		if (hasGlyph(c))
			return m_a[c] + m_b[c] + m_c[c] - m_rightOverlap - m_leftOverlap;
		return m_spaceWidth;
	}

	// 0x47E720
	int Font::getHeight() const
	{
		return m_height;
	}

	// 0x47E730
	int Font::getSpaceWidth() const
	{
		return m_spaceWidth;
	}

	// 0x47E740
	int Font::getTopOffset() const
	{
		return -m_topOverlap;
	}

	// 0x47E750
	int Font::getLineHeight() const
	{
		return m_height - m_bottomOverlap - m_topOverlap;
	}

	// 0x47E770
	int Font::getCharOffset(char c) const
	{
		return m_a[c] - m_leftOverlap;
	}

	// 0x47E790
	int Font::getCharAdvance(char c) const
	{
		return m_b[c] + m_c[c] - m_rightOverlap;
	}

	// 0x47E7C0
	const IntRect& Font::getGlyphRect(char c) const
	{
		return m_glyphRects[c];
	}

	// 0x47E7E0
	Font::~Font()
	{
	}

	// 0x47E8D0
	Font::Font(Application* application)
		: m_application(application)
	{
	}

	// 0x47E9A0
	void Font::loadResource(const char* name)
	{
		Reader* reader = m_application->getDiskReader(name);
		reader->addRef();
		XmlParseScope scope;
		FontXmlHandler handler(this);
		parseXml(&handler, reader);
		reader->release();
	}

	// 0x47EAD0
	void Font::addGlyph(int ch, int a, int b, int c)
	{
		m_a[ch] = a;
		m_b[ch] = b;
		m_c[ch] = c;
		if (m_packX + m_gap * 2 + b > m_image->getWidth())
		{
			m_packX = 0;
			m_packY += m_height + m_gap * 2;
		}
		IntRect& rect = m_glyphRects[ch];
		rect.left = m_packX + m_gap;
		rect.right = rect.left + b;
		rect.top = m_packY + m_gap;
		rect.bottom = rect.top + m_height;
		m_packX += b + m_gap * 2;
	}

	// 0x47EB90
	void Font::load(const char* fileName)
	{
		FontXmlHandler handler(this);
		m_application->loadXml(fileName, &handler);
	}

	// 0x47ECA0
	void FontXmlHandler::startElement(const std::string& name, const Properties& attributes)
	{
		if (name == "font")
		{
			std::string face = attributes.getString("face", "");
			std::string image = attributes.getString("image", "");
			int height = attributes.getInt("height", 0);
			int spaceWidth = attributes.getInt("spaceWidth", 0);
			int gap = attributes.getInt("gap", 0);
			int leftOverlap = attributes.getInt("leftOverlap", 0);
			int rightOverlap = attributes.getInt("rightOverlap", 0);
			int topOverlap = attributes.getInt("topOverlap", 0);
			int bottomOverlap = attributes.getInt("bottomOverlap", 0);
			m_font->m_image = getApplication()->getImage(image.c_str());
			m_font->m_height = height;
			m_font->m_spaceWidth = spaceWidth;
			m_font->m_gap = gap;
			m_font->m_leftOverlap = leftOverlap;
			m_font->m_rightOverlap = rightOverlap;
			m_font->m_topOverlap = topOverlap;
			m_font->m_bottomOverlap = bottomOverlap;
			m_font->m_packX = 0;
			m_font->m_packY = 0;
		}
		else if (name == "glyph")
		{
			int ch = attributes.getInt("char", 0);
			int a = attributes.getInt("a", 0);
			int b = attributes.getInt("b", 0);
			int c = attributes.getInt("c", 0);
			m_font->addGlyph(ch, a, b, c);
		}
	}
}
