// engine::Container: a component with child components.
#pragma once

#include <string>
#include <vector>

#include "Component.h"
#include "Rect.h"

namespace engine
{
	class Graphics;
	struct UpdateContext;

	// Component +0x00, members from +0x108, then the vtordisp (+0x128) and the Interface subobject (+0x12C): 0x130
	// bytes. A child holds a reference to itself while it is in the container (Component::onAddedTo/onRemovedFrom).
	// The constructor and getTypeName are inline: their only copies are in the game's objects (0x40DF00, 0x40DFF0,
	// next to the scalar deleting destructor 0x40DFA0), not in Container.cpp.
	class Container : public Component
	{
	public:
		// 0x40DF00
		Container()
		{
		}

		virtual ~Container();

		// 0x40DFF0
		virtual std::string getTypeName() const							// slot 24 (engine::Component)
		{
			return "Container";
		}

		virtual Component* getComponentAt(unsigned int flags, Vector2& point);	// slot 27: children last to first
		virtual void activate();										// slot 34: every child
		virtual void deactivate();										// slot 35: every child
		virtual void draw(Graphics& g);									// slot 36: the children with tree flag 1
		virtual void update(UpdateContext& context);					// slot 37: children; removes flag-0x10 ones
		virtual void updateBounds();									// slot 38: m_rect united with the children's
		virtual void dump(int indent);									// slot 39
		virtual void removeTreeFlags(unsigned int flags);				// slot 64 (engine::Component)

		virtual void removeChild(Component* child);						// slot 66
		virtual void onChildAdded(Component* child);					// slot 67 (0x492310 folded)
		virtual void onChildRemoved(Component* child);					// slot 68 (0x492310 folded)
		virtual void refreshMouseOver();								// slot 69: forwarded to the root Scene
		virtual void setRect(float left, float top, float right, float bottom);	// slot 70
		virtual void addChild(Component* child);						// slot 71
		virtual void removeAllChildren();								// slot 72
		virtual Component* getChild(int index);							// slot 73: 0 if out of range
		virtual int getChildCount() const;								// slot 74 (0x47B6D0 folded)

		std::vector<Component*>::iterator eraseChild(std::vector<Component*>::iterator where);

		std::vector<Component*> m_children;					// +0x108
		Rect m_rect;										// +0x118 own area (starts m_bounds in updateBounds)
	};
}
