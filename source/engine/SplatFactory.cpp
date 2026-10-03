#include "SplatFactory.h"

#include <map>
#include <string>
#include <vector>

#include "Application.h"
#include "Component.h"
#include "Image.h"
#include "Oscillator.h"
#include "Point.h"
#include "Rect.h"
#include "Splat.h"
#include "SplatManifestHandler.h"
#include "Surface.h"

namespace engine
{
	// 0x4739F0: false when parsing the manifest throws
	bool SplatFactory::load(const std::string& fileName)
	{
		SplatManifestHandler handler;
		handler.setFactory(this);
		try
		{
			getApplication()->loadXml(fileName, &handler);
		}
		catch (...)
		{
			// 0x473A66 (load$catch): the code after the catch block, which destroys the handler and returns false
			return false;
		}
		return true;
		// 0x473A77 (load$epilog): the epilog both returns share
	}

	// 0x473B90: the splats created since the last update become children; children whose time ran out are finished
	// and removed, the others updated (Container::update is not called)
	void SplatFactory::update(UpdateContext& context)
	{
		for (std::vector<Splat*>::iterator it = m_newSplats.begin(); it != m_newSplats.end(); it = m_newSplats.erase(it))
			addChild(*it);
		clearFlags(4);
		for (std::vector<Component*>::iterator child = m_children.begin(); child != m_children.end(); )
		{
			Splat* splat = static_cast<Splat*>(*child);
			if (splat->m_timeLeft < 0.0f)
			{
				splat->fireFinished();
				splat->onRemovedFrom(this);
				child = m_children.erase(child);
			}
			else
			{
				splat->update(context);
				++child;
			}
		}
	}

	// 0x474670: the splat becomes a child at the next update
	void SplatFactory::addSplat(Splat* splat)
	{
		m_newSplats.push_back(splat);
		setFlags(4);
	}

	// 0x4748C0
	SplatFactory::~SplatFactory()
	{
		for (std::map<std::string, SplatDef*>::iterator it = m_splatDefs.begin(); it != m_splatDefs.end(); ++it)
			delete it->second;
		m_splatDefs.clear();
	}

	// 0x4749F0
	std::string SplatFactory::getTypeName() const
	{
		return "SplatFactory";
	}

	// 0x474A20: 0 for an unknown type. The splat gets random values from the definition's ranges, its image or text,
	// its motion and life time; it starts at (x, y) moved inside the screen horizontally (by half its width) and
	// becomes a child at the next update. Without a parent the parent's screen position is left uninitialised.
	Splat* SplatFactory::createSplat(const std::string& type, float x, float y, void (*onFinished)(void*), void* userData)
	{
		std::map<std::string, SplatDef*>::iterator it = m_splatDefs.find(type);
		if (it == m_splatDefs.end())
			return 0;
		SplatDef* def = it->second;
		Splat* splat = new Splat();
		splat->setName(type);
		splat->m_drag = def->m_acceleration.random();
		// the X velocity is drawn before the Y velocity
		float velocityX = def->m_velocityX.random();
		float velocityY = def->m_velocityY.random();
		splat->m_velocity = Vector2(velocityX, velocityY);
		splat->m_velocity.rotate(def->m_rotation.random());
		splat->m_startScale = def->m_startScale.random();
		splat->m_endScale = def->m_endScale.random();
		splat->m_scaleStart = def->m_startAlpha;
		splat->m_scaleEnd = def->m_endAlpha;
		splat->m_startAlpha = def->m_startOpacity;
		splat->m_endAlpha = def->m_endOpacity;
		splat->m_acceleration = def->m_extForce;
		if (def->m_destination.x != 0.0f && def->m_destination.y != 0.0f)
			splat->moveTo(def->m_destination.x, def->m_destination.y);
		splat->setCallback(onFinished, userData);
		if (!def->m_imagePath.empty())
		{
			Bitmap* image = getApplication()->getImage(def->m_imagePath.c_str());
			image->setPivotType(def->m_pivot);
			splat->addChild(new Image(image));
			splat->updateBounds();
		}
		if (!def->m_fontPath.empty())
		{
			splat->m_fontName = def->m_fontPath;
			splat->m_textColor = def->m_textColor;
			splat->setShadow(def->m_textShadow, def->m_textShadowOpacity, def->m_textShadowOffset);
			splat->setText(def->m_text);
		}
		float halfWidth = splat->getBounds().getWidth() * 0.5f;
		Vector2 origin;
		if (m_parent)
			origin = m_parent->getScreenPosition();
		if (origin.x + x - halfWidth < 0.0f)
			x = halfWidth;
		else if (getApplication()->getWidth() < origin.x + halfWidth + x)
			x = getApplication()->getWidth() - halfWidth - origin.x;
		splat->setStartPosition(x, y);
		splat->setScale(splat->m_startScale, splat->m_startScale);
		splat->setAlpha(splat->m_startAlpha);
		float jitterPeriodX = def->m_jitterPeriodX.random();
		float jitterPeriodY = def->m_jitterPeriodY.random();
		if (jitterPeriodX != 0.0f || jitterPeriodY != 0.0f)
		{
			splat->removeAllAnimators();
			// VS2003 evaluated the constructor's arguments right to left: the Y amplitude is drawn first (the original
			// draws both after operator new, which draws nothing)
			float jitterAmplitudeY = def->m_jitterAmplitudeY.random();
			float jitterAmplitudeX = def->m_jitterAmplitudeX.random();
			splat->addAnimator(new Oscillator(jitterPeriodX, jitterPeriodY, jitterAmplitudeX, jitterAmplitudeY, 0.0f));
		}
		splat->updateBounds();
		splat->m_lifeTime = splat->m_timeLeft = def->m_lifeTime.random();
		addSplat(splat);
		return splat;
	}

	// 0x474F40
	SplatFactory::SplatFactory()
	{
	}

	// 0x475020: called by SplatManifestHandler at each </splat>; a type defined twice keeps the last definition (the
	// first is not deleted)
	void SplatFactory::addSplatDef(const std::string& type, SplatDef* def)
	{
		m_splatDefs[type] = def;
	}

	// 0x473AA0 (folded)
	void SplatFactory::updateBounds()
	{
		m_bounds.clear();
		removeTreeFlags(8);
	}
}
