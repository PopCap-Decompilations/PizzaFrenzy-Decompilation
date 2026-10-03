// OrderButton: a button showing a topping's icon and a count.
#include "OrderButton.h"

#include "engine/Application.h"
#include "engine/Image.h"
#include "engine/ImageButton.h"
#include "engine/ScreenLayout.h"
#include "engine/TextItem.h"
#include "TileManager.h"

// 0x427100
void OrderButton::enable()
{
	setEnabled(true);
	setColor(0.0f, 0.0f, 0.0f);
}

// 0x427120
void OrderButton::disable()
{
	setEnabled(false);
	setColor(-0.2f, -0.2f, -0.2f);
	setColorMode(1);
}

// 0x427150
void OrderButton::setScale(float scale)
{
	m_button->setBaseScale(engine::Vector2(scale, scale));
	if (m_count >= 0)
		m_icon->setPosition(0.0f, scale * -18.0f);
	else
		m_icon->setPosition(0.0f, scale * -5.0f);
	m_countText->setPosition(0.0f, scale * 30.0f);
}

// 0x4271D0
void OrderButton::setCountText(const std::string& text)
{
	m_countText->setText(text);
}

// 0x4271E0
void OrderButton::setEnabled(bool enabled)
{
	engine::Component::setEnabled(enabled);
	m_button->setEnabled(enabled);
}

// 0x427210
OrderButton::OrderButton(Topping* topping, int count, engine::ScreenLayout* layout)
{
	m_topping = topping;
	m_count = count;
	m_button = new engine::ImageButton();
	m_button->setHotspotMode(1);
	m_button->setImages(engine::getApplication()->getImage("res\\pizza\\orderButton.jpg"), NULL, NULL);
	m_button->setSounds("", "res\\sounds\\menu_clicked.ogg");
	m_button->setName(topping->m_name);
	addChild(m_button);
	layout->addButton(m_button, topping->m_name, topping->m_name);
	layout->addComponent(m_button, topping->m_name);

	m_countText = new engine::TextItem();
	m_countText->setFont(engine::getApplication()->getFont("res\\fonts\\buttonFont.xml"));
	m_countText->setXAlign(1);
	m_countText->setYAlign(2);
	m_countText->setColorMode(2);
	m_countText->setColor(0.0f, 0.0f, 0.0f);
	m_countText->setNumber(count, "");
	m_countText->setPosition(0.0f, 30.0f);
	addChild(m_countText);

	m_icon = new engine::Image(topping->m_smallImage);
	m_icon->setPosition(0.0f, -17.0f);
	addChild(m_icon);
	if (count < 0)
	{
		m_countText->setVisible(false);
		m_icon->setPosition(0.0f, -5.0f);
	}
}
