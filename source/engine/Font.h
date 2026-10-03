// engine::Font, a bitmap font read from res/fonts/*.xml, and engine::FontXmlHandler, the handler that reads it.
#pragma once

#include <string>

#include "Object.h"
#include "Rect.h"
#include "RefPtr.h"
#include "XmlDefaultHandler.h"

namespace engine
{
	class Application;
	class Bitmap;

	// One glyph sheet, per-character a/b/c spacing and glyph source rects. The XML gives no rects: addGlyph packs the
	// glyphs left to right in rows of the sheet (a gap on every side, rows height + 2*gap high), so the glyph order of
	// the file matters. Created by Application::createFont/createDiskFont. Object +0x00, members from +0x0C, then the
	// vtordisp (+0x1C38) and the Interface subobject (+0x1C3C): 0x1C40 bytes.
	class Font : public Object
	{
	public:
		Font(Application* application);
		virtual ~Font();

		void loadResource(const char* name);
		void load(const char* fileName);
		void addGlyph(int ch, int a, int b, int c);

		bool hasGlyph(char c) const;
		int getStringWidth(const char* text) const;
		int getCharWidth(char c) const;
		int getHeight() const;
		int getSpaceWidth() const;
		int getTopOffset() const;				// -m_topOverlap
		int getLineHeight() const;				// height - topOverlap - bottomOverlap
		int getCharOffset(char c) const;		// a - leftOverlap
		int getCharAdvance(char c) const;		// b + c - rightOverlap
		const IntRect& getGlyphRect(char c) const;
		Bitmap* getImage() const;				// folded 0x411970 (return the pointer at +0x10), called by Graphics::drawString

		Application* m_application;				// +0x0C opens the font XML (resource stream or loadXml)
		RefPtr<Bitmap> m_image;					// +0x10 glyph sheet ("image" attribute)
		int m_a[256];							// +0x14 per char "a": space before the glyph
		int m_b[256];							// +0x414 per char "b": glyph width in the sheet
		int m_c[256];							// +0x814 per char "c": space after the glyph
		IntRect m_glyphRects[256];				// +0xC14 source rect per char in pixels, packed by addGlyph (empty: no glyph)
		int m_height;							// +0x1C14 "height"
		int m_spaceWidth;						// +0x1C18 "spaceWidth": advance of chars without a glyph
		int m_gap;								// +0x1C1C "gap": padding around each glyph in the sheet
		int m_packX;							// +0x1C20 packing cursor
		int m_packY;							// +0x1C24 packing cursor (row top)
		int m_leftOverlap;						// +0x1C28 "leftOverlap"
		int m_rightOverlap;						// +0x1C2C "rightOverlap"
		int m_topOverlap;						// +0x1C30 "topOverlap"
		int m_bottomOverlap;					// +0x1C34 "bottomOverlap"
	};

	// The handler Font::load and Font::loadResource build on the stack: <font face image height spaceWidth gap
	// leftOverlap rightOverlap topOverlap bottomOverlap> (face is read and ignored) and <glyph char a b c>.
	// XmlDefaultHandler +0x00, m_font +0x14, then the vtordisp (+0x18) and the Interface subobject (+0x1C): 0x20 bytes.
	// Implicit destructor: its copy 0x47E620 is a bare jump to ~XmlDefaultHandler without vfptr stores.
	class FontXmlHandler : public XmlDefaultHandler
	{
	public:
		// always inlined (Font::loadResource, Font::load)
		FontXmlHandler(Font* font)
			: m_font(font)
		{
		}

		virtual void startElement(const std::string& name, const Properties& attributes);	// slot 0 (engine::XmlHandler)

		Font* m_font;							// +0x14 font being filled
	};
}
