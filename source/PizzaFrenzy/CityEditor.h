// CityEditor and CityEditorScreen: the developer city editor, of which only the helpers the game states call remain.
#pragma once

#include <windows.h>

#include "engine/Application.h"
#include "engine/Object.h"
#include "engine/RefPtr.h"

#include "CityMap.h"
#include "GameScreen.h"
#include "Tile.h"

namespace engine
{
	struct UpdateContext;
}

// The city editor (the game's +0xF8, used by LevelEditorState). The release build never creates it: no constructor,
// vtable or out-of-line method survives, only these inline helpers (emitted in the game's object), so its layout
// is known only where they touch it.
class CityEditor : public engine::Object
{
public:
	virtual void update(engine::UpdateContext& context) = 0;	// slot 1: per frame, from LevelEditorState::update (body not in the EXE)

	// 0x40DA60
	bool confirmDiscardChanges()
	{
		if (m_modified)
			return MessageBoxA(engine::getApplication()->getWindowHandle(),
				"There are unsaved changes\n are you sure you want to do this?\n", "Are you sure?", MB_OKCANCEL) == IDOK;
		return true;
	}

	// 0x40DA90
	bool hasCity() const
	{
		return m_city && m_city->getCols() > 0;
	}

	// 0x40DAB0
	void newCity()
	{
		m_city->create(28, 24, 25, g_zeroPoint, 0);
		m_view->rebuild();
		m_view->buildMap();
		m_modified = false;
	}

	unsigned char m_unknown0C[0x2D];		// +0x0C unknown
	bool m_modified;						// +0x39 unsaved changes
	engine::RefPtr<CityMap> m_city;			// +0x3C the city being edited
	engine::RefPtr<GameScreen> m_view;		// +0x40 its view
};

// The city editor's screen (the game's +0x108), never created either: a GameScreen (the components at +0x1E4 and
// +0x1E8 are its ground and air groups) with editor settings from +0x218.
class CityEditorScreen : public GameScreen
{
public:
	// 0x40DE40
	void setSnapToGrid(bool snap)
	{
		m_snapToGrid = snap;
	}

	// 0x40DE50: hides the ground and air layers
	void setToolbarHidden(bool hidden)
	{
		m_toolbarHidden = hidden;
		m_groundGroup->setVisible(!m_toolbarHidden);
		m_airGroup->setVisible(!m_toolbarHidden);
	}

	unsigned char m_unknown218[0x15];		// +0x218 unknown
	bool m_snapToGrid;						// +0x22D
	bool m_toolbarHidden;					// +0x22E
};
