// engine::Scene: the root of the scene graph; dispatches the application's update, draw and mouse events.
#pragma once

#include <string>

#include "Component.h"
#include "Container.h"
#include "RefPtr.h"
#include "sigslot.h"

namespace engine
{
	class Graphics;
	struct Point;
	struct UpdateContext;

	// Container +0x00, sigslot::has_slots<> +0x128, members from +0x138, then the vtordisp (+0x154) and the
	// Interface subobject (+0x158): 0x15C bytes. Created by the application; while active it is connected to the
	// application's update and draw signals and to its mouse signals, tracks the component under the mouse and
	// the one capturing it (a pressed button) and sends them mouse events.
	class Scene : public Container, public sigslot::has_slots<>
	{
	public:
		Scene();
		virtual ~Scene();

		virtual std::string getTypeName() const;	// slot 24 (engine::Component): "Scene"
		virtual void activate();					// slot 34 (engine::Component): connects to the application
		virtual void deactivate();					// slot 35 (engine::Component): disconnects
		virtual void draw(Graphics& g);				// slot 36 (engine::Component): Container::draw (body folded: 0x471110)
		virtual void update(UpdateContext& context);	// slot 37 (engine::Component)
		virtual void refreshMouseOver();			// slot 69 (engine::Container): picks the component under the mouse

		void setCapture(Component* component);
		void releaseCapture();
		Component* updateMouseTarget(const Point& mousePos);
		void onMouseDown(const Point& pos);
		void onMouseUp(const Point& pos);
		void onRightMouseDown(const Point& pos);
		void onRightMouseUp(const Point& pos);
		void onMouseMove(const Point& pos);

		RefPtr<Component> m_hoverComponent;		// +0x138 component under the mouse
		RefPtr<Component> m_captureComponent;	// +0x13C component capturing the mouse (pressed button)
		MouseEvent m_mouseEvent;				// +0x140 passed to the mouse handlers; scene = this
	};
}
