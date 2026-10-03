// engine::ScreenLayoutParser: XML handler that builds a ScreenLayout from res/screenLayouts/*.xml.
#include "ScreenLayoutParser.h"

#include <ctype.h>

#include "Application.h"
#include "Checkbox.h"
#include "EditBox.h"
#include "Exception.h"
#include "FadeContainer.h"
#include "Image.h"
#include "ImageButton.h"
#include "ListBox.h"
#include "ParticleSystem.h"
#include "Properties.h"
#include "RadioGroup.h"
#include "ScreenLayout.h"
#include "Selector.h"
#include "Table.h"
#include "TextButton.h"
#include "TextItem.h"

namespace engine
{
	// 0x48BFA0
	ScreenLayoutParser::ScreenLayoutParser()
	{
		m_radioGroup = 0;
	}

	// 0x484A10
	ScreenLayoutParser::~ScreenLayoutParser()
	{
		m_layout = 0;
	}

	// 0x490FC0 (folded)
	void ScreenLayoutParser::setLayout(ScreenLayout* layout)
	{
		m_layout = layout;
	}

	// 0x48CC30
	void ScreenLayoutParser::startDocument()
	{
		m_containers.push_back(m_layout);
	}

	// 0x484C40
	void ScreenLayoutParser::endDocument()
	{
		m_containers.pop_back();
	}

	// 0x48D250
	void ScreenLayoutParser::startElement(const std::string& name, const Properties& attrs)
	{
		if (name == "screen")
		{
			std::string action = attrs.getString("defaultAction", "");
			m_layout->setDefaultAction(action);
			action = attrs.getString("cancelAction", "");
			m_layout->setCancelAction(action);
			std::string screenName = attrs.getString("name", "[Untitled]");
			m_layout->setName(screenName);
		}
		else if (name == "image")
			parseImage(attrs);
		else if (name == "text")
			parseText(attrs);
		else if (name == "imageButton")
			parseImageButton(attrs);
		else if (name == "checkBox")
			parseCheckBox(attrs);
		else if (name == "textButton")
			parseTextButton(attrs);
		else if (name == "group")
			parseGroup(attrs);
		else if (name == "selector")
			parseSelector(attrs);
		else if (name == "particleSystem")
			parseParticleSystem(attrs);
		else if (name == "textSub")
			parseTextSub(attrs);
		else if (name == "editBox")
			parseEditBox(attrs);
		else if (name == "include")
			parseInclude(attrs);
		else if (name == "radioGroup")
			parseRadioGroup(attrs);
		else if (name == "listBox")
			parseListBox(attrs);
		else if (name == "table")
			parseTable(attrs);
		else if (m_table && name == "column")
			parseColumn(attrs);
		else if (name == "keyAction")
			parseKeyAction(attrs);
	}

	// 0x48B7F0
	void ScreenLayoutParser::endElement(const std::string& name)
	{
		if (name == "group" || name == "selector")
		{
			if (m_containers.size() < 2)
			{
				// a named local: built before the message string dies, then copied into the throw object
				Exception error("Mis-matched end group");
				throw error;
			}
			m_containers.pop_back();
		}
		else if (name == "radioGroup")
			endRadioGroup();
		else if (name == "table")
		{
			m_table->updateBounds();
			m_table = 0;
		}
	}

	// 0x48B990
	bool ScreenLayoutParser::parseKeyAction(const Properties& attrs)
	{
		std::string action = attrs.getString("action", "");
		std::string type = attrs.getString("type", "ascii");
		std::string key = attrs.getString("key", "");
		if (!key.empty() && !action.empty())
		{
			int code;
			if (type == "ascii")
				code = toupper(key[0]);
			else
				code = attrs.getInt("key", 0);
			if (code > 0 && !m_layout->addKeyAction(code, action))
				getApplication()->log("Warning: duplicate action for key code: %d\n", code);
		}
		return true;
	}

	// 0x48C090
	bool ScreenLayoutParser::parseInclude(const Properties& attrs)
	{
		ScreenLayoutParser parser;
		parser.m_layout = m_layout;
		std::string file = attrs.getString("file", "");
		if (file != "")
		{
			try
			{
				getApplication()->loadXml(file, &parser);
			}
			catch (...)
			{
				return false;
			}
		}
		return true;
	}

	// 0x48C2A0
	bool ScreenLayoutParser::parseGroup(const Properties& attrs)
	{
		FadeContainer* group = new FadeContainer();
		std::string name = attrs.getString("name", "_Group_");
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		float opacity = attrs.getFloat("opacity", 1.0f);
		float scaleX = attrs.getFloat("scaleX", 1.0f);
		float scaleY = attrs.getFloat("scaleY", 1.0f);
		float width = attrs.getFloat("width", 0.0f);
		float height = attrs.getFloat("height", 0.0f);
		group->setPosition(x, y);
		group->setScale(scaleX, scaleY);
		group->setAlpha(opacity);
		group->setName(name);
		group->setRect(0.0f, 0.0f, width, height);
		group->updateBounds();
		m_containers.back()->addChild(group);
		m_layout->addComponent(group, name.c_str());
		m_containers.push_back(group);
		return true;
	}

	// 0x48C770
	bool ScreenLayoutParser::parseSelector(const Properties& attrs)
	{
		Selector* selector = new Selector();
		std::string name = attrs.getString("name", "_Selector_");
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		float opacity = attrs.getFloat("opacity", 1.0f);
		float scaleX = attrs.getFloat("scaleX", 1.0f);
		float scaleY = attrs.getFloat("scaleY", 1.0f);
		float width = attrs.getFloat("width", 0.0f);
		float height = attrs.getFloat("height", 0.0f);
		selector->setPosition(x, y);
		selector->setScale(scaleX, scaleY);
		selector->setAlpha(opacity);
		selector->setName(name);
		selector->setRect(0.0f, 0.0f, width, height);
		m_containers.back()->addChild(selector);
		m_layout->addComponent(selector, name.c_str());
		m_containers.push_back(selector);
		return true;
	}

	// 0x48CC90
	bool ScreenLayoutParser::parseRadioGroup(const Properties& attrs)
	{
		if (m_radioGroup)
			return false;

		m_radioGroup = new RadioGroup();
		m_containers.back()->addChild(m_radioGroup);
		std::string name = attrs.getString("name", "");
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		float opacity = attrs.getFloat("opacity", 1.0f);
		float scaleX = attrs.getFloat("scaleX", 1.0f);
		float scaleY = attrs.getFloat("scaleY", 1.0f);
		float width = attrs.getFloat("width", 0.0f);
		float height = attrs.getFloat("height", 0.0f);
		m_radioGroup->setPosition(x, y);
		m_radioGroup->setScale(scaleX, scaleY);
		m_radioGroup->setAlpha(opacity);
		m_radioGroup->setName(name);
		m_radioGroup->setRect(0.0f, 0.0f, width, height);
		m_layout->addComponent(m_radioGroup, name.c_str());
		m_initialSelect = attrs.getString("initialSelect", "");
		m_containers.push_back(m_radioGroup);
		return true;
	}

	// 0x484DA0
	void ScreenLayoutParser::endRadioGroup()
	{
		if (m_radioGroup)
		{
			m_containers.pop_back();
			m_layout->connectRadioGroup(m_radioGroup);
			m_radioGroup->setSelection(m_initialSelect);
		}
		m_radioGroup = 0;
		m_initialSelect.clear();
	}

	// 0x484EA0
	bool ScreenLayoutParser::parseImage(const Properties& attrs)
	{
		std::string file = attrs.getString("file", "");
		std::string name = attrs.getString("name", "_Image_");
		if (file == "")
			return false;

		Bitmap* bitmap = getApplication()->getImage(file.c_str());
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		float opacity = attrs.getFloat("opacity", 1.0f);
		float scaleX = attrs.getFloat("scaleX", 1.0f);
		float scaleY = attrs.getFloat("scaleY", 1.0f);
		std::string alphaType = attrs.getString("alphaType", "");
		float alphaThresh = attrs.getFloat("alphaThresh", 5.0f);
		if (alphaType == "alphaTest")
			bitmap->setAlphaType(1, (int)alphaThresh);
		else if (alphaType == "alphaBlend")
			bitmap->setAlphaType(2, 0);
		else if (alphaType == "none")
			bitmap->setAlphaType(0, 0);

		// the bitmap is scaled once here; the component keeps only the signs (mirroring)
		if (scaleX != 1.0f || scaleY != 1.0f)
			bitmap = bitmap->copyScaled(scaleX, scaleY);
		scaleX = scaleX < 0.0f ? -1.0f : 1.0f;
		scaleY = scaleY < 0.0f ? -1.0f : 1.0f;

		std::string pivot = attrs.getString("pivot", "");
		if (pivot == "center")
			bitmap->setPivotType(1);
		else if (pivot == "upperleft")
			bitmap->setPivotType(0);

		Image* image = new Image(bitmap);
		image->setPosition(x, y);
		image->setAlpha(opacity);
		image->setScale(scaleX, scaleY);
		image->setName(name);
		std::string smoothType = attrs.getString("smoothType", "");
		if (smoothType == "smoothAll")
			image->setBlendMode(1);
		else
			image->setBlendMode(0);
		m_layout->addComponent(image, name);
		m_containers.back()->addChild(image);
		return true;
	}

	// 0x4858A0
	bool ScreenLayoutParser::parseImageButton(const Properties& attrs)
	{
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		float opacity = attrs.getFloat("opacity", 1.0f);
		float scaleX = attrs.getFloat("scaleX", 1.0f);
		float scaleY = attrs.getFloat("scaleY", 1.0f);
		std::string upImg = attrs.getString("upImg", "");
		std::string overImg = attrs.getString("overImg", "");
		std::string activeImg = attrs.getString("activeImg", "");
		std::string name = attrs.getString("name", "_ImgButton_");
		std::string action = attrs.getString("action", "defaultAction");
		std::string overSound = attrs.getString("overSound", "");
		std::string activeSound = attrs.getString("activeSound", "");
		Bitmap* up = getApplication()->getImage(upImg.c_str());
		Vector2 upScale = attrs.getPoint("upScale", Vector2(1.0f, 1.0f));
		Vector2 overScale = attrs.getPoint("overScale", Vector2(1.0f, 1.0f));
		float scaleTime = attrs.getFloat("scaleTime", 0.5f);
		std::string pivot = attrs.getString("pivot", "upperLeft");
		Bitmap* over;
		if (overImg != "")
			over = getApplication()->getImage(overImg.c_str());
		else
			over = 0;
		Bitmap* active;
		if (activeImg != "")
			active = getApplication()->getImage(activeImg.c_str());
		else
			active = 0;
		if (!up)
			return false;

		ImageButton* button = new ImageButton();
		if (pivot == "center")
			button->setHotspotMode(1);
		if (!button->setImages(up, over, active))
			return false;
		button->setPosition(x, y);
		button->setAlpha(opacity);
		button->setScale(scaleX, scaleY);
		button->setHoverScale(overScale);
		button->setNormalScale(upScale);
		button->setBaseScale(Vector2(scaleX, scaleY));
		button->setSounds(overSound, activeSound);
		button->setScaleDuration(scaleTime);
		button->setName(name);
		m_layout->addButton(button, name, action);
		m_layout->addComponent(button, name);
		m_containers.back()->addChild(button);
		return true;
	}

	// 0x4867A0
	bool ScreenLayoutParser::parseCheckBox(const Properties& attrs)
	{
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		float opacity = attrs.getFloat("opacity", 1.0f);
		float scaleX = attrs.getFloat("scaleX", 1.0f);
		float scaleY = attrs.getFloat("scaleY", 1.0f);
		std::string uncheckedImg = attrs.getString("uncheckedImg", "");
		std::string uncheckedOverImg = attrs.getString("uncheckedOverImg", "");
		std::string checkedImg = attrs.getString("checkedImg", "");
		std::string checkedOverImg = attrs.getString("checkedOverImg", "");
		std::string overSound = attrs.getString("overSound", "");
		std::string activeSound = attrs.getString("activeSound", "");
		std::string name = attrs.getString("name", "_ImgButton_");
		std::string action = attrs.getString("action", "defaultAction");
		std::string pivot = attrs.getString("pivot", "upperLeft");
		Bitmap* unchecked = getApplication()->getImage(uncheckedImg.c_str());
		Bitmap* checked = getApplication()->getImage(checkedImg.c_str());
		Bitmap* uncheckedOver = 0;
		Bitmap* checkedOver = 0;
		bool startOn = attrs.getBool("startOn", false);
		if (!uncheckedOverImg.empty())
			uncheckedOver = getApplication()->getImage(uncheckedOverImg.c_str());
		if (!checkedOverImg.empty())
			checkedOver = getApplication()->getImage(checkedOverImg.c_str());
		if (!checked || !unchecked)
			return false;

		// the implicit constructor (not value-initialised: no parentheses)
		Checkbox* checkbox = new Checkbox;
		if (pivot == "center")
			checkbox->setHotspotMode(1);
		if (!checkbox->create(unchecked, uncheckedOver, checked, checkedOver))
			return false;
		checkbox->setPosition(x, y);
		checkbox->setAlpha(opacity);
		checkbox->setScale(scaleX, scaleY);
		checkbox->setSounds(overSound, activeSound);
		checkbox->setName(name);
		if (m_radioGroup)
			m_radioGroup->addButton(checkbox);
		else
			m_layout->addButton(checkbox, name, action);
		m_layout->addComponent(checkbox, name);
		m_containers.back()->addChild(checkbox);
		checkbox->setChecked(startOn);
		return true;
	}

	// 0x487680
	bool ScreenLayoutParser::parseTextButton(const Properties& attrs)
	{
		std::string text = attrs.getString("string", "");
		std::string name = attrs.getString("name", "_Text_");
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		std::string xAlign = attrs.getString("xAlign", "");
		std::string yAlign = attrs.getString("yAlign", "");
		std::string font = attrs.getString("font", "");
		std::string overFont = attrs.getString("overFont", "");
		std::string activeFont = attrs.getString("activeFont", overFont);
		std::string action = attrs.getString("action", "defaultAction");
		std::string overSound = attrs.getString("overSound", "");
		std::string activeSound = attrs.getString("activeSound", "");
		Color upColor;
		Color overColor;
		Color activeColor;
		upColor = attrs.getColor("upAddColor", Color(0, 0, 0));
		overColor = attrs.getColor("overAddColor", Color(0, 0, 0));
		activeColor = attrs.getColor("activeAddColor", Color(0, 0, 0));
		if (font.empty() || overFont.empty())
			return false;

		TextButton* button = new TextButton();
		if (!button->create(font, overFont, activeFont))
			return false;
		button->setText(text);
		if (xAlign == "center")
			button->setXAlign(1);
		else if (xAlign == "right")
			button->setXAlign(2);
		else
			button->setXAlign(0);
		if (yAlign == "middle")
			button->setYAlign(1);
		else if (yAlign == "bottom")
			button->setYAlign(2);
		else
			button->setYAlign(0);
		button->setPosition(x, y);
		button->setSounds(overSound, activeSound);
		button->setName(name);
		button->setTextColors(upColor, overColor, activeColor);
		m_layout->addButton(button, name, action);
		m_layout->addComponent(button, name);
		m_containers.back()->addChild(button);
		return true;
	}

	// 0x488550
	bool ScreenLayoutParser::parseText(const Properties& attrs)
	{
		std::string text = attrs.getString("string", "");
		std::string font = attrs.getString("font", "");
		std::string name = attrs.getString("name", "_Text_");
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		float opacity = attrs.getFloat("opacity", 1.0f);
		Color color = attrs.getColor("color", Color(255, 255, 255));
		int wordWrap = attrs.getInt("wordWrap", 0);
		std::string xAlign = attrs.getString("xAlign", "");
		std::string yAlign = attrs.getString("yAlign", "");
		std::string style = attrs.getString("style", "");
		if ((text == "" && name == "") || font == "")
			return false;

		TextItem* item = new TextItem();
		if (wordWrap > 0)
			item->m_wordWrap = wordWrap;
		item->setFont(font.c_str());
		item->setText(text.c_str());
		// getColor gives 0..1 channels, so the comparison with 255 always differs
		if (color.r != 255.0f || color.g != 255.0f || color.b != 255.0f)
		{
			item->setColorMode(2);
			item->setColor(color);
		}
		if (xAlign == "center")
			item->setXAlign(1);
		else if (xAlign == "right")
			item->setXAlign(2);
		else
			item->setXAlign(0);
		if (yAlign == "middle")
			item->setYAlign(1);
		else if (yAlign == "bottom")
			item->setYAlign(2);
		else
			item->setYAlign(0);
		if (style.find("underline") != std::string::npos)
			item->m_style |= 1;
		if (style.find("strikethrough") != std::string::npos)
			item->m_style |= 2;
		if (style.find("shadow") != std::string::npos)
			item->m_style |= 4;
		Vector2 shadowOffset = attrs.getPoint("shadowOffset", Vector2(1.0f, 1.0f));
		float shadowOpacity = attrs.getFloat("shadowOpacity", 1.0f);
		item->setShadow(shadowOpacity, shadowOffset);
		item->setPosition(x, y);
		item->setAlpha(opacity);
		item->setName(name);
		m_layout->addComponent(item, name.c_str());
		m_containers.back()->addChild(item);
		return true;
	}

	// 0x4892C0
	bool ScreenLayoutParser::parseParticleSystem(const Properties& attrs)
	{
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		std::string file = attrs.getString("file", "");
		std::string name = attrs.getString("name", "ParticleSystem");
		int warmup = attrs.getInt("warmup", 0);
		if (file != "")
		{
			// added before loading; not registered with the layout
			ParticleSystem* particles = new ParticleSystem();
			m_containers.back()->addChild(particles);
			particles->load(file);
			particles->setPosition(x, y);
			particles->start();
			particles->setName(name);
			if (warmup > 0)
				particles->advance((float)warmup);
		}
		return true;
	}

	// 0x4896D0
	bool ScreenLayoutParser::parseTextSub(const Properties& attrs)
	{
		std::string name = attrs.getString("name", "");
		std::string text = attrs.getString("string", "");
		TextItem* item = static_cast<TextItem*>(m_layout->getComponent(name));
		if (item)
			item->setText(text);
		return true;
	}

	// 0x489910
	bool ScreenLayoutParser::parseEditBox(const Properties& attrs)
	{
		EditBox* editBox = new EditBox();
		m_containers.back()->addChild(editBox);
		std::string name = attrs.getString("name", "_EditBox_");
		std::string font = attrs.getString("font", "");
		std::string bgImg = attrs.getString("bgImg", "");
		std::string cursorImg = attrs.getString("cursorImg", "");
		editBox->init(bgImg, cursorImg, font);
		std::string action = attrs.getString("action", "");
		editBox->setAction(action);
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		float opacity = attrs.getFloat("opacity", 1.0f);
		editBox->setPosition(x, y);
		editBox->setAlpha(opacity);
		Vector2 offset = attrs.getPoint("cursorOffset", Vector2(0.0f, 0.0f));
		editBox->setCursorOffset(offset);
		float blinkTime = attrs.getFloat("cursorBlinkTime", 1.0f);
		editBox->setCursorBlinkTime(blinkTime);
		offset = attrs.getPoint("textOffset", Vector2(0.0f, 0.0f));
		editBox->setTextOffset(offset);
		int maxLength = attrs.getInt("strLen", 12);
		editBox->setMaxLength(maxLength);
		// only registered: setName is never called
		m_layout->addComponent(editBox, name.c_str());
		return true;
	}

	// 0x48A160
	bool ScreenLayoutParser::parseListBox(const Properties& attrs)
	{
		ListBox* listBox = new ListBox();
		std::string name = attrs.getString("name", "_ListBox_");
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		float opacity = attrs.getFloat("opacity", 1.0f);
		float scaleX = attrs.getFloat("scaleX", 1.0f);
		float scaleY = attrs.getFloat("scaleY", 1.0f);
		float width = attrs.getFloat("width", 0.0f);
		float height = attrs.getFloat("height", 0.0f);
		int indent = attrs.getInt("indent", 0);
		std::string overSound = attrs.getString("overSound", "");
		std::string activeSound = attrs.getString("activeSound", "");
		Color backgroundColor = attrs.getColor("backgroundColor", listBox->getBackgroundColor());
		Color textColor = attrs.getColor("textColor", listBox->getTextColor());
		Color highlightColor = attrs.getColor("highlightColor", listBox->getHighlightColor());
		Color selectionColor = attrs.getColor("selectionColor", listBox->getSelectionBackgroundColor());
		selectionColor = attrs.getColor("selectionBackgroundColor", selectionColor);
		Color selectionTextColor = attrs.getColor("selectionTextColor", textColor);
		Color selectionHighlightColor = attrs.getColor("selectionHighlightColor", highlightColor);
		std::string fontName = attrs.getString("font", "");
		Font* font = getApplication()->getFont(fontName.c_str());
		listBox->setPosition(x, y);
		listBox->setScale(scaleX, scaleY);
		listBox->setAlpha(opacity);
		listBox->setName(name);
		listBox->setSize(width, height);
		listBox->setFont(font);
		listBox->setBackgroundColor(backgroundColor);
		listBox->setTextColor(textColor);
		listBox->setHighlightColor(highlightColor);
		listBox->setSelectionBackgroundColor(selectionColor);
		listBox->setSelectionTextColor(selectionTextColor);
		listBox->setSelectionHighlightColor(selectionHighlightColor);
		listBox->setIndent(indent);
		listBox->setSounds(overSound, activeSound);
		listBox->updateBounds();
		m_containers.back()->addChild(listBox);
		m_layout->addComponent(listBox, name.c_str());
		return true;
	}

	// 0x48ADE0
	bool ScreenLayoutParser::parseTable(const Properties& attrs)
	{
		// stays open for <column> until </table>
		m_table = new Table();
		std::string name = attrs.getString("name", "_Table_");
		float x = attrs.getFloat("x", 0.0f);
		float y = attrs.getFloat("y", 0.0f);
		int rows = attrs.getInt("rows", 10);
		Color backgroundColor = attrs.getColor("backgroundColor", m_table->getBackgroundColor());
		Color textColor = attrs.getColor("textColor", m_table->getTextColor());
		Color selectionBackgroundColor = attrs.getColor("selectionBackgroundColor", backgroundColor);
		Color selectionTextColor = attrs.getColor("selectionTextColor", textColor);
		std::string fontName = attrs.getString("font", "");
		Font* font = getApplication()->getFont(fontName.c_str());
		m_table->setName(name);
		m_table->setPosition(x, y);
		m_table->setFont(font);
		m_table->setVisibleRows(rows);
		m_table->setBackgroundColor(backgroundColor);
		m_table->setTextColor(textColor);
		m_table->setSelectionBackgroundColor(selectionBackgroundColor);
		m_table->setSelectionTextColor(selectionTextColor);
		m_containers.back()->addChild(m_table);
		m_layout->addComponent(m_table, name.c_str());
		return true;
	}

	// 0x48B4F0
	bool ScreenLayoutParser::parseColumn(const Properties& attrs)
	{
		std::string name = attrs.getString("name", "");
		int width = attrs.getInt("width", 0);
		std::string xAlign = attrs.getString("xAlign", "");
		int align;
		if (xAlign == "center")
			align = 1;
		else if (xAlign == "right")
			align = 2;
		else
			align = 0;
		int column = m_table->addColumn();
		m_table->setColumnName(column, name);
		m_table->setColumnWidth(column, width);
		m_table->setColumnAlign(column, align);
		return true;
	}
}
