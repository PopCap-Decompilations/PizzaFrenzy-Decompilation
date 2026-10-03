// CitySelectScreen: the city select screen (a postcard button per visited city, locks, Lisa's hint).
#pragma once

#include <string>
#include <vector>

#include "engine/RefPtr.h"
#include "engine/Screen.h"

namespace engine
{
	class Bitmap;
	class Component;
	class Container;
	class ImageButton;
	class TextTyper;
}

class City;

// res/screenLayouts/citySelectScreen.xml, created by the game (game+0xF0) and given its city list. One postcard
// ImageButton "citySelect%d" per city: a snapshot of the city rendered into res\menuAssets\postcardFrame.jpg with the
// city's name; lock overlays cover the cities not unlocked yet. The members end at +0x210; the vtordisp (+0x210) and
// the Interface subobject (+0x214) follow them (size 0x218).
class CitySelectScreen : public engine::Screen
{
public:
	CitySelectScreen();
	virtual ~CitySelectScreen();

	virtual void init(const std::vector<engine::RefPtr<City> >& cities);		// slot 95
	virtual void updateLocks();													// slot 96
	virtual void setCities(const std::vector<engine::RefPtr<City> >& cities);	// slot 97

	virtual void activate();										// slot 34 (engine::Component)
	virtual void onMouseEnter(std::string command);					// slot 0 of engine::ButtonListener (+0x144)
	virtual void onMouseLeave(std::string command);					// slot 1 of engine::ButtonListener (+0x144)

	engine::Bitmap* getPostcard(int index) const;
	engine::Bitmap* createPostcard(City* city);

	engine::RefPtr<engine::Container> m_postcardLayer;					// +0x1D4 added to "postcardGroup"; holds the postcard buttons
	std::vector<engine::RefPtr<engine::Bitmap> > m_postcardImages;		// +0x1D8 one composed postcard per city (createPostcard)
	std::vector<engine::RefPtr<engine::ImageButton> > m_postcardButtons;	// +0x1E8 "citySelect%d", 4 per row on a 122 x 105 grid
	engine::RefPtr<engine::Container> m_lisa;							// +0x1F8 "Lisa": parent of m_lisaText
	engine::RefPtr<engine::Component> m_lisaBubble;						// +0x1FC "LisaBubble" (only looked up)
	engine::RefPtr<engine::TextTyper> m_lisaText;						// +0x200 replaces "lisaText" (which is blanked)
	engine::RefPtr<engine::Bitmap> m_unknownPostcard;					// +0x204 createPostcard(NULL) ("???" card); never used
	engine::RefPtr<engine::ImageButton> m_hoveredButton;				// +0x208 postcard raised into m_topLayer by onMouseEnter
	engine::RefPtr<engine::Container> m_topLayer;						// +0x20C second container in "postcardGroup": lock overlays
																		//        and the hovered postcard
};
