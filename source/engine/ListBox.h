// engine::ListModel (a list box's data), engine::UserListModel (the user profiles) and engine::ListBox.
#pragma once

#include <string>

#include "Color.h"
#include "Component.h"
#include "Object.h"
#include "Point.h"
#include "RefPtr.h"
#include "sigslot.h"

namespace engine
{
	class Graphics;
	class SoundHandle;
	class UserManager;

	// Abstract list data: item count, item text, and a signal emitted when they change (ListBox::setModel connects
	// to it). Implicit destructor (0x4922B0). Layout: Object +0x00, m_changed +0x0C, then the vtordisp and Interface
	// (0x24 bytes).
	class ListModel : public Object
	{
	public:
		ListModel();

		// slot 1: number of items
		virtual int getCount() = 0;
		// slot 2: text of an item
		virtual std::string getItem(int index) = 0;

		sigslot::signal0<> m_changed;			// +0x0C
	};

	// The user profiles of the UserManager singleton; a missing user shows ">_<". Implicit destructor (it shares
	// ListModel's deleting destructor). Layout: ListModel +0x00, m_users +0x1C, then the vtordisp and Interface
	// (0x28 bytes).
	class UserListModel : public ListModel
	{
	public:
		// m_users = UserManager::getInstance()
		UserListModel();

		// ListModel
		virtual int getCount();
		virtual std::string getItem(int index);

		UserManager* m_users;					// +0x1C
	};

	// List box component (layout element <listBox>, default name "_ListBox_"): draws the rows of a ListModel,
	// follows the hovered and selected rows and plays the over/active sounds; keeps the inherited type name
	// "Component". Layout: Component +0x00, has_slots<> +0x108, members +0x118, then the vtordisp and Interface
	// (0x1B0 bytes).
	class ListBox : public Component, public sigslot::has_slots<>
	{
	public:
		// background and selection text Color(0, 0, 0), text and selection background Color(1, 1, 1) (the int
		// constructor: 1/255, not white), highlight Color(1, 1, 0), selection highlight Color(0, 0, 1); no hovered
		// or selected row; flags 3
		ListBox();
		// destroy() on both sounds
		virtual ~ListBox();

		// Component
		// background, then the rows: the selected one on the selection background in the selection colours
		virtual void draw(Graphics& g);
		// bounds = position .. position + m_size
		virtual void updateBounds();
		// selects the clicked row; on a change emits m_selectionChanged and plays m_activeSound (Component's
		// signals and hooks are not called)
		virtual void mouseDown(const MouseEvent& event);
		// follows the hovered row; replays m_overSound when it changes to a row
		virtual void mouseMove(const MouseEvent& event);
		// m_hoverIndex = -1
		virtual void mouseLeave(const MouseEvent& event);

		// the slot connected to the model's m_changed: addTreeFlags(8)
		void onModelChanged();
		void setSize(float width, float height);
		void setTextColor(const Color& color);
		const Color& getTextColor() const;
		void setBackgroundColor(const Color& color);
		const Color& getBackgroundColor() const;
		void setHighlightColor(const Color& color);
		const Color& getHighlightColor() const;
		void setSelectionBackgroundColor(const Color& color);
		const Color& getSelectionBackgroundColor() const;
		void setSelectionTextColor(const Color& color);
		void setSelectionHighlightColor(const Color& color);
		void setIndent(int indent);
		int getSelectedIndex() const;
		// m_selectedIndex = index (body folded with EditBox::setCharFilter, 0x471C80)
		void setSelectedIndex(int index);
		// loads each non-empty sound (Application slot 22) and creates its handle (slot 24)
		void setSounds(const std::string& overSound, const std::string& activeSound);
		// m_model->getCount()
		int getItemCount() const;
		// (int)(m_size.y / font height), a font height of 1 without a font
		int getVisibleRowCount() const;
		// the row under a point (local y / font height), -1 outside the rows
		int getItemAt(const Vector2& point) const;
		// connects onModelChanged to the model's m_changed, then addTreeFlags(8)
		void setModel(ListModel* model);

		sigslot::signal1<int> m_selectionChanged;	// +0x118 emitted with the new index when a click changes it
		Vector2 m_size;							// +0x128 width, height
		Color m_backgroundColor;				// +0x130
		Color m_textColor;						// +0x140
		Color m_highlightColor;					// +0x150 text colour of the hovered row
		Color m_selectionBackgroundColor;		// +0x160
		Color m_selectionTextColor;				// +0x170
		Color m_selectionHighlightColor;		// +0x180 text colour of the selected row when hovered
		int m_indent;							// +0x190 text x offset
		RefPtr<ListModel> m_model;				// +0x194
		int m_hoverIndex;						// +0x198 -1
		int m_selectedIndex;					// +0x19C -1
		SoundHandle* m_overSound;				// +0x1A0 played when the hovered row changes
		SoundHandle* m_activeSound;				// +0x1A4 played when the selection changes
	};
}
