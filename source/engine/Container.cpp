#include "Container.h"

#include <algorithm>
#include <stdio.h>
#include <string>
#include <vector>

#include "Application.h"
#include "Graphics.h"

namespace engine
{
	// 0x4670E0
	void Container::refreshMouseOver()
	{
		if (m_parent)
			m_parent->refreshMouseOver();
	}

	// 0x467100
	void Container::setRect(float left, float top, float right, float bottom)
	{
		m_rect.set(left, top, right, bottom);
	}

	// 0x467110
	Component* Container::getComponentAt(unsigned int flags, Vector2& point)
	{
		if (testTreeFlags(flags) && hitTest(point))
		{
			point.x -= m_position.x;
			point.y -= m_position.y;
			Vector2 local(point);
			for (std::vector<Component*>::reverse_iterator it = m_children.rbegin(); it != m_children.rend(); ++it)
			{
				Component* found = (*it)->getComponentAt(flags, point);
				if (found)
					return found;
				point = local;
			}
			if (testFlags(flags))
				return this;
		}
		return 0;
	}

	// 0x4671B0
	void Container::activate()
	{
		for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
			(*it)->activate();
	}

	// 0x4671E0
	void Container::deactivate()
	{
		for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
			(*it)->deactivate();
	}

	// 0x467210
	void Container::updateBounds()
	{
		m_bounds = m_rect;
		for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
		{
			if ((*it)->testTreeFlags(8))
				(*it)->updateBounds();
			m_bounds.unite((*it)->getBounds());
		}
		m_bounds.scale(m_scale.x, m_scale.y);
		m_bounds.normalize();
		m_bounds.offset(m_position.x, m_position.y);
		removeTreeFlags(8);
	}

	// 0x4672B0
	void Container::draw(Graphics& g)
	{
		for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
		{
			if ((*it)->testTreeFlags(1))
			{
				g.pushState();
				(*it)->setupGraphics(g);
				(*it)->draw(g);
				g.popState();
			}
		}
	}

	// 0x467320
	void Container::removeTreeFlags(unsigned int flags)
	{
		if (flags & m_treeFlags & ~m_flags)
		{
			m_treeFlags = (m_treeFlags & ~flags) | m_flags;
			for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
				m_treeFlags |= (*it)->getTreeFlags();
			if (m_parent)
				m_parent->removeTreeFlags(flags);
		}
	}

	// 0x467390
	std::vector<Component*>::iterator Container::eraseChild(std::vector<Component*>::iterator where)
	{
		onChildRemoved(*where);
		(*where)->onRemovedFrom(this);
		where = m_children.erase(where);
		removeTreeFlags(~m_flags);
		addTreeFlags(8);
		return where;
	}

	// 0x467400
	void Container::removeAllChildren()
	{
		for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->onRemovedFrom(this);
			onChildRemoved(*it);
		}
		m_children.clear();
		removeTreeFlags(~m_flags);
		addTreeFlags(8);
	}

	// 0x467480
	Container::~Container()
	{
		removeAllAnimators();
		removeAllChildren();
	}

	// 0x467540
	void Container::update(UpdateContext& context)
	{
		Component::update(context);
		context.origin.x += m_position.x;
		context.origin.y += m_position.y;
		std::vector<Component*>::iterator it = m_children.begin();
		while (it != m_children.end())
		{
			Component* child = *it;
			if (child->testTreeFlags(0x14))
				child->update(context);
			if (child->testFlags(0x10))
			{
				child->clearFlags(0x10);
				it = eraseChild(it);
			}
			else
				++it;
		}
		context.origin.x -= m_position.x;
		context.origin.y -= m_position.y;
	}

	// 0x467630
	void Container::removeChild(Component* child)
	{
		std::vector<Component*>::iterator it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end())
			eraseChild(it);
	}

	// 0x467670
	void Container::dump(int indent)
	{
		static char s_line[256];

		std::string name = m_name;
		if (name.empty())
			name = "[unknown]";
		Application* app = getApplication();
		for (int i = 0; i < indent; i++)
			app->log("\t");
		const char* visible = isVisible() ? "true" : "false";
		const char* remove = (m_flags & 0x10) ? "DELETE" : "";
		sprintf(s_line, "<%s name=\"%s\" %s visible=\"%s\" flags=\"%x\" refCount=\"%d\" bounds=\"(%.2f,%.2f),(%.2f,%.2f)\" />\n",
			getTypeName().c_str(), name.c_str(), remove, visible, m_treeFlags, getRefCount(),
			m_bounds.left, m_bounds.top, m_bounds.right, m_bounds.bottom);
		app->log(s_line);
		for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
			(*it)->dump(indent + 1);
		for (int i = 0; i < indent; i++)
			app->log("\t");
		app->log("</%s >\n", getTypeName().c_str());
	}

	// 0x467930
	Component* Container::getChild(int index)
	{
		if (index >= 0 && index < (int)m_children.size())
			return m_children.at(index);
		return 0;
	}

	// 0x4679D0
	void Container::addChild(Component* child)
	{
		if (std::find(m_children.begin(), m_children.end(), child) == m_children.end())
		{
			m_children.push_back(child);
			addTreeFlags(child->getTreeFlags());
			addTreeFlags(8);
			child->onAddedTo(this);
			onChildAdded(child);
		}
	}

	// 0x492310 (folded)
	void Container::onChildAdded(Component* child)
	{
	}

	// 0x492310 (folded)
	void Container::onChildRemoved(Component* child)
	{
	}

	// 0x47B6D0 (folded)
	int Container::getChildCount() const
	{
		return m_children.size();
	}
}
