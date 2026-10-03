#include "OrderHighlightState.h"

#include "engine/Component.h"

// 0x452060 (folded)
void OrderHighlightState::update(engine::UpdateContext& context)
{
	m_orderPopup->m_patience -= context.elapsed;
	m_orderPopup->m_waitTime += context.elapsed;
}
