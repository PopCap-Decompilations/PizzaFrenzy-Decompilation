#include "ImageButton.h"

#include "Image.h"
#include "Selector.h"

namespace engine
{
	// 0x4646E0 (folded)
	void ImageButton::endPulse()
	{
		onStateChanged();
	}

	// 0x478900
	bool ImageButton::setImages(Bitmap* normal, Bitmap* hover, Bitmap* pressed)
	{
		if (normal == 0)
			return false;
		if (m_selector != 0)
		{
			m_selector->removeAllChildren();
		}
		else
		{
			m_selector = new Selector();
			addChild(m_selector);
		}
		normal->setPivotType(m_hotspotMode);
		Image* image = new Image(normal);
		m_selector->addChild(image);
		m_bounds = image->getBounds();
		if (hover != 0)
		{
			hover->setPivotType(m_hotspotMode);
			image = new Image(hover);
		}
		else
		{
			image = new Image(normal);
			image->setColorMode(1);
			image->setColor(0.3f, 0.3f, 0.3f);
		}
		m_selector->addChild(image);
		if (pressed != 0)
		{
			pressed->setPivotType(m_hotspotMode);
			image = new Image(pressed);
		}
		else
		{
			image = new Image(hover != 0 ? hover : normal);
			if (hover == 0)
			{
				image->setColorMode(1);
				image->setColor(0.4f, 0.4f, 0.4f);
			}
		}
		m_selector->addChild(image);
		setFlags(2);
		m_selector->select(0, 0.0f, true);
		return true;
	}

	// 0x478BC0
	void ImageButton::onStateChanged()
	{
		switch (m_state)
		{
		case 0:
		case 2:
			m_selector->select(0, 0.0f, true);
			break;
		case 1:
			m_selector->select(1, 0.0f, true);
			break;
		case 3:
			m_selector->select(2, 0.0f, true);
			break;
		}
	}

	// 0x478C30
	void ImageButton::setPulseLevel(float level)
	{
		if (level <= 0.0f)
			m_selector->select(1, 0.2f, true);
		else if (level >= 1.0f)
			m_selector->select(0, 0.2f, false);
	}

	// 0x478C90
	ImageButton::ImageButton()
		: m_selector(0)
	{
	}
}
