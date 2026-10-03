#include "StorySequence.h"

#include <string>
#include <vector>

#include "engine/Component.h"
#include "engine/FadeContainer.h"
#include "engine/Image.h"
#include "engine/Properties.h"
#include "engine/ScreenLayout.h"
#include "engine/SoundMgr.h"
#include "engine/Surface.h"
#include "engine/TextTyper.h"
#include "engine/Xml.h"

#include "Constants.h"
#include "PizzaFrenzy.h"
#include "StoryScreen.h"

// 0x44B590
void DialogAction::setSequence(StorySequence* sequence)
{
	StoryAction::setSequence(sequence);
	m_textBox = m_sequence->getSpeakerText(m_speaker);
	m_textBox->setText(m_text);
	m_textBox->reset();
}

// 0x44B600
void DialogAction::stop()
{
	m_textBox->pause();
}

// 0x44B610
DialogAction::DialogAction()
{
	m_textBox = 0;
}

// 0x44B6F0
DialogAction::~DialogAction()
{
	m_textBox = 0;
}

// 0x44B7F0
DialogAction::DialogAction(const std::string& text, const std::string& speaker)
	: m_text(text), m_speaker(speaker)
{
}

// 0x44B8D0
void DialogAction::start()
{
	m_sequence->doAction("dialogStart", m_speaker);
	m_textBox->setup(g_typerWidth, g_typerSpeed, g_typerDelay);
	m_textBox->setText(m_text);
	m_textBox->start();
}

// 0x492310 (folded)
void DialogAction::update(engine::UpdateContext& info)
{
}

// 0x451610 (folded)
bool DialogAction::isDone() const
{
	return true;
}

// 0x44B9A0
void DialogAction::onEvent(const std::string& event)
{
	if (event == "fastForwardStep")
	{
		m_textBox->setup(g_typerWidth, g_typerSpeed * 20.0f, 0.0f);
		PizzaFrenzy::getSounds()->playSound("order_click1", 1.0f, 1.0f);
	}
}

// 0x44BA70
DialogHandler::DialogHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
	StorySequence* sequence)
	: engine::XmlElementHandler(name, reader)
{
	DialogAction* action = new DialogAction();
	action->m_text = attrs.getString("text",
		"I have nothing to say... Except there was not text in my script, so something is wrong");
	action->m_speaker = attrs.getString("speaker", "nobody");
	sequence->addAction(action);
}

// 0x44BCF0 (folded)
void EventAction::setSequence(StorySequence* sequence)
{
	StoryAction::setSequence(sequence);
	m_done = false;
}

// 0x44BD10
void EventAction::start()
{
	m_sequence->doAction(m_action, m_param);
	m_done = true;
}

// 0x4D0470 (folded)
void EventAction::stop()
{
}

// 0x492310 (folded)
void EventAction::update(engine::UpdateContext& info)
{
}

// 0x44E1E0 (folded)
bool EventAction::isDone() const
{
	return m_done;
}

// 0x44BD30
EventAction::EventAction()
{
}

// 0x44BDE0
EventAction::~EventAction()
{
}

// 0x44BE70
EventAction::EventAction(const std::string& action, const std::string& param)
{
	m_action = action;
	m_param = param;
}

// 0x44BF50
EventHandler::EventHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
	StorySequence* sequence)
	: engine::XmlElementHandler(name, reader)
{
	EventAction* action = new EventAction();
	action->m_action = attrs.getString("action", "");
	action->m_param = attrs.getString("param", "");
	sequence->addAction(action);
}

// 0x44C1B0
bool ImageAction::isDone() const
{
	return m_done;
}

// 0x44C1C0
void ImageAction::start()
{
	m_sequence->showImage(m_dest, m_img);
	m_done = true;
}

// 0x4D0470 (folded)
void ImageAction::stop()
{
}

// 0x44C580 (folded)
void ImageAction::setSequence(StorySequence* sequence)
{
	StoryAction::setSequence(sequence);
}

// 0x492310 (folded)
void ImageAction::update(engine::UpdateContext& info)
{
}

// 0x44C1E0
ImageAction::ImageAction()
	: m_done(false)
{
}

// 0x44C290
ImageAction::~ImageAction()
{
}

// 0x44C320
ImageHandler::ImageHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
	StorySequence* sequence)
	: engine::XmlElementHandler(name, reader)
{
	ImageAction* action = new ImageAction();
	action->m_dest = attrs.getString("dest", "");
	action->m_img = attrs.getString("img", "");
	sequence->addAction(action);
}

// 0x44C590
bool LayoutAction::isDone() const
{
	return m_done;
}

// 0x44C5A0
void LayoutAction::start()
{
	m_sequence->showInLayout(m_dest, m_layout);
	m_done = true;
}

// 0x4D0470 (folded)
void LayoutAction::stop()
{
}

// 0x44C580 (folded)
void LayoutAction::setSequence(StorySequence* sequence)
{
	StoryAction::setSequence(sequence);
}

// 0x492310 (folded)
void LayoutAction::update(engine::UpdateContext& info)
{
}

// 0x44C5C0
LayoutAction::LayoutAction()
	: m_done(false)
{
}

// 0x44C670
LayoutAction::~LayoutAction()
{
	m_layout = 0;
}

// 0x44C760
LayoutHandler::LayoutHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
	StorySequence* sequence)
	: engine::XmlElementHandler(name, reader)
{
	LayoutAction* action = new LayoutAction();
	action->m_dest = attrs.getString("dest", "");
	std::string file = attrs.getString("file", "");
	action->m_layout = new engine::ScreenLayout();
	action->m_layout->load(file);
	sequence->addAction(action);
}

// 0x44CA90
PauseAction::~PauseAction()
{
}

// 0x44CAC0
void PauseAction::setSequence(StorySequence* sequence)
{
	StoryAction::setSequence(sequence);
	m_timeLeft = m_time;
	m_running = false;
	m_done = false;
}

// 0x44CAE0
void PauseAction::start()
{
	m_running = true;
}

// 0x44CAF0
void PauseAction::stop()
{
	m_running = false;
}

// 0x44CB00
void PauseAction::update(engine::UpdateContext& info)
{
	if (m_running)
	{
		m_timeLeft -= info.elapsed;
		if (m_timeLeft < 0.0f)
		{
			m_running = false;
			m_done = true;
		}
	}
}

// 0x4797F0 (folded)
bool PauseAction::isDone() const
{
	return m_done;
}

// 0x44CB50
PauseAction::PauseAction(float time)
{
	m_time = time;
	m_running = false;
	m_done = false;
}

// 0x44CC00
PauseAction::PauseAction()
{
	m_running = false;
	m_done = false;
}

// 0x44CCA0
void PauseAction::onEvent(const std::string& event)
{
	if (event == "fastForwardStep")
	{
		m_timeLeft = -1.0f;
		PizzaFrenzy::getSounds()->playSound("order_click1", 1.0f, 1.0f);
	}
}

// 0x44CD50
PauseHandler::PauseHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
	StorySequence* sequence)
	: engine::XmlElementHandler(name, reader)
{
	PauseAction* action = new PauseAction();
	float time = attrs.getTime("time", 1.0f);
	action->m_time = time;
	sequence->addAction(action);
}

// 0x44CE80
void SoundAction::start()
{
	PizzaFrenzy::getSounds()->playSound(m_name, 1.0f, 1.0f);
	m_done = true;
}

// 0x4D0470 (folded)
void SoundAction::stop()
{
}

// 0x44C580 (folded)
void SoundAction::setSequence(StorySequence* sequence)
{
	StoryAction::setSequence(sequence);
}

// 0x492310 (folded)
void SoundAction::update(engine::UpdateContext& info)
{
}

// 0x44CEB0
bool SoundAction::isDone() const
{
	return m_done;
}

// 0x44CEC0
SoundAction::SoundAction()
	: m_done(false)
{
}

// 0x44CF90
SoundAction::~SoundAction()
{
}

// 0x44D000
SoundHandler::SoundHandler(const std::string& name, engine::XmlHandlerStack* reader, const engine::Properties& attrs,
	StorySequence* sequence)
	: engine::XmlElementHandler(name, reader)
{
	SoundAction* action = new SoundAction();
	action->m_name = attrs.getString("name", "");
	sequence->addAction(action);
}

// 0x44D1A0
StoryAction::~StoryAction()
{
	m_sequence = 0;
}

// 0x44D240
void StoryAction::setSequence(StorySequence* sequence)
{
	m_sequence = sequence;
}

// 0x492310 (folded)
void StoryAction::onEvent(const std::string& event)
{
}

// 0x44D300
StoryHandler::StoryHandler(const std::string& name, engine::XmlHandlerStack* reader)
	: engine::XmlElementHandler(name, reader)
{
}

// 0x44D360
void StorySequence::setScreen(StoryScreen* screen)
{
	m_running = false;
	m_current = 0;
	m_finished = false;
	m_screen = screen;
}

// 0x44D3B0
engine::TextTyper* StorySequence::getSpeakerText(const std::string& speaker)
{
	return m_screen->showSpeechBubble(speaker);
}

// 0x44D3C0
void StorySequence::doAction(const std::string& action, const std::string& param)
{
	m_screen->handleStoryEvent(action, param);
}

// 0x44D3D0
void StorySequence::showInLayout(const std::string& dest, engine::Component* content)
{
	engine::FadeContainer* container = static_cast<engine::FadeContainer*>(m_screen->getComponent(dest));
	if (container)
	{
		if (content)
		{
			container->removeAllChildren();
			container->addChild(content);
			container->setAlpha(0.0f);
			container->fade(true, 0.2f, 1.0f, false);
		}
		else
			container->fade(false, 0.2f, 1.0f, false);
	}
}

// 0x44D450
void StorySequence::showImage(const std::string& dest, const std::string& img)
{
	engine::Bitmap* image = m_screen->getStoryImage(img);
	if (image)
		showInLayout(dest, new engine::Image(image));
	else
	{
		engine::FadeContainer* container = static_cast<engine::FadeContainer*>(m_screen->getComponent(dest));
		if (container)
			container->fade(false, 0.2f, 1.0f, false);
	}
}

// 0x44D630
void StorySequence::start()
{
	m_current = 0;
	if (!m_actions.empty())
	{
		m_actions[m_current]->setSequence(this);
		m_actions[m_current]->start();
		m_running = true;
		m_finished = false;
	}
}

// 0x44D680
void StorySequence::stop()
{
	if (!m_actions.empty() && m_current < (int)m_actions.size())
		m_actions[m_current]->stop();
	m_running = false;
}

// 0x44D6D0
void StorySequence::nextAction()
{
	m_current++;
	if (m_current >= (int)m_actions.size())
	{
		if (m_running)
			m_finished = true;
		m_running = false;
	}
	else
	{
		m_actions[m_current]->setSequence(this);
		m_actions[m_current]->start();
	}
}

// 0x44D730
void StorySequence::onEvent(const std::string& event)
{
	if (m_running && !m_actions.empty() && m_current < (int)m_actions.size())
		m_actions[m_current]->onEvent(event);
}

// 0x44D780
void StorySequence::update(engine::UpdateContext& info)
{
	if (m_current >= (int)m_actions.size())
	{
		if (m_running)
			m_finished = true;
		m_running = false;
	}
	else if (m_running && !m_actions.empty())
	{
		m_actions[m_current]->update(info);
		if (m_actions[m_current]->isDone())
			nextAction();
	}
}

// 0x44D8A0
void StorySequence::clear()
{
	m_actions.clear();
	m_screen = 0;
}

// 0x4797F0 (folded)
bool StorySequence::isDone() const
{
	return m_finished;
}

// 0x44DBC0
StorySequence::~StorySequence()
{
	clear();
}

// 0x44DCB0
StorySequence::StorySequence()
{
}

// 0x44DD60
void StoryHandler::startElement(const std::string& name, const engine::Properties& attributes)
{
	if (name == "sequence")
	{
		std::string sequenceName = attributes.getString("name", "No Name");
		m_sequence = new StorySequence();
		m_sequence->m_name = sequenceName;
	}
	else if (name == "dialog")
		m_handlerStack->pushHandler(new DialogHandler(name, m_handlerStack, attributes, m_sequence));
	else if (name == "pause")
		m_handlerStack->pushHandler(new PauseHandler(name, m_handlerStack, attributes, m_sequence));
	else if (name == "waitEvent")
		m_handlerStack->pushHandler(new WaitEventHandler(name, m_handlerStack, attributes, m_sequence));
	else if (name == "image")
		m_handlerStack->pushHandler(new ImageHandler(name, m_handlerStack, attributes, m_sequence));
	else if (name == "sound")
		m_handlerStack->pushHandler(new SoundHandler(name, m_handlerStack, attributes, m_sequence));
	else if (name == "layout")
		m_handlerStack->pushHandler(new LayoutHandler(name, m_handlerStack, attributes, m_sequence));
	else if (name == "event")
		m_handlerStack->pushHandler(new EventHandler(name, m_handlerStack, attributes, m_sequence));
}

// 0x44E160
void StorySequence::addAction(StoryAction* action)
{
	m_actions.push_back(action);
}

// 0x44E1F0
WaitEventAction::WaitEventAction()
{
}

// 0x4D0470 (folded)
void WaitEventAction::stop()
{
}

// 0x44BCF0 (folded)
void WaitEventAction::setSequence(StorySequence* sequence)
{
	StoryAction::setSequence(sequence);
	m_done = false;
}

// 0x44E1E0 (folded)
bool WaitEventAction::isDone() const
{
	return m_done;
}

// 0x44E2D0
WaitEventAction::~WaitEventAction()
{
}

// 0x44E380
WaitEventAction::WaitEventAction(const std::string& preAction, const std::string& waitAction,
	const std::string& delayAction, float delay)
{
	m_preAction = preAction;
	m_waitAction = waitAction;
	m_delayAction = delayAction;
	m_delay = delay;
}

// 0x44E480
void WaitEventAction::start()
{
	m_sequence->doAction(m_preAction, "");
	m_done = false;
}

// 0x44E500
void WaitEventAction::update(engine::UpdateContext& info)
{
	if (m_delay > 0.0f)
	{
		m_delay -= info.elapsed;
		if (m_delay <= 0.0f)
			m_sequence->doAction(m_delayAction, "");
	}
}

// 0x44E5B0
void WaitEventAction::onEvent(const std::string& event)
{
	if (event == m_waitAction)
	{
		m_done = true;
		PizzaFrenzy::getSounds()->playSound("order_click1", 1.0f, 1.0f);
	}
}

// 0x44E660
WaitEventHandler::WaitEventHandler(const std::string& name, engine::XmlHandlerStack* reader,
	const engine::Properties& attrs, StorySequence* sequence)
	: engine::XmlElementHandler(name, reader)
{
	WaitEventAction* action = new WaitEventAction();
	std::string preAction = attrs.getString("preAction", "");
	std::string waitAction = attrs.getString("waitAction", "");
	std::string delayAction = attrs.getString("delayAction", "");
	float delay = attrs.getFloat("delay", 0.0f);
	action->m_preAction = preAction;
	action->m_waitAction = waitAction;
	action->m_delayAction = delayAction;
	action->m_delay = delay;
	sequence->addAction(action);
}
