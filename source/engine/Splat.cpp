#include "Splat.h"

#include "Application.h"
#include "TextItem.h"

namespace engine
{
	// 0x4811D0
	void Splat::setPosition(float x, float y)
	{
		m_position.set(x, y);
	}

	// 0x4811E0
	void Splat::setStartPosition(float x, float y)
	{
		m_startPosition.x = x;
		m_startPosition.y = y;
		setPosition(x, y);
	}

	// 0x481200
	void Splat::moveTo(float x, float y)
	{
		m_targetPosition.x = x;
		m_targetPosition.y = y;
		m_moveToTarget = true;
		float speed = m_velocity.length();
		if (speed > 0.0f)
			m_timeLeft = m_lifeTime = (m_targetPosition - m_startPosition).length() / speed;
	}

	// 0x481280
	void Splat::setCallback(void (*callback)(void*), void* data)
	{
		m_callback = callback;
		m_callbackData = data;
	}

	// 0x4812A0
	void Splat::setShadow(bool enabled, float alpha, Vector2 offset)
	{
		m_shadow = enabled;
		m_shadowAlpha = alpha;
		m_shadowOffset = offset;
	}

	// 0x4812D0
	void Splat::setContent(Component* content)
	{
		removeAllChildren();
		addChild(content);
	}

	// 0x4812F0
	void Splat::update(UpdateContext& context)
	{
		float dt = context.elapsed;
		m_timeLeft -= dt;
		if (m_timeLeft >= 0.0f)
		{
			float life = (m_lifeTime - m_timeLeft) / m_lifeTime;
			float alpha = (m_endAlpha - m_startAlpha) * life + m_startAlpha;
			if (alpha <= 0.0f)
				alpha = 0.0f;
			else if (alpha >= 1.0f)
				alpha = 1.0f;
			float t = life < m_scaleStart ? 0.0f : (life > m_scaleEnd ? 1.0f : (life - m_scaleStart) / (m_scaleEnd - m_scaleStart));
			float scale = (m_endScale - m_startScale) * t + m_startScale;
			if (scale <= 0.0f)
				scale = 0.0f;
			setScale(scale, scale);
			setAlpha(alpha);
			if (m_moveToTarget)
			{
				Vector2 position(m_targetPosition);
				position -= m_startPosition;
				position *= life;
				position += m_startPosition;
				setPosition(position.x, position.y);
			}
			else
			{
				Vector2 velocity(m_velocity);
				Vector2 position(getX(), getY());
				Vector2 direction(velocity);
				if (direction.length() != 0.0f)
					direction.normalize();
				direction *= dt * m_drag;
				if (m_drag < 0.0f)
				{
					// drag slows down to a stop: an axis whose velocity would change sign stops instead
					Vector2 previous(velocity);
					velocity += direction;
					if (previous.x * velocity.x < 0.0f)
						velocity.x = 0.0f;
					if (previous.y * velocity.y < 0.0f)
						velocity.y = 0.0f;
				}
				else
				{
					velocity += direction;
				}
				direction = m_acceleration;
				direction *= dt;
				velocity += direction;
				m_velocity = velocity;
				velocity *= dt;
				position += velocity;
				setPosition(position.x, position.y);
			}
			Container::update(context);
		}
	}

	// 0x4815F0
	void Splat::setText(const std::string& text)
	{
		if (!text.empty())
		{
			TextItem* item = new TextItem();
			item->setFont(m_fontName);
			item->setXAlign(1);
			item->setYAlign(1);
			item->setText(text);
			if (m_shadow)
			{
				item->m_style |= 4;
				item->setShadow(m_shadowAlpha, m_shadowOffset);
			}
			if (m_textColor.r != 1.0f || m_textColor.g != 1.0f || m_textColor.b != 1.0f)
			{
				item->setColorMode(2);
				item->setColor(m_textColor);
			}
			removeAllChildren();
			addChild(item);
			updateBounds();
			// keep the text on the screen horizontally
			float halfWidth = getBounds().getWidth() * 0.5f;
			float x = getPosition().x;
			if (x - halfWidth < 0.0f)
				x = halfWidth;
			else if (x + halfWidth > getApplication()->getWidth())
				x = getApplication()->getWidth() - halfWidth;
			setPosition(x, getPosition().y);
		}
	}

	// 0x4817C0
	void Splat::fireFinished()
	{
		if (m_callback != 0)
			m_callback(m_callbackData);
		m_onFinished.emit(m_eventArg);
	}

	// 0x481810
	Splat::Splat()
		: m_drag(0.0f), m_startScale(1.0f), m_endScale(1.0f), m_startAlpha(1.0f), m_endAlpha(1.0f), m_lifeTime(1.0f),
		  m_timeLeft(1.0f), m_moveToTarget(false), m_callback(0), m_callbackData(0), m_eventArg(0), m_shadow(false),
		  m_shadowAlpha(0.0f)
	{
		setFlags(4);
	}

	// 0x4819B0
	std::string Splat::getTypeName() const
	{
		return "Splat";
	}
}
