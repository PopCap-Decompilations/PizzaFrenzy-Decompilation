#include "Image.h"

#include <cmath>

#include "Graphics.h"

namespace engine
{
	// 0x470CE0
	void Image::setUseSourceRect(bool use)
	{
		m_useSourceRect = use;
	}

	// 0x470CF0
	void Image::setSourceRect(const IntRect& r)
	{
		m_sourceRect = r;
	}

	// 0x470D20
	Image::Image()
	{
		setFlags(1);
		m_useSourceRect = false;
	}

	// 0x470DF0
	void Image::setImage(Bitmap* image)
	{
		m_image = image;
		addTreeFlags(8);
	}

	// 0x470E40: the image's rectangle placed with its pivot at the position; empty without an image
	void Image::updateBounds()
	{
		if (m_image)
		{
			Bitmap* image = m_image;
			m_bounds.set(m_position.x - image->getPivot().x, m_position.y - image->getPivot().y,
				m_position.x + image->getWidth() - image->getPivot().x, m_position.y + image->getHeight() - image->getPivot().y);
		}
		else
			m_bounds.clear();
		m_bounds.scale(fabs(m_scale.x), fabs(m_scale.y));
		removeTreeFlags(8);
	}

	// 0x470F20
	void Image::draw(Graphics& g)
	{
		if (isVisible() && m_image)
		{
			if (m_useSourceRect)
				g.drawImage(m_image, m_sourceRect, 0.0f, 0.0f);
			else
				g.drawImage(m_image);
		}
		removeTreeFlags(1);
	}

	// 0x470F80
	int Image::getImageWidth() const
	{
		if (m_image)
			return m_image->getWidth();
		return 0;
	}

	// 0x470FA0
	int Image::getImageHeight() const
	{
		if (m_image)
			return m_image->getHeight();
		return 0;
	}

	// 0x470FC0
	Bitmap* Image::getImage() const
	{
		return m_image;
	}

	// 0x470FD0
	Image::Image(Bitmap* image)
	{
		setImage(image);
		setFlags(1);
		m_useSourceRect = false;
	}

	// 0x4710C0
	std::string Image::getTypeName() const
	{
		return "Image";
	}
}
