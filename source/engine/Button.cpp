#include "Button.h"

#include "Application.h"
#include "ButtonListener.h"
#include "Scene.h"
#include "SoundHandle.h"

namespace engine
{
	// 0x47A080
	void Button::setNormalScale(Vector2 scale)
	{
		m_normalScale = scale;
		m_scaleEffect = m_normalScale.x != m_hoverScale.x || m_normalScale.y != m_hoverScale.y;
	}

	// 0x47A0E0
	void Button::setHoverScale(Vector2 scale)
	{
		m_hoverScale = scale;
		m_scaleEffect = m_normalScale.x != m_hoverScale.x || m_normalScale.y != m_hoverScale.y;
	}

	// 0x47A140
	void Button::setScaleDuration(float seconds)
	{
		m_scaleDuration = seconds;
	}

	// 0x47A150
	void Button::update(UpdateContext& context)
	{
		if (m_scaleEffect)
		{
			if (m_scalePhase == 1 || m_scalePhase == 2)
			{
				m_scaleTimer -= context.elapsed;
				if (m_scaleTimer < 0.0f)
					m_scaleTimer = 0.0f;
			}
			switch (m_scalePhase)
			{
			case 0:
				setScale(m_baseScale.x * m_hoverScale.x, m_baseScale.y * m_hoverScale.y);
				break;
			case 1:
			{
				float t = 1.0f - m_scaleTimer / m_scaleDuration;
				setScale((m_normalScale.x + (m_hoverScale.x - m_normalScale.x) * t) * m_baseScale.x,
					(m_normalScale.y + (m_hoverScale.y - m_normalScale.y) * t) * m_baseScale.y);
				if (t >= 1.0)
					m_scalePhase = 0;
				break;
			}
			case 2:
			{
				float t = m_scaleTimer / m_scaleDuration;
				setScale((m_normalScale.x + (m_hoverScale.x - m_normalScale.x) * t) * m_baseScale.x,
					(m_normalScale.y + (m_hoverScale.y - m_normalScale.y) * t) * m_baseScale.y);
				if (t <= 0.0)
					m_scalePhase = 3;
				break;
			}
			case 3:
				setScale(m_baseScale.x * m_normalScale.x, m_baseScale.y * m_normalScale.y);
				break;
			}
		}
		if (m_pulsing)
		{
			m_pulseTimer -= context.elapsed;
			if (m_pulseTimer < 0.0f)
				m_pulseTimer = 0.0f;
			float level = m_pulseTimer / m_pulsePeriod;
			if (m_pulseFalling)
			{
				setPulseLevel(level);
				if (m_pulseTimer <= 0.0f)
				{
					m_pulseTimer = m_pulsePeriod;
					m_pulseFalling = false;
				}
			}
			else
			{
				setPulseLevel(1.0f - level);
				if (m_pulseTimer <= 0.0f)
				{
					m_pulseFalling = true;
					m_pulseTimer = m_pulsePeriod;
				}
			}
		}
		Container::update(context);
	}

	// 0x47A3E0
	void Button::setPulseLevel(float level)
	{
		setAlpha(level);
	}

	// 0x47A3F0
	void Button::endPulse()
	{
		setAlpha(1.0f);
	}

	// 0x47A400
	void Button::setBaseScale(Vector2 scale)
	{
		m_baseScale = scale;
		Component::setScale(scale.x, scale.y);
	}

	// 0x47A420
	void Button::setHotspotMode(int mode)
	{
		m_hotspotMode = mode;
	}

	// 0x47A430
	void Button::setPulsePeriod(float seconds)
	{
		if (m_pulsePeriod != seconds)
		{
			m_pulseTimer = seconds;
			m_pulsePeriod = seconds;
			m_pulseFalling = true;
			m_pulsing = seconds > 0.0f;
		}
	}

	// 0x47A490
	void Button::deactivate()
	{
		if (m_state != 0)
		{
			m_state = 0;
			onStateChanged();
		}
		maskFlags(2);
	}

	// 0x47A4C0
	void Button::activate()
	{
		unmaskFlags(2);
	}

	// 0x47A4D0
	void Button::mouseDown(const MouseEvent& event)
	{
		if (m_state == 1)
		{
			m_state = 3;
			onStateChanged();
			fireMouseDown();
			event.scene->setCapture(this);
			if (m_clickSound)
				m_clickSound->play();
		}
	}

	// 0x47A520
	void Button::mouseUp(const MouseEvent& event)
	{
		if (m_state == 3)
		{
			m_state = 1;
			onStateChanged();
			fireMouseUp();
			fireClick();
		}
		else if (m_state == 2)
		{
			m_state = 0;
			onStateChanged();
			if (m_pulsePeriod > 0.0f)
			{
				m_pulseTimer = m_pulsePeriod;
				m_pulseFalling = true;
				m_pulsing = true;
			}
			fireMouseLeave();
		}
		event.scene->releaseCapture();
	}

	// 0x47A5D0
	void Button::mouseEnter(const MouseEvent& event)
	{
		if (m_state == 2)
		{
			m_state = 3;
			onStateChanged();
			fireMouseDown();
		}
		else if (m_state == 0)
		{
			m_state = 1;
			onStateChanged();
			fireMouseEnter();
			if (m_hoverSound)
				m_hoverSound->play();
		}
		m_scalePhase = 1;
		if (m_scaleTimer <= 0.0f)
			m_scaleTimer = m_scaleDuration;
		else
			m_scaleTimer = m_scaleDuration - m_scaleTimer;
	}

	// 0x47A670
	void Button::mouseLeave(const MouseEvent& event)
	{
		if (m_state == 3)
		{
			m_state = 2;
			onStateChanged();
			fireMouseUp();
		}
		else if (m_state == 1)
		{
			m_state = 0;
			onStateChanged();
			if (m_pulsePeriod > 0.0f)
			{
				m_pulseTimer = m_pulsePeriod;
				m_pulseFalling = true;
				m_pulsing = true;
			}
			fireMouseLeave();
		}
		m_scalePhase = 2;
		if (m_scaleTimer <= 0.0f)
			m_scaleTimer = m_scaleDuration;
		else
			m_scaleTimer = m_scaleDuration - m_scaleTimer;
	}

	// 0x47A730
	void Button::setSounds(const std::string& hoverSound, const std::string& clickSound)
	{
		Application* application = getApplication();
		if (!hoverSound.empty())
		{
			application->loadSound(hoverSound, hoverSound);
			m_hoverSound = application->createSound(hoverSound, true, 0);
		}
		if (!clickSound.empty())
		{
			application->loadSound(clickSound, clickSound);
			m_clickSound = application->createSound(clickSound, true, 0);
		}
	}

	// 0x47A790
	Button::~Button()
	{
		std::vector<ButtonListenerEntry*>::iterator it = m_listeners.begin();
		while (it != m_listeners.end())
		{
			ButtonListenerEntry* entry = *it;
			it = m_listeners.erase(it);
			delete entry;
		}
		if (m_hoverSound)
			m_hoverSound->destroy();
		if (m_clickSound)
			m_clickSound->destroy();
	}

	// 0x47A8F0
	void Button::fireMouseEnter()
	{
		for (std::vector<ButtonListenerEntry*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
			(*it)->listener->onMouseEnter((*it)->command);
	}

	// 0x47A950
	void Button::fireMouseLeave()
	{
		for (std::vector<ButtonListenerEntry*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
			(*it)->listener->onMouseLeave((*it)->command);
	}

	// 0x47A9B0
	void Button::fireMouseDown()
	{
		for (std::vector<ButtonListenerEntry*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
			(*it)->listener->onMouseDown((*it)->command);
	}

	// 0x47AA10
	void Button::fireMouseUp()
	{
		for (std::vector<ButtonListenerEntry*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
			(*it)->listener->onMouseUp((*it)->command);
	}

	// 0x47AA70
	void Button::fireClick()
	{
		for (std::vector<ButtonListenerEntry*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
			(*it)->listener->onClick((*it)->command);
	}

	// 0x47AD40
	Button::Button()
	{
		m_state = 0;
		m_hoverSound = 0;
		m_clickSound = 0;
		m_scaleEffect = false;
		m_normalScale = Vector2(1.0f, 1.0f);
		m_hoverScale = Vector2(1.0f, 1.0f);
		m_baseScale = Vector2(1.0f, 1.0f);
		m_scaleTimer = 0.0f;
		m_hotspotMode = 0;
		m_pulsePeriod = 0.0f;
		m_pulseTimer = 0.0f;
		m_pulsing = false;
		m_scaleDuration = 0.5f;
		m_scalePhase = 3;
		m_pulseFalling = true;
	}

	// 0x47AEB0
	void Button::addListener(ButtonListener* listener, std::string command)
	{
		ButtonListenerEntry* entry = new ButtonListenerEntry;
		entry->command = command;
		entry->listener = listener;
		m_listeners.push_back(entry);
	}

	// 0x492310 (folded)
	void Button::mouseMove(const MouseEvent& event)
	{
	}
}
