// ToppingBookScreen: the topping book (two facing pages of topping pictures, levels and values; earned rank titles).
#pragma once

#include <map>
#include <string>

#include "engine/RefPtr.h"
#include "engine/Screen.h"

namespace engine
{
	class Bitmap;
	class Component;
	class Container;
	class HsvFilter;
	class Image;
	class TextItem;
}

class Topping;

// res/screenLayouts/toppingBook.xml, built by the game into game+0xE0 and opened by the "viewToppingBook" button.
// Each page shows one topping (picture, caption, level, value, next-level requirement, description, page number);
// the left page of the first spread lists the earned rank titles; toppings the player does not own are drawn as
// black silhouettes. The members end at +0x298; the vtordisp (+0x298) and the Interface subobject (+0x29C) follow
// them (0x2A0 bytes).
class ToppingBookScreen : public engine::Screen
{
public:
	ToppingBookScreen();
	virtual ~ToppingBookScreen();

	virtual void init();																// slot 95
	virtual void showPages();															// slot 96
	virtual void onButton(const std::string& name);									// slot 97
	virtual void showTopping(Topping* topping, int level, int side, int pageNumber);	// slot 98
	virtual void showTitles();															// slot 99

	virtual void activate();						// slot 34 (engine::Component), folded body 0x443BB0

	engine::RefPtr<engine::Container> m_leftTitles;						// +0x1D4 "leftTitles": the earned rank titles
	engine::RefPtr<engine::Component> m_pages[2];						// +0x1D8 "%sPage" (left, right)
	engine::RefPtr<engine::Image> m_toppingBgs[2];						// +0x1E0 "%sToppingBg": the large picture
	engine::RefPtr<engine::Image> m_toppingImages[2];					// +0x1E8 "%sTopping": the image or its silhouette
	engine::RefPtr<engine::Component> m_lockedStamps[2];				// +0x1F0 "%sLocked": new topping not owned yet
	engine::RefPtr<engine::TextItem> m_captionTexts[2];				// +0x1F8 "%sToppingCaptionText"
	engine::RefPtr<engine::TextItem> m_valueTexts[2];					// +0x200 "%sToppingValue"
	engine::RefPtr<engine::TextItem> m_levelTexts[2];					// +0x208 "%sToppingLevel"
	engine::RefPtr<engine::TextItem> m_nextTexts[2];					// +0x210 "%sToppingNext": next requirement or "N/A"
	engine::RefPtr<engine::TextItem> m_descTexts[2];					// +0x218 "%sToppingDesc"
	engine::RefPtr<engine::TextItem> m_pageTexts[2];					// +0x220 "%sToppingPage"
	engine::RefPtr<engine::Component> m_pageButtons[2];				// +0x228 "%sButton": "bookLeft"/"bookRight"
	std::map<Topping*, engine::RefPtr<engine::Bitmap> > m_silhouettes;	// +0x230 black copy of each topping's image (init)
	engine::RefPtr<engine::HsvFilter> m_silhouetteFilter;				// +0x23C (0, -1, -1): turns the images black
	std::string m_valueFormat;											// +0x240 layout text of "leftToppingValue"
	std::string m_levelFormat;											// +0x25C layout text of "leftToppingLevel"
	std::string m_pageFormat;											// +0x278 layout text of "leftToppingPage"
	int m_page;															// +0x294 left page: 0 = titles, else topping m_page-1; not set by the ctor
};
