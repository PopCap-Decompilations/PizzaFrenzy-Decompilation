#include "ScreenMgr.h"

#include "Screen.h"

namespace engine
{
	// 0x46AF40: the screens stay active
	void ScreenMgr::deactivate()
	{
		maskFlags(2);
	}

	// 0x46AFD0: once both screens' transitions are done, the incoming screen becomes the current one
	void ScreenMgr::updateTransition(UpdateContext& ctx)
	{
		if ((!m_nextScreen || m_nextScreen->isIdle()) && (!m_currentScreen || m_currentScreen->isIdle()))
		{
			if (m_currentScreen)
				removeChild(m_currentScreen);
			m_currentScreen = m_nextScreen;
			m_nextScreen = 0;
			m_isTransitioning = false;
		}
	}

	// 0x46B090
	bool ScreenMgr::isTransitioning() const
	{
		return m_isTransitioning || !m_pendingScreens.empty();
	}

	// 0x46B1B0
	void ScreenMgr::clear()
	{
		while (!m_pendingScreens.empty())
			m_pendingScreens.pop_front();
		m_isTransitioning = false;
		m_currentScreen = 0;
		m_nextScreen = 0;
		removeAllChildren();
	}

	// 0x46B280: starts the switch to the next queued screen (hide the current one, add and show the new one)
	void ScreenMgr::update(UpdateContext& context)
	{
		if (m_isTransitioning)
			updateTransition(context);
		else if (!m_pendingScreens.empty())
		{
			m_nextScreen = m_pendingScreens.front();
			m_pendingScreens.pop_front();
			if (m_currentScreen != m_nextScreen)
			{
				if (m_currentScreen)
				{
					m_currentScreen->setTransitionDelay(0.0f);
					m_currentScreen->hide();
				}
				if (m_nextScreen)
				{
					addChild(m_nextScreen);
					m_nextScreen->show();
				}
				m_isTransitioning = true;
			}
			else
			{
				m_nextScreen = 0;
				m_isTransitioning = false;
			}
		}
		Container::update(context);
	}

	// 0x46B390
	ScreenMgr::~ScreenMgr()
	{
	}

	// 0x46B470
	std::string ScreenMgr::getTypeName() const
	{
		return "ScreenMgr";
	}

	// 0x46B710
	ScreenMgr::ScreenMgr()
	{
		setFlags(5);
		m_currentScreen = 0;
		m_isTransitioning = false;
	}

	// 0x46B7F0: queued; the screen's show transition starts after delay seconds
	void ScreenMgr::showScreen(Screen* screen, float delay)
	{
		if (screen)
			screen->setTransitionDelay(delay);
		m_pendingScreens.push_back(screen);
	}

	// 0x47A4C0 (folded)
	void ScreenMgr::activate()
	{
		unmaskFlags(2);
	}
}
