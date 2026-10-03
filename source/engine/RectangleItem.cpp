#include "RectangleItem.h"

#include "Graphics.h"

namespace engine
{
	// 0x483880
	void RectangleItem::setRect(Rect rect)
	{
		m_bounds = rect;
	}

	// 0x4838B0
	void RectangleItem::draw(Graphics& g)
	{
		g.setColor(m_color);
		g.fillRect(IntRect((int)m_bounds.left, (int)m_bounds.top, (int)m_bounds.right, (int)m_bounds.bottom));
	}

	// 0x483950
	RectangleItem::RectangleItem(Rect rect)
	{
		setRect(rect);
		setFlags(1);
	}

	// 0x483A60
	std::string RectangleItem::getTypeName() const
	{
		return "RectangleItem";
	}

	// 0x4D0470 (folded)
	void RectangleItem::updateBounds()
	{
	}
}
