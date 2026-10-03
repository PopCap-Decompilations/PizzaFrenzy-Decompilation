#include "Screen.h"

#include <string>

#include "Component.h"
#include "Transition.h"

namespace engine
{
	// 0x464010
	void Screen::setTransitionDelay(float delay)
	{
		m_transitionDelay = delay;
	}

	// 0x464020
	void Screen::show()
	{
		prepareShow();
		m_delayLeft = m_transitionDelay;
		m_transitionState = 2;
		setFlags(5);
	}

	// 0x464050
	bool Screen::isIdle() const
	{
		return m_transitionState != 2 && m_transitionState != 3 && m_transitionState != 6 && m_transitionState != 5;
	}

	// 0x464080
	void Screen::setTransitions(Transition* showTransition, Transition* hideTransition)
	{
		if (m_showTransition)
		{
			m_showTransition->setTarget(0);
			removeAnimator(m_showTransition);
		}
		if (m_hideTransition)
		{
			m_hideTransition->setTarget(0);
			removeAnimator(m_hideTransition);
		}
		m_showTransition = showTransition;
		m_showTransition->setTarget(this);
		m_hideTransition = hideTransition;
		m_hideTransition->setTarget(this);
		prepareShow();
	}

	// 0x464150
	void Screen::prepareShow()
	{
		removeAnimator(m_hideTransition);
		addAnimator(m_showTransition);
		m_showTransition->setProgress(0.0f);
		deactivate();
	}

	// 0x464190
	void Screen::hide()
	{
		if (m_transitionState != 1)
		{
			removeAnimator(m_showTransition);
			addAnimator(m_hideTransition);
			m_hideTransition->setProgress(1.0f);
			m_delayLeft = m_transitionDelay;
			m_transitionState = 5;
			setFlags(5);
			deactivate();
		}
	}

	// 0x464200
	void Screen::update(UpdateContext& context)
	{
		switch (m_transitionState)
		{
		case 2:
			m_delayLeft -= context.elapsed;
			if (m_delayLeft < 0.0f)
			{
				m_transitionState = 3;
				m_showTransition->start(0);
			}
			break;
		case 3:
			if (m_showTransition->isFinished())
			{
				m_transitionState = 4;
				removeAnimator(m_showTransition);
				activate();
			}
			break;
		case 5:
			m_delayLeft -= context.elapsed;
			if (m_delayLeft < 0.0f)
			{
				m_transitionState = 6;
				m_hideTransition->start(1);
			}
			break;
		case 6:
			if (m_hideTransition->isFinished())
			{
				removeAnimator(m_hideTransition);
				m_transitionState = 1;
			}
			break;
		}
		ScreenLayout::update(context);
	}

	// 0x464330
	void Screen::onAction(std::string action)
	{
		m_actionSignal.emit(action);
	}

	// 0x464410
	Screen::~Screen()
	{
	}

	// 0x464540
	Screen::Screen()
	{
		setFlags(5);
		m_transitionState = 0;
		m_transitionDelay = 0.0f;
		m_delayLeft = 0.0f;
		setActionListener(this);
		m_hideTransition = 0;
		m_showTransition = 0;
	}
}
