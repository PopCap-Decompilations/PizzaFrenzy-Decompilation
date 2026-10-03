#include "Blink.h"

#include "Component.h"

namespace engine
{
	// 0x47AF90
	void Blink::update(UpdateContext& context, Component* component)
	{
		m_timer -= context.elapsed;
		if (m_timer < 0.0f)
		{
			if (component->isVisible())
			{
				component->setVisible(false);
				m_timer = m_offTime;
			}
			else
			{
				component->setVisible(true);
				m_timer = m_onTime;
			}
		}
	}

	// 0x47AFF0
	Blink::Blink(float onTime, float offTime)
	{
		m_offTime = offTime;
		m_onTime = onTime;
		m_timer = 0.0f;
	}
}
