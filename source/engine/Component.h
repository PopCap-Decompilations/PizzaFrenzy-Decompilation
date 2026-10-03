// engine::Component, the root of the scene graph, and the structs its virtual functions take: engine::UpdateContext
// (argument of every update) and engine::MouseEvent (argument of the mouse signals).
#pragma once

#include <string>
#include <vector>

#include "Color.h"
#include "Object.h"
#include "Point.h"
#include "Rect.h"
#include "RefPtr.h"
#include "sigslot.h"

namespace engine
{
	class Animator;
	class Component;
	class Container;
	class Font;
	class Graphics;
	class Scene;
	class WaveEffect;

	// Built per frame by the application (0x4BD210) and by ParticleSystem::advance; Container::update adds its
	// position to origin for its children and takes it off again. Wherever one is made the original constructs
	// origin with Vector2(0, 0) (0x475430), and 0x4BD210 stores elapsed = 0 before computing it: an inline
	// constructor.
	struct UpdateContext
	{
		UpdateContext()
			: origin(0.0f, 0.0f), elapsed(0.0f)
		{
		}

		Vector2 origin;							// +0x00
		float elapsed;							// +0x08 seconds since the last frame (capped at 0.33 by the application)
	};

	// Argument of the component mouse signals, held by engine::Scene (+0x140) with scene = this. No user-declared
	// constructor: the Scene constructor default-constructs position and sets buttons and scene itself.
	struct MouseEvent
	{
		Component* target;						// +0x00
		Vector2 position;						// +0x04 local to the target
		int buttons;							// +0x0C 1 left, 2 right
		Scene* scene;							// +0x10
	};

	// Object +0x00, members from +0x0C, then the vtordisp (+0x108) and the Interface subobject (+0x10C): 0x110 bytes.
	// Flag bits (m_flags, m_treeFlags, m_maskFlags): 1 draw, 2 mouse input (the Scene picks with 2), 4 update,
	// 8 bounds dirty (cleared by updateBounds), 0x10 remove at the parent's next update. m_treeFlags is the own flags
	// OR'ed with every descendant's; m_maskFlags suppresses flags (setVisible(false) and FadeTransition set 1,
	// deactivate sets 2).
	// MSVC groups overloaded virtuals and lays each group out in reverse declaration order: the overloads below are
	// declared in the reverse of their slot order.
	class Component : public Object
	{
	public:
		Component();
		virtual ~Component();

		virtual float getX() const;										// slot 1 (0x450480 folded)
		virtual float getY() const;										// slot 2 (0x450490 folded)
		virtual const Vector2& getPosition() const;						// slot 3
		virtual Vector2 getScreenPosition() const;						// slot 4
		virtual void setPosition(float x, float y);						// slot 6
		virtual void setPosition(const Vector2& position);				// slot 5
		virtual void setScale(float scale);								// slot 8: setScale(scale, scale)
		virtual void setScale(float scaleX, float scaleY);				// slot 7
		virtual const Vector2& getScale() const;						// slot 9
		virtual void setRotation(float rotation);						// slot 10
		virtual const Rect& getBounds() const;							// slot 11: in the parent's space
		virtual void setAlpha(float alpha);								// slot 12
		virtual float getAlpha() const;									// slot 13
		virtual void setFont(Font* font);								// slot 14
		virtual void setColor(const Color& color);						// slot 16
		virtual void setColor(float red, float green, float blue);		// slot 15
		virtual const Color& getColor() const;							// slot 17
		virtual void setColorMode(int mode);							// slot 18: 0 = colour not applied
		virtual int getColorMode() const;								// slot 19 (0x4D0CA0 folded)
		virtual void setBlendMode(int mode);							// slot 20
		virtual void setEffect(WaveEffect* effect);						// slot 21: handed to Graphics::setWave
		virtual void setName(const std::string& name);					// slot 22: registered with the Application
		virtual const std::string& getName() const;						// slot 23
		virtual std::string getTypeName() const;						// slot 24: the class name
		virtual void setupGraphics(Graphics& g);						// slot 25: transform, alpha, font, colour...
		virtual bool hitTest(const Vector2& point);						// slot 26
		virtual Component* getComponentAt(unsigned int flags, Vector2& point);	// slot 27: picking
		virtual void screenToLocal(Vector2& point);						// slot 28
		virtual void screenToParent(Vector2& point);					// slot 29
		virtual void setVisible(bool visible);							// slot 30
		virtual bool isVisible() const;									// slot 31
		virtual void setEnabled(bool enabled);							// slot 32
		virtual bool isEnabled() const;									// slot 33
		virtual void activate();										// slot 34 (0x4D0470 folded)
		virtual void deactivate();										// slot 35 (0x4D0470 folded)
		virtual void draw(Graphics& g) = 0;								// slot 36
		virtual void update(UpdateContext& context);					// slot 37
		virtual void updateBounds() = 0;								// slot 38
		virtual void dump(int indent);									// slot 39: debug print through Application::log
		virtual void mouseDown(const MouseEvent& event);				// slot 40: signal, then onMouseDown(position)
		virtual void onMouseDown();										// slot 42 (0x4D0470 folded)
		virtual void onMouseDown(const Vector2& position);				// slot 41
		virtual void mouseUp(const MouseEvent& event);					// slot 43
		virtual void onMouseUp();										// slot 45 (0x4D0470 folded)
		virtual void onMouseUp(const Vector2& position);				// slot 44
		virtual void rightMouseDown(const MouseEvent& event);			// slot 46
		virtual void onRightMouseDown();								// slot 48 (0x4D0470 folded)
		virtual void onRightMouseDown(const Vector2& position);			// slot 47
		virtual void rightMouseUp(const MouseEvent& event);				// slot 49
		virtual void onRightMouseUp();									// slot 51 (0x4D0470 folded)
		virtual void onRightMouseUp(const Vector2& position);			// slot 50
		virtual void mouseMove(const MouseEvent& event);				// slot 52
		virtual void onMouseMove(const Vector2& position);				// slot 53 (0x492310 folded)
		virtual void mouseEnter(const MouseEvent& event);				// slot 54
		virtual void onMouseEnter();									// slot 56 (0x4D0470 folded)
		virtual void onMouseEnter(const Vector2& position);				// slot 55
		virtual void mouseLeave(const MouseEvent& event);				// slot 57
		virtual void onMouseLeave();									// slot 59 (0x4D0470 folded)
		virtual void onMouseLeave(const Vector2& position);				// slot 58
		virtual void onAddedTo(Container* parent);						// slot 60: m_parent = parent, addRef
		virtual void onRemovedFrom(Container* parent);					// slot 61: m_parent = 0, release
		virtual Container* getParent() const;							// slot 62 (0x411960 folded)
		virtual void addTreeFlags(unsigned int flags);					// slot 63: propagates to the parent
		virtual void removeTreeFlags(unsigned int flags);				// slot 64
		virtual void updateAnimators(UpdateContext& context);			// slot 65

		void setFlags(unsigned int flags);
		void clearFlags(unsigned int flags);
		void maskFlags(unsigned int flags);
		void unmaskFlags(unsigned int flags);
		bool testTreeFlags(unsigned int flags) const;
		bool testFlags(unsigned int flags) const;
		unsigned int getTreeFlags() const;
		void removeAnimator(Animator* animator);
		void removeAllAnimators();
		void addAnimator(Animator* animator);

		Container* m_parent;								// +0x0C not ref-counted
		std::string m_name;									// +0x10 registered in Application::m_components
		Vector2 m_position;									// +0x2C local position
		Vector2 m_screenPosition;							// +0x34 m_position + context origin (update)
		Vector2 m_scale;									// +0x3C (1, 1)
		float m_rotation;									// +0x44
		float m_alpha;										// +0x48 1
		RefPtr<Font> m_font;								// +0x4C
		Color m_color;										// +0x50
		int m_colorMode;									// +0x60
		int m_blendMode;									// +0x64
		RefPtr<WaveEffect> m_effect;						// +0x68
		unsigned int m_flags;								// +0x6C own flags
		unsigned int m_treeFlags;							// +0x70 own flags | every descendant's
		unsigned int m_maskFlags;							// +0x74 suppressed flags
		Rect m_bounds;										// +0x78
		std::vector<Animator*> m_animators;					// +0x88 referenced (addRef) by addAnimator
		sigslot::signal1<const MouseEvent&> m_mouseDownSignal;		// +0x98
		sigslot::signal1<const MouseEvent&> m_mouseUpSignal;		// +0xA8
		sigslot::signal1<const MouseEvent&> m_rightMouseDownSignal;	// +0xB8
		sigslot::signal1<const MouseEvent&> m_rightMouseUpSignal;	// +0xC8
		sigslot::signal1<const MouseEvent&> m_mouseMoveSignal;		// +0xD8
		sigslot::signal1<const MouseEvent&> m_mouseEnterSignal;		// +0xE8
		sigslot::signal1<const MouseEvent&> m_mouseLeaveSignal;		// +0xF8
	};
}
