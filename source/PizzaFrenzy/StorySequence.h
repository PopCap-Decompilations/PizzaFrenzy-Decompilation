// The story scripts (res\story\*.xml): StorySequence, its StoryActions and the XML handlers that build them.
#pragma once

#include <string>
#include <vector>

#include "engine/Object.h"
#include "engine/RefPtr.h"
#include "engine/XmlElementHandler.h"

namespace engine
{
	class Component;
	class Properties;
	class ScreenLayout;
	class TextTyper;
	class XmlHandlerStack;
	struct UpdateContext;
}

class StoryScreen;
class StorySequence;

// One step of a StorySequence: setSequence() attaches it to the running sequence, start() makes it current, then
// update() runs every frame until isDone(); the story screen's events ("fastForwardStep", "continue") reach onEvent().
// The member ends at +0x10; the vtordisp (+0x10) and the Interface subobject (+0x14) follow it (0x18 bytes).
class StoryAction : public engine::Object
{
public:
	virtual ~StoryAction();

	virtual void start() = 0;													// slot 1
	virtual void stop() = 0;													// slot 2
	virtual void setSequence(StorySequence* sequence);							// slot 3
	virtual void update(engine::UpdateContext& info) = 0;						// slot 4
	virtual bool isDone() const = 0;											// slot 5
	virtual void onEvent(const std::string& event);								// slot 6: empty default

	engine::RefPtr<StorySequence> m_sequence;									// +0x0C set by setSequence
};

// The actions: StoryAction +0x00, own members from +0x10, then the vtordisp and the Interface subobject.

// <dialog speaker="" text=""/>: types the text into the speaker's speech bubble on the story screen, 20 times faster
// after "fastForwardStep" (0x54 bytes).
class DialogAction : public StoryAction
{
public:
	DialogAction();
	DialogAction(const std::string& text, const std::string& speaker);
	virtual ~DialogAction();

	virtual void start();														// slot 1
	virtual void stop();														// slot 2
	virtual void setSequence(StorySequence* sequence);							// slot 3
	virtual void update(engine::UpdateContext& info);							// slot 4: empty
	virtual bool isDone() const;												// slot 5: always true
	virtual void onEvent(const std::string& event);								// slot 6

	std::string m_text;															// +0x10 "text"
	std::string m_speaker;														// +0x2C "speaker" (default "nobody")
	engine::RefPtr<engine::TextTyper> m_textBox;								// +0x48 the speaker's text
};

// <event action="" param=""/>: sends one action to the story screen and finishes (0x54 bytes).
class EventAction : public StoryAction
{
public:
	EventAction();
	EventAction(const std::string& action, const std::string& param);
	virtual ~EventAction();

	virtual void start();														// slot 1
	virtual void stop();														// slot 2: empty
	virtual void setSequence(StorySequence* sequence);							// slot 3
	virtual void update(engine::UpdateContext& info);							// slot 4: empty
	virtual bool isDone() const;												// slot 5

	bool m_done;																// +0x10
	std::string m_action;														// +0x14 "action"
	std::string m_param;														// +0x30 "param"
};

// <image dest="" img=""/>: shows the image in the screen's container 'dest', or fades 'dest' out if there is no such
// image (0x54 bytes).
class ImageAction : public StoryAction
{
public:
	ImageAction();
	virtual ~ImageAction();

	virtual void start();														// slot 1
	virtual void stop();														// slot 2: empty
	virtual void setSequence(StorySequence* sequence);							// slot 3
	virtual void update(engine::UpdateContext& info);							// slot 4: empty
	virtual bool isDone() const;												// slot 5

	std::string m_dest;															// +0x10 "dest"
	std::string m_img;															// +0x2C "img"
	bool m_done;																// +0x48
};

// <layout dest="" file=""/>: shows a ScreenLayout loaded from 'file' in the screen's container 'dest' (0x3C bytes).
class LayoutAction : public StoryAction
{
public:
	LayoutAction();
	virtual ~LayoutAction();

	virtual void start();														// slot 1
	virtual void stop();														// slot 2: empty
	virtual void setSequence(StorySequence* sequence);							// slot 3
	virtual void update(engine::UpdateContext& info);							// slot 4: empty
	virtual bool isDone() const;												// slot 5

	std::string m_dest;															// +0x10 "dest"
	engine::RefPtr<engine::ScreenLayout> m_layout;								// +0x2C loaded by LayoutHandler
	bool m_done;																// +0x30
};

// <pause time=""/>: waits 'time' seconds; "fastForwardStep" ends it (0x24 bytes).
class PauseAction : public StoryAction
{
public:
	PauseAction(float time);
	PauseAction();
	virtual ~PauseAction();

	virtual void start();														// slot 1
	virtual void stop();														// slot 2
	virtual void setSequence(StorySequence* sequence);							// slot 3
	virtual void update(engine::UpdateContext& info);							// slot 4
	virtual bool isDone() const;												// slot 5
	virtual void onEvent(const std::string& event);								// slot 6

	bool m_running;																// +0x10
	bool m_done;																// +0x11
	float m_timeLeft;															// +0x14
	float m_time;																// +0x18 "time" (default 1.0)
};

// <sound name=""/>: plays a sound cue and finishes (0x38 bytes).
class SoundAction : public StoryAction
{
public:
	SoundAction();
	virtual ~SoundAction();

	virtual void start();														// slot 1
	virtual void stop();														// slot 2: empty
	virtual void setSequence(StorySequence* sequence);							// slot 3
	virtual void update(engine::UpdateContext& info);							// slot 4: empty
	virtual bool isDone() const;												// slot 5

	std::string m_name;															// +0x10 "name"
	bool m_done;																// +0x2C
};

// <waitEvent preAction="" waitAction="" delayAction="" delay=""/>: sends preAction, sends delayAction after 'delay'
// seconds, and finishes when the screen forwards the event named waitAction (0x74 bytes).
class WaitEventAction : public StoryAction
{
public:
	WaitEventAction();
	WaitEventAction(const std::string& preAction, const std::string& waitAction, const std::string& delayAction,
		float delay);
	virtual ~WaitEventAction();

	virtual void start();														// slot 1
	virtual void stop();														// slot 2: empty
	virtual void setSequence(StorySequence* sequence);							// slot 3
	virtual void update(engine::UpdateContext& info);							// slot 4
	virtual bool isDone() const;												// slot 5
	virtual void onEvent(const std::string& event);								// slot 6

	bool m_done;																// +0x10
	std::string m_preAction;													// +0x14 "preAction"
	std::string m_waitAction;													// +0x30 "waitAction"
	std::string m_delayAction;													// +0x4C "delayAction"
	float m_delay;																// +0x68 "delay" (default 0), counted down
};

// A <sequence name=""> of a story XML (or built in code by StoryScreen): its actions run one after another against
// the story screen. The members end at +0x44; the vtordisp (+0x44) and the Interface subobject (+0x48) follow them
// (0x4C bytes).
class StorySequence : public engine::Object
{
public:
	StorySequence();
	virtual ~StorySequence();

	void setScreen(StoryScreen* screen);
	engine::TextTyper* getSpeakerText(const std::string& speaker);
	void doAction(const std::string& action, const std::string& param);
	void showInLayout(const std::string& dest, engine::Component* content);
	void showImage(const std::string& dest, const std::string& img);
	void start();
	void stop();
	void nextAction();
	void onEvent(const std::string& event);
	void update(engine::UpdateContext& info);
	void clear();
	void addAction(StoryAction* action);
	bool isDone() const;														// m_finished (StoryScreen::updateStory)

	int m_current;																// +0x0C index of the running action
	bool m_running;																// +0x10
	bool m_finished;															// +0x11 set when the last action is done
	std::string m_name;															// +0x14 "name" (default "No Name")
	std::vector<engine::RefPtr<StoryAction> > m_actions;						// +0x30
	engine::RefPtr<StoryScreen> m_screen;										// +0x40 the screen the actions talk to
};

// The XML handlers of a story file. They share one folded destructor-only vtable (0x4FFC54) and have implicit
// destructors. engine::XmlElementHandler +0x00 with its engine::XmlHandler interface at +0x0C; the element handlers
// add no members (0x3C bytes): their constructors create the action and add it to the sequence.

// Root handler of a story XML ("story", made by StoryScreen::loadStory): startElement("sequence") creates the
// sequence, the other elements push the element handlers below. The member ends at +0x38; the vtordisp (+0x38) and
// the Interface subobject (+0x3C) follow it (0x40 bytes).
class StoryHandler : public engine::XmlElementHandler
{
public:
	StoryHandler(const std::string& name, engine::XmlHandlerStack* reader);

	// slot 0 of the engine::XmlHandler interface (+0x0C)
	virtual void startElement(const std::string& name, const engine::Properties& attributes);

	StorySequence* m_sequence;													// +0x34 not set by the constructor
};

// <dialog speaker="" text=""/>: adds a DialogAction.
class DialogHandler : public engine::XmlElementHandler
{
public:
	DialogHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
		StorySequence* sequence);
};

// <event action="" param=""/>: adds an EventAction.
class EventHandler : public engine::XmlElementHandler
{
public:
	EventHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
		StorySequence* sequence);
};

// <image dest="" img=""/>: adds an ImageAction.
class ImageHandler : public engine::XmlElementHandler
{
public:
	ImageHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
		StorySequence* sequence);
};

// <layout dest="" file=""/>: adds a LayoutAction with its ScreenLayout.
class LayoutHandler : public engine::XmlElementHandler
{
public:
	LayoutHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
		StorySequence* sequence);
};

// <pause time=""/>: adds a PauseAction.
class PauseHandler : public engine::XmlElementHandler
{
public:
	PauseHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
		StorySequence* sequence);
};

// <sound name=""/>: adds a SoundAction.
class SoundHandler : public engine::XmlElementHandler
{
public:
	SoundHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
		StorySequence* sequence);
};

// <waitEvent preAction="" waitAction="" delayAction="" delay=""/>: adds a WaitEventAction.
class WaitEventHandler : public engine::XmlElementHandler
{
public:
	WaitEventHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
		StorySequence* sequence);
};
