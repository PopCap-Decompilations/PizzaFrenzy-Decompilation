#include "PopupResetState.h"

// 0x451310
void PopupResetState::enter()
{
	m_owner->setAlpha(1.0f);
	m_owner->setScale(1.0f);
}
