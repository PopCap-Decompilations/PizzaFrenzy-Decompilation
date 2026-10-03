// engine::ScreenLayout: a screen loaded from an XML layout; forwards its button, radio-group and key actions to an
// ActionListener.
#pragma once

#include <map>
#include <string>

#include "ButtonListener.h"
#include "FadeContainer.h"
#include "sigslot.h"

namespace engine
{
	class ActionListener;
	class Button;
	class Component;
	class RadioGroup;

	// FadeContainer +0x00, ButtonListener +0x144, sigslot::has_slots<> +0x148, members from +0x158, then the vtordisp
	// (+0x1AC) and the Interface subobject (+0x1B0): 0x1B4 bytes. While active it is connected to the application's
	// key-down signal (onKeyDown). getTypeName is inline (its copy 0x406470 is in the game's objects).
	class ScreenLayout : public FadeContainer, public ButtonListener, public sigslot::has_slots<>
	{
	public:
		ScreenLayout();
		virtual ~ScreenLayout();

		// 0x406470
		virtual std::string getTypeName() const							// slot 24 (engine::Component)
		{
			return "ScreenLayout";
		}

		virtual void activate();										// slot 34 (engine::Component): key-down, input
		virtual void deactivate();										// slot 35 (engine::Component)
		virtual void addChild(Component* child);						// slot 71 (engine::Container): Container::addChild

		virtual bool load(std::string path);							// slot 79: false if the layout throws
		virtual void addComponent(Component* component, std::string name);			// slot 80: by the layout parser
		virtual void addButton(Button* button, std::string name, std::string action);	// slot 81: name unused
		virtual void connectRadioGroup(RadioGroup* group);				// slot 82: selection -> doAction
		virtual void doAction(const std::string& action);				// slot 83
		virtual void setDefaultAction(const std::string& action);		// slot 84: <screen defaultAction>, VK_RETURN
		virtual void setCancelAction(const std::string& action);		// slot 85: <screen cancelAction>, VK_ESCAPE
		virtual void onKeyDown(int keyCode);							// slot 86
		virtual bool addKeyAction(int keyCode, const std::string& action);	// slot 87: false for a duplicate key
		virtual void clearKeyActions();									// slot 88

		// ButtonListener (+0x144): only the click is used; the other four ignore the command
		virtual void onMouseEnter(std::string command);					// slot 0 (0x496410 folded)
		virtual void onMouseLeave(std::string command);					// slot 1 (0x496410 folded)
		virtual void onMouseDown(std::string command);					// slot 2 (0x496410 folded)
		virtual void onMouseUp(std::string command);					// slot 3 (0x496410 folded)
		virtual void onClick(std::string command);						// slot 4: m_actionListener->onAction

		Component* getComponent(const std::string& name);
		void setActionListener(ActionListener* listener);				// 0x47A140 (folded with Button::setScaleDuration)

		std::map<std::string, Component*> m_components;		// +0x158 named components (addComponent)
		ActionListener* m_actionListener;					// +0x164 (engine::Screen: its own +0x1AC interface)
		std::map<int, std::string> m_keyActions;			// +0x168 key code -> action
		std::string m_defaultAction;						// +0x174
		std::string m_cancelAction;							// +0x190
	};
}
