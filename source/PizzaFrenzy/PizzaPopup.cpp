// PizzaPopup, GameStateMachine, PopupShowState and PopupCloseState.
#include "PizzaPopup.h"

#include "engine/Application.h"
#include "engine/Image.h"
#include "engine/SoundMgr.h"
#include "BuildingTile.h"
#include "Constants.h"
#include "PizzaFrenzy.h"

// ---- PopupShowState ---------------------------------------------------------------------------------------------

// 0x451620
bool PopupShowState::acceptsInput() const
{
	return m_acceptsInput;
}

// ---- PizzaPopup -------------------------------------------------------------------------------------------------

// 0x454C40
void PizzaPopup::setEnabled(bool enabled)
{
	m_enabled = enabled;
}

// 0x454C50
void PizzaPopup::updateState(engine::UpdateContext& ctx)
{
	m_state.update(ctx);
}

// 0x454C60
bool PizzaPopup::acceptsInput() const
{
	PizzaPopupState* state = static_cast<PizzaPopupState*>(m_state.getState());
	return state && state->acceptsInput();
}

// 0x454C90
bool PizzaPopup::isClosing() const
{
	return m_closing;
}

// 0x454CA0
bool PizzaPopup::isActive() const
{
	PizzaPopupState* state = static_cast<PizzaPopupState*>(m_state.getState());
	return state && state->isActive();
}

// 0x454CD0
void PizzaPopup::onMouseDown()
{
	if (acceptsInput())
		m_owner->onMouseDown();
}

// 0x454CF0
void PizzaPopup::onMouseEnter()
{
	if (acceptsInput())
		highlight();
}

// 0x454D10
void PizzaPopup::onMouseLeave()
{
	if (acceptsInput())
		deselect();
}

// 0x454D30
void PizzaPopup::select()
{
	setAlpha(1.0f);
	setScale(1.0f);
}

// 0x454D50 (folded)
void PizzaPopup::onDispatched()
{
	showSelection(false);
}

// 0x454D50 (folded)
void PizzaPopup::deselect()
{
	showSelection(false);
}

// 0x454D60
void PizzaPopup::onRemovedFrom(engine::Container* parent)
{
	m_owner->clearPopup();
	engine::Container::onRemovedFrom(parent);
}

// ---- PopupShowState ---------------------------------------------------------------------------------------------

// 0x454D90
void PopupShowState::enter()
{
	m_time = g_popupShowTime;
}

// 0x454DA0
void PopupShowState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	if (m_time < 0.0f)
		m_time = 0.0f;
	float t = m_time / g_popupShowTime;
	m_owner->setAlpha(1.0f - t);
	m_owner->setScale(1.0f - t * 0.5f);
	if (m_time <= 0.0f)
		m_owner->onShowFinished();
}

// ---- PopupCloseState --------------------------------------------------------------------------------------------

// 0x454E30
void PopupCloseState::enter()
{
	m_time = g_popupCloseTime;
	m_owner->m_closing = true;
	m_owner->showSelection(false);
}

// 0x454E50
void PopupCloseState::exit()
{
	m_owner->m_closing = false;
}

// 0x454E70
void PopupCloseState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	m_time = m_time < 0.0f ? 0.0f : m_time;
	float t = m_time / g_popupCloseTime;
	m_owner->setAlpha(t);
	m_owner->setScale(t);
	if (m_time <= 0.0f)
		m_owner->onCloseFinished();
}

// 0x4529D0 (folded)
bool PopupCloseState::acceptsInput() const
{
	return false;
}

// 0x4529D0 (folded)
bool PopupCloseState::isActive() const
{
	return false;
}

// ---- GameStateMachine -------------------------------------------------------------------------------------------

// 0x454E60 (folded)
bool GameStateMachine::setState(engine::StateBase* state)
{
	return false;
}

// 0x454EE0
bool GameStateMachine::switchState(engine::StateBase* state)
{
	state->addRef();
	if (m_state)
	{
		m_state->exit();
		m_state.reset(0);
	}
	m_state.reset(state);
	if (m_state)
		m_state->enter();
	state->release();
	return true;
}

// 0x411960 (folded)
engine::StateBase* GameStateMachine::getState() const
{
	return m_state;
}

// ---- PizzaPopup -------------------------------------------------------------------------------------------------

// 0x454F40
PizzaPopup::~PizzaPopup()
{
	m_selectionBox = 0;
}

// 0x455000
void PizzaPopup::init(BuildingTile* owner)
{
	m_owner = owner;
	m_enabled = true;
	setBlendMode(1);
	engine::Bitmap* image = engine::getApplication()->getImage("res\\pizza\\selection-box.jpg");
	image->setPivotType(1);
	m_selectionBox = new engine::Image(image);
	m_selectionBox->setPosition(0.0f, 0.0f);
	m_selectionBox->setVisible(false);
	addChild(m_selectionBox);
	addTreeFlags(4);
	addTreeFlags(8);
}

// 0x455110
void PizzaPopup::updateBounds()
{
	m_bounds = m_rect;
	m_selectionBox->updateBounds();
	m_bounds.unite(m_selectionBox->getBounds());
	m_bounds.scale(m_scale.x, m_scale.y);
	m_bounds.normalize();
	m_bounds.offset(m_position.x, m_position.y);
	removeTreeFlags(8);
}

// 0x455190
void PizzaPopup::showSelection(bool show)
{
	m_selectionBox->setVisible(show);
}

// 0x4551C0
void PizzaPopup::close()
{
	m_state.switchState(new PopupCloseState(this));
}

// 0x455260
void PizzaPopup::highlight()
{
	showSelection(true);
	setAlpha(1.0f);
	setScale(1.0f);
	PizzaFrenzy::getSounds()->playSound("order_mouseover1", 1.0f, 1.0f);
}

// 0x455310
std::string PizzaPopup::getTypeName() const
{
	return "PizzaPopup";
}

// 0x455340
PizzaPopup::PizzaPopup()
	: m_owner(0), m_closing(false)
{
}

// 0x42F770 (folded)
void PizzaPopup::remove()
{
	setFlags(0x10);
}
