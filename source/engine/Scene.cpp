#include "Scene.h"

#include "Application.h"
#include "Point.h"

namespace engine
{
	// 0x4710F0
	void Scene::update(UpdateContext& context)
	{
		Container::update(context);
		Container::updateBounds();
	}

	// 0x471230
	void Scene::setCapture(Component* component)
	{
		m_captureComponent = component;
	}

	// 0x471270
	void Scene::releaseCapture()
	{
		m_captureComponent = 0;
	}

	// 0x4712A0: picks the component under the mouse (only the capturing one while there is one), sends leave and
	// enter events when it changes and returns the event's target, its position made local to a capturing component
	Component* Scene::updateMouseTarget(const Point& mousePos)
	{
		if (m_hoverComponent && m_hoverComponent->getRefCount() == 1)
			m_hoverComponent = 0;
		m_mouseEvent.position.set((float)mousePos.x, (float)mousePos.y);
		Component* target = getComponentAt(2, m_mouseEvent.position);
		if (m_captureComponent && target != m_captureComponent)
			target = 0;
		if (target != m_hoverComponent)
		{
			if (m_hoverComponent)
			{
				m_mouseEvent.target = m_hoverComponent;
				m_hoverComponent->mouseLeave(m_mouseEvent);
			}
			m_hoverComponent = target;
			if (m_hoverComponent)
			{
				m_mouseEvent.target = m_hoverComponent;
				m_hoverComponent->mouseEnter(m_mouseEvent);
			}
		}
		if (m_captureComponent)
		{
			m_mouseEvent.target = m_captureComponent;
			m_mouseEvent.position.set((float)mousePos.x, (float)mousePos.y);
			m_mouseEvent.position -= m_captureComponent->getScreenPosition();
		}
		else
			m_mouseEvent.target = target;
		return m_mouseEvent.target;
	}

	// 0x4714C0
	void Scene::onMouseDown(const Point& pos)
	{
		m_mouseEvent.buttons |= 1;
		Component* target = updateMouseTarget(pos);
		if (target)
			target->mouseDown(m_mouseEvent);
	}

	// 0x4714F0
	void Scene::onMouseUp(const Point& pos)
	{
		m_mouseEvent.buttons &= ~1;
		Component* target = updateMouseTarget(pos);
		if (target)
			target->mouseUp(m_mouseEvent);
	}

	// 0x471520
	void Scene::onRightMouseDown(const Point& pos)
	{
		m_mouseEvent.buttons |= 2;
		Component* target = updateMouseTarget(pos);
		if (target)
			target->rightMouseDown(m_mouseEvent);
	}

	// 0x471550
	void Scene::onRightMouseUp(const Point& pos)
	{
		m_mouseEvent.buttons &= ~2;
		Component* target = updateMouseTarget(pos);
		if (target)
			target->rightMouseUp(m_mouseEvent);
	}

	// 0x471580
	void Scene::onMouseMove(const Point& pos)
	{
		Component* target = updateMouseTarget(pos);
		if (target)
			target->mouseMove(m_mouseEvent);
	}

	// 0x4715B0
	void Scene::refreshMouseOver()
	{
		Component* target = updateMouseTarget(getApplication()->getMousePosition());
		if (target)
			target->mouseMove(m_mouseEvent);
	}

	// 0x471690
	void Scene::deactivate()
	{
		Container::deactivate();
		Application* app = getApplication();
		app->m_updateSignal.disconnect(this);
		app->m_drawSignal.disconnect(this);
		app->m_mouseDownSignal.disconnect(this);
		app->m_mouseUpSignal.disconnect(this);
		app->m_rightMouseDownSignal.disconnect(this);
		app->m_rightMouseUpSignal.disconnect(this);
		app->m_mouseMoveSignal.disconnect(this);
	}

	// 0x471760
	Scene::~Scene()
	{
	}

	// 0x471830
	std::string Scene::getTypeName() const
	{
		return "Scene";
	}

	// 0x471A40: the event's target is left uninitialised
	Scene::Scene()
	{
		m_mouseEvent.buttons = 0;
		m_mouseEvent.scene = this;
	}

	// 0x471B50: the children are not activated (Container::activate is not called)
	void Scene::activate()
	{
		Application* app = getApplication();
		app->m_updateSignal.connect(this, &Scene::update);
		app->m_drawSignal.connect(this, &Scene::draw);
		app->m_mouseDownSignal.connect(this, &Scene::onMouseDown);
		app->m_mouseUpSignal.connect(this, &Scene::onMouseUp);
		app->m_rightMouseDownSignal.connect(this, &Scene::onRightMouseDown);
		app->m_rightMouseUpSignal.connect(this, &Scene::onRightMouseUp);
		app->m_mouseMoveSignal.connect(this, &Scene::onMouseMove);
	}

	// 0x471110 (folded)
	void Scene::draw(Graphics& g)
	{
		Container::draw(g);
	}
}
