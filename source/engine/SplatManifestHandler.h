// engine::SplatDef (one splat definition) and engine::SplatManifestHandler (the SAX handler of res\manifests\splats.xml).
#pragma once

#include <string>

#include "Color.h"
#include "Point.h"
#include "Range.h"
#include "XmlHandlerBase.h"

namespace engine
{
	class Properties;
	class SplatFactory;

	// One <splat> of res\manifests\splats.xml: what SplatFactory::createSplat builds a splat from. No vtable;
	// created by SplatManifestHandler::startSplat (which sets the defaults), deleted by SplatFactory's destructor;
	// implicit destructor (0x473AC0). A struct, as SplatFactory.h declares it. 0xF8 bytes.
	struct SplatDef
	{
		// the members' constructors only
		SplatDef();

		Range m_velocityX;						// +0x00 <velocity startX>
		Range m_velocityY;						// +0x08 <velocity startY>
		Range m_acceleration;					// +0x10 <acceleration rate>
		Range m_startScale;						// +0x18 <scale startScale>
		Range m_endScale;						// +0x20 <scale endScale>
		float m_startAlpha;						// +0x28 <scale startAlpha>
		float m_endAlpha;						// +0x2C <scale endAlpha>
		Range m_unknown30;						// +0x30 set to (1, 1) by startSplat; never parsed or read
		Range m_rotation;						// +0x38 <rotation degrees>
		float m_startOpacity;					// +0x40 <opacity startOp>
		float m_endOpacity;						// +0x44 <opacity endOp>
		Vector2 m_extForce;						// +0x48 <extForce force>
		Range m_jitterPeriodX;					// +0x50 <jitter axis="X" period>
		Range m_jitterAmplitudeX;				// +0x58 <jitter axis="X" amplitude>
		Range m_jitterPeriodY;					// +0x60 <jitter axis="Y" period>
		Range m_jitterAmplitudeY;				// +0x68 <jitter axis="Y" amplitude>
		std::string m_imagePath;				// +0x70 <resource type="image" path>
		std::string m_fontPath;					// +0x8C <resource type="text" font>
		std::string m_text;						// +0xA8 <resource type="text" text>
		bool m_textShadow;						// +0xC4 the textStyle attribute contains "shadow"
		float m_textShadowOpacity;				// +0xC8 textShadowOpacity
		Vector2 m_textShadowOffset;				// +0xCC textShadowOffset
		Vector2 m_destination;					// +0xD4 <destination pos>; (0, 0) = none
		Range m_lifeTime;						// +0xDC <splat lifeTime>
		Color m_textColor;						// +0xE4 <resource color>
		int m_pivot;							// +0xF4 1 if pivot="center"
	};

	// <splat name lifeTime> starts a SplatDef (startSplat), its child elements fill it, and </splat> hands it to
	// SplatFactory::addSplatDef. Built on the stack by SplatFactory::load. Layout: XmlHandlerBase +0x00, members
	// +0x14, then the vtordisp and Interface (0x40 bytes).
	class SplatManifestHandler : public XmlHandlerBase
	{
	public:
		SplatManifestHandler();
		virtual ~SplatManifestHandler();

		// slot 1: a new SplatDef with the defaults, its lifeTime, and m_splatName (both arguments by value)
		virtual void startSplat(std::string name, Properties attributes);

		// XmlHandler
		// <splat> -> startSplat(copies); velocity, acceleration, extForce, scale, rotation, opacity, resource
		// (image or text), destination and jitter fill m_splat
		virtual void startElement(const std::string& name, const Properties& attributes);
		// </splat>: m_factory->addSplatDef(m_splatName, m_splat), m_splat = 0
		virtual void endElement(const std::string& name);

		void setFactory(SplatFactory* factory);

		SplatDef* m_splat;						// +0x14 the definition being built (0 outside <splat>)
		SplatFactory* m_factory;				// +0x18 set by setFactory
		std::string m_splatName;				// +0x1C
	};
}
