#include "Checkbox.h"

#include "Image.h"
#include "Selector.h"
#include "Surface.h"

namespace engine
{
	// 0x4954A0: frames 0 unchecked, 1 uncheckedOver, 2 checked, 3 checkedOver; missing hover images reuse the plain ones
	bool Checkbox::create(Bitmap* unchecked, Bitmap* uncheckedOver, Bitmap* checked, Bitmap* checkedOver)
	{
		if (!unchecked || !checked)
			return false;

		m_selector = new Selector();
		addChild(m_selector);

		unchecked->setPivotType(m_hotspotMode);
		m_selector->addChild(new Image(unchecked));
		if (uncheckedOver)
		{
			uncheckedOver->setPivotType(m_hotspotMode);
			m_selector->addChild(new Image(uncheckedOver));
		}
		else
		{
			m_selector->addChild(new Image(unchecked));
		}

		checked->setPivotType(m_hotspotMode);
		m_selector->addChild(new Image(checked));
		if (checkedOver)
		{
			checkedOver->setPivotType(m_hotspotMode);
			m_selector->addChild(new Image(checkedOver));
		}
		else
		{
			m_selector->addChild(new Image(checked));
		}

		setFlags(2);
		m_selector->updateBounds();
		updateBounds();
		m_selector->select(0, 0.0f, true);
		setChecked(false);
		return true;
	}

	// 0x495730
	void Checkbox::onStateChanged()
	{
		updateCheckImage();
	}

	// 0x495740
	void Checkbox::fireClick()
	{
		m_checked = !m_checked;
		updateCheckImage();
		Button::fireClick();
	}

	// 0x495770: button states 0 and 2 show the plain frame, 1 (over) and 3 (pressed) the hover frame
	void Checkbox::updateCheckImage()
	{
		switch (m_state)
		{
		case 0:
		case 2:
			m_selector->select(m_checked ? 2 : 0, 0.0f, true);
			break;
		case 1:
		case 3:
			m_selector->select(m_checked ? 3 : 1, 0.0f, true);
			break;
		}
	}

	// 0x4957E0
	bool Checkbox::isChecked() const
	{
		return m_checked;
	}

	// 0x4957F0
	void Checkbox::setChecked(bool checked)
	{
		m_checked = checked;
		updateCheckImage();
	}
}
