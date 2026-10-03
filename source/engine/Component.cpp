#include "Component.h"

#include <algorithm>
#include <stdio.h>
#include <string>
#include <vector>

#include "Animator.h"
#include "Application.h"
#include "Container.h"
#include "Font.h"
#include "Graphics.h"
#include "WaveEffect.h"

namespace engine
{
	// 0x467A50
	const Vector2& Component::getPosition() const
	{
		return m_position;
	}

	// 0x467A60
	Vector2 Component::getScreenPosition() const
	{
		if (m_parent)
			return m_parent->getScreenPosition() + m_position;
		return m_position;
	}

	// 0x467AB0
	const Rect& Component::getBounds() const
	{
		return m_bounds;
	}

	// 0x467AC0
	void Component::setPosition(float x, float y)
	{
		m_position.set(x, y);
		addTreeFlags(8);
	}

	// 0x467AF0
	void Component::setPosition(const Vector2& position)
	{
		m_position.set(position.x, position.y);
		addTreeFlags(8);
	}

	// 0x467B20
	void Component::setAlpha(float alpha)
	{
		m_alpha = alpha;
	}

	// 0x467B30
	float Component::getAlpha() const
	{
		return m_alpha;
	}

	// 0x467B40
	void Component::setScale(float scale)
	{
		setScale(scale, scale);
	}

	// 0x467B50
	void Component::setScale(float scaleX, float scaleY)
	{
		m_scale.x = scaleX;
		m_scale.y = scaleY;
		addTreeFlags(8);
	}

	// 0x467B70
	const Vector2& Component::getScale() const
	{
		return m_scale;
	}

	// 0x467B80
	void Component::setRotation(float rotation)
	{
		m_rotation = rotation;
	}

	// 0x467B90
	void Component::setColor(const Color& color)
	{
		m_color = color;
	}

	// 0x467BB0
	const Color& Component::getColor() const
	{
		return m_color;
	}

	// 0x467BC0
	void Component::setColor(float red, float green, float blue)
	{
		m_color.set(red, green, blue);
	}

	// 0x467BD0
	void Component::setColorMode(int mode)
	{
		m_colorMode = mode;
	}

	// 0x467BE0
	void Component::setBlendMode(int mode)
	{
		m_blendMode = mode;
	}

	// 0x467BF0
	bool Component::hitTest(const Vector2& point)
	{
		return m_bounds.contains(point.x, point.y);
	}

	// 0x467C10
	void Component::screenToLocal(Vector2& point)
	{
		if (m_parent)
		{
			m_parent->screenToLocal(point);
			point.x -= m_position.x;
			point.y -= m_position.y;
		}
	}

	// 0x467C40
	void Component::screenToParent(Vector2& point)
	{
		if (m_parent)
			m_parent->screenToLocal(point);
	}

	// 0x467C60
	void Component::update(UpdateContext& context)
	{
		m_screenPosition.x = m_position.x + context.origin.x;
		m_screenPosition.y = m_position.y + context.origin.y;
		updateAnimators(context);
	}

	// 0x467C90
	void Component::onMouseDown(const Vector2& position)
	{
		onMouseDown();
	}

	// 0x467CA0
	void Component::onMouseUp(const Vector2& position)
	{
		onMouseUp();
	}

	// 0x467CB0
	void Component::onRightMouseDown(const Vector2& position)
	{
		onRightMouseDown();
	}

	// 0x467CC0
	void Component::onRightMouseUp(const Vector2& position)
	{
		onRightMouseUp();
	}

	// 0x467CD0
	void Component::onMouseEnter(const Vector2& position)
	{
		onMouseEnter();
	}

	// 0x467CE0
	void Component::onMouseLeave(const Vector2& position)
	{
		onMouseLeave();
	}

	// 0x467CF0
	void Component::onAddedTo(Container* parent)
	{
		m_parent = parent;
		addRef();
	}

	// 0x467D10
	void Component::onRemovedFrom(Container* parent)
	{
		m_parent = 0;
		release();
	}

	// 0x467D30
	void Component::setFlags(unsigned int flags)
	{
		m_flags |= flags;
		addTreeFlags(flags);
	}

	// 0x467D50
	void Component::clearFlags(unsigned int flags)
	{
		m_flags &= ~flags;
		removeTreeFlags(flags);
	}

	// 0x467D70
	void Component::maskFlags(unsigned int flags)
	{
		m_maskFlags |= flags;
	}

	// 0x467D80
	void Component::unmaskFlags(unsigned int flags)
	{
		m_maskFlags &= ~flags;
	}

	// 0x467DA0
	void Component::addTreeFlags(unsigned int flags)
	{
		if (flags & ~m_treeFlags)
		{
			m_treeFlags |= flags;
			if (m_parent)
				m_parent->addTreeFlags(flags);
		}
	}

	// 0x467DD0
	void Component::removeTreeFlags(unsigned int flags)
	{
		if (flags & m_treeFlags & ~m_flags)
		{
			m_treeFlags = (m_treeFlags & ~flags) | m_flags;
			if (m_parent)
				m_parent->removeTreeFlags(flags);
		}
	}

	// 0x467E10
	bool Component::testTreeFlags(unsigned int flags) const
	{
		return (m_treeFlags & ~m_maskFlags & flags) != 0;
	}

	// 0x467E30
	bool Component::testFlags(unsigned int flags) const
	{
		return (m_flags & ~m_maskFlags & flags) != 0;
	}

	// 0x467E50
	unsigned int Component::getTreeFlags() const
	{
		return m_treeFlags;
	}

	// 0x467E60
	const std::string& Component::getName() const
	{
		return m_name;
	}

	// 0x467E70
	Component* Component::getComponentAt(unsigned int flags, Vector2& point)
	{
		if (testTreeFlags(flags) && hitTest(point))
			return this;
		return 0;
	}

	// 0x467EB0
	void Component::setVisible(bool visible)
	{
		if (visible)
			unmaskFlags(1);
		else
			maskFlags(1);
	}

	// 0x467ED0
	bool Component::isVisible() const
	{
		return testTreeFlags(1);
	}

	// 0x467EF0
	void Component::setEnabled(bool enabled)
	{
		if (enabled)
			setFlags(2);
		else
			clearFlags(2);
	}

	// 0x467F20
	bool Component::isEnabled() const
	{
		return testFlags(2);
	}

	// 0x467F40
	void Component::setFont(Font* font)
	{
		m_font = font;
		addTreeFlags(8);
	}

	// 0x467F90
	void Component::setEffect(WaveEffect* effect)
	{
		m_effect = effect;
	}

	// 0x467FD0
	void Component::setupGraphics(Graphics& g)
	{
		g.translate(m_position.x * g.getScaleX(), m_position.y * g.getScaleY());
		g.scale(m_scale.x, m_scale.y);
		g.rotate(m_rotation);
		g.multiplyAlpha(m_alpha);
		if (m_font)
			g.setFont(m_font);
		if (m_colorMode)
		{
			g.setColor(m_color.r, m_color.g, m_color.b);
			g.setColorMode(m_colorMode);
		}
		if (m_blendMode)
			g.setSmoothing(m_blendMode);
		if (m_effect)
			g.setWave(m_effect);
	}

	// 0x468080
	void Component::updateAnimators(UpdateContext& context)
	{
		for (std::vector<Animator*>::iterator it = m_animators.begin(); it != m_animators.end(); ++it)
			(*it)->update(context, this);
	}

	// 0x4680C0
	void Component::mouseDown(const MouseEvent& event)
	{
		m_mouseDownSignal.emit(event);
		onMouseDown(event.position);
	}

	// 0x468100
	void Component::mouseUp(const MouseEvent& event)
	{
		m_mouseUpSignal.emit(event);
		onMouseUp(event.position);
	}

	// 0x468140
	void Component::rightMouseDown(const MouseEvent& event)
	{
		m_rightMouseDownSignal.emit(event);
		onRightMouseDown(event.position);
	}

	// 0x468180
	void Component::rightMouseUp(const MouseEvent& event)
	{
		m_rightMouseUpSignal.emit(event);
		onRightMouseUp(event.position);
	}

	// 0x4681C0
	void Component::mouseMove(const MouseEvent& event)
	{
		m_mouseMoveSignal.emit(event);
		onMouseMove(event.position);
	}

	// 0x468200
	void Component::mouseEnter(const MouseEvent& event)
	{
		m_mouseEnterSignal.emit(event);
		onMouseEnter(event.position);
	}

	// 0x468240
	void Component::mouseLeave(const MouseEvent& event)
	{
		m_mouseLeaveSignal.emit(event);
		onMouseLeave(event.position);
	}

	// 0x468280
	void Component::removeAnimator(Animator* animator)
	{
		std::vector<Animator*>::iterator it = std::find(m_animators.begin(), m_animators.end(), animator);
		if (it != m_animators.end())
		{
			m_animators.erase(it);
			animator->release();
		}
	}

	// 0x4682E0
	void Component::removeAllAnimators()
	{
		for (std::vector<Animator*>::iterator it = m_animators.begin(); it != m_animators.end(); ++it)
			(*it)->release();
		m_animators.clear();
	}

	// 0x468360
	void Component::setName(const std::string& name)
	{
		Application* app = getApplication();
		if (m_name != "")
			app->unregisterComponent(this);
		m_name = name;
		app->registerComponent(this);
	}

	// 0x4683B0
	void Component::dump(int indent)
	{
		static char s_line[256];

		Application* app = getApplication();
		std::string name = m_name;
		if (name.empty())
			name = "[unknown]";
		for (int i = 0; i < indent; i++)
			app->log("\t");
		const char* visible = isVisible() ? "true" : "false";
		const char* remove = (m_flags & 0x10) ? "DELETE" : "";
		sprintf(s_line, "<%s name=\"%s\" %s visible=\"%s\" flags=\"%x\" refCount=\"%d\" bounds=\"(%.2f,%.2f),(%.2f,%.2f)\" />\n",
			getTypeName().c_str(), name.c_str(), remove, visible, m_treeFlags, getRefCount(),
			m_bounds.left, m_bounds.top, m_bounds.right, m_bounds.bottom);
		app->log(s_line);
	}

	// 0x468950
	Component::~Component()
	{
		if (m_name != "")
			getApplication()->unregisterComponent(this);
		removeAllAnimators();
	}

	// 0x468B20
	std::string Component::getTypeName() const
	{
		return "Component";
	}

	// 0x468BA0
	Component::Component()
		: m_parent(0)
		, m_position(0.0f, 0.0f)
		, m_screenPosition(0.0f, 0.0f)
		, m_scale(1.0f, 1.0f)
		, m_rotation(0.0f)
		, m_alpha(1.0f)
		, m_colorMode(0)
		, m_blendMode(0)
		, m_flags(0)
		, m_treeFlags(0)
		, m_maskFlags(0)
	{
	}

	// 0x468DF0
	void Component::addAnimator(Animator* animator)
	{
		if (std::find(m_animators.begin(), m_animators.end(), animator) == m_animators.end())
		{
			animator->addRef();
			m_animators.push_back(animator);
		}
		setFlags(4);
	}

	// 0x450480 (folded)
	float Component::getX() const
	{
		return m_position.x;
	}

	// 0x450490 (folded)
	float Component::getY() const
	{
		return m_position.y;
	}

	// 0x4D0CA0 (folded)
	int Component::getColorMode() const
	{
		return m_colorMode;
	}

	// 0x4D0470 (folded)
	void Component::activate()
	{
	}

	// 0x4D0470 (folded)
	void Component::deactivate()
	{
	}

	// 0x4D0470 (folded)
	void Component::onMouseDown()
	{
	}

	// 0x4D0470 (folded)
	void Component::onMouseUp()
	{
	}

	// 0x4D0470 (folded)
	void Component::onRightMouseDown()
	{
	}

	// 0x4D0470 (folded)
	void Component::onRightMouseUp()
	{
	}

	// 0x492310 (folded)
	void Component::onMouseMove(const Vector2& position)
	{
	}

	// 0x4D0470 (folded)
	void Component::onMouseEnter()
	{
	}

	// 0x4D0470 (folded)
	void Component::onMouseLeave()
	{
	}

	// 0x411960 (folded)
	Container* Component::getParent() const
	{
		return m_parent;
	}
}
