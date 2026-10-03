#include "FadeContainer.h"

namespace engine
{
	// 0x466D70
	void FadeContainer::fade(bool fadeIn, float duration, float amount, bool removeWhenDone)
	{
		if (fadeIn == m_fadingIn && m_fadeTimeLeft > 0.0f)
			return;
		float alpha = getAlpha();
		if (fadeIn)
		{
			if (alpha == amount && isVisible() && m_fadeTimeLeft == 0.0f)
				return;
			setVisible(true);
			setAlpha(0.0f);
			m_removeWhenFaded = false;
		}
		else
		{
			if (alpha == 1.0f - amount && !isVisible() && m_fadeTimeLeft == 0.0f)
				return;
			m_removeWhenFaded = removeWhenDone;
		}
		m_fadeAmount = amount;
		setFlags(4);
		m_fadeTimeLeft = duration;
		m_fadingIn = fadeIn;
		m_fadeDuration = duration;
		if (duration == 0.0f)
		{
			if (fadeIn)
				setAlpha(m_fadeAmount);
			else
			{
				setVisible(false);
				setAlpha(1.0f - m_fadeAmount);
			}
		}
	}

	// 0x466EB0
	void FadeContainer::setFadeCallback(void (*callback)(void*), void* userData)
	{
		m_fadeCallback = callback;
		m_fadeCallbackData = userData;
	}

	// 0x466ED0
	bool FadeContainer::isFadeFinished() const
	{
		return m_fadeTimeLeft == 0.0f;
	}

	// 0x466EF0
	void FadeContainer::update(UpdateContext& context)
	{
		Container::update(context);
		if (m_fadeTimeLeft > 0.0f && m_fadeDuration > 0.0f)
		{
			m_fadeTimeLeft -= context.elapsed;
			if (m_fadeTimeLeft <= 0.0f)
			{
				m_fadeTimeLeft = 0.0f;
				if (m_fadeCallback)
					m_fadeCallback(m_fadeCallbackData);
				clearFlags(4);
				if (!m_fadingIn && m_fadeAmount == 1.0f)
				{
					setVisible(false);
					if (m_removeWhenFaded)
						setFlags(0x10);
				}
			}
			float alpha = (1.0f - m_fadeTimeLeft / m_fadeDuration) * m_fadeAmount;
			if (m_fadingIn)
				setAlpha(alpha);
			else
				setAlpha(1.0f - alpha);
		}
	}

	// 0x466FF0
	FadeContainer::FadeContainer()
	{
		clearFadeCallback();
		setFlags(4);
		m_fadeDuration = 1.0f;
		m_fadeAmount = 1.0f;
		m_fadingIn = false;
		m_fadeTimeLeft = 0.0f;
		m_removeWhenFaded = false;
	}
}
