// engine::ParticleSystemDef and engine::ParticleStreamDef: a parsed particle file (<particleSystem>, <particleStream>).
#pragma once

#include <string>
#include <vector>

#include "Object.h"
#include "Point.h"
#include "Range.h"
#include "RefPtr.h"

namespace engine
{
	class Bitmap;

	// A parsed <particleStream>: emitter parameters (filled by ParticleSystemLoader; the parser supplies the
	// attribute defaults) plus its loaded frame images. Object +0x00, members from +0x0C, then the vtordisp (+0x114)
	// and the Interface subobject (+0x118): 0x11C bytes. ParticleSystem::addStream copies it into a ParticleStream.
	class ParticleStreamDef : public Object
	{
	public:
		ParticleStreamDef();
		virtual ~ParticleStreamDef();

		bool loadFrames();

		Vector2 m_offset;						// +0x0C "offset" (x,y)
		float m_emitRate;						// +0x14 "emitRate"
		int m_maxParticles;						// +0x18 "maxParticles"
		int m_frameMode;						// +0x1C "sequence": 0 random, 1 "sequential", 2 "anim"
		float m_animRate;						// +0x20 "animRate"
		Range m_velocityX;						// +0x24 <velocity startX>
		Range m_velocityY;						// +0x2C <velocity startY>
		Range m_acceleration;					// +0x34 <acceleration rate>
		Range m_positionX;						// +0x3C <position startX>
		Range m_positionY;						// +0x44 <position startY>
		Range m_startScale;						// +0x4C <scale startScale>
		Range m_endScale;						// +0x54 <scale endScale>
		Range m_rotation;						// +0x5C <rotation degrees>
		Range m_jitterXPeriod;					// +0x64 <jitter axis="X" period>
		Range m_jitterXAmplitude;				// +0x6C <jitter axis="X" amplitude>
		Range m_jitterYPeriod;					// +0x74 <jitter axis="Y" period>
		Range m_jitterYAmplitude;				// +0x7C <jitter axis="Y" amplitude>
		bool m_constantScale;					// +0x84 <scale constant="true">
		bool m_constantOpacity;					// +0x85 <opacity constant="true">
		bool m_blackHole;						// +0x86 <blackHole> present
		float m_blackHoleAccel;					// +0x88 <blackHole accel>
		Vector2 m_blackHolePos;					// +0x8C <blackHole pos>
		Range m_lifeTime;						// +0x94 <particleLife lifeTime>
		Range m_startOpacity;					// +0x9C <opacity startOp>
		Range m_endOpacity;						// +0xA4 <opacity endOp>
		Vector2 m_extForce;						// +0xAC <extForce force>
		unsigned char m_unusedB4[8];			// +0xB4 never constructed, read or written by the engine
		std::string m_resourcePath;				// +0xBC <resource path>: frames are <path><n>.png or .jpg
		std::string m_parent;					// +0xD8 "parent" stream name
		int m_filter;							// +0xF4 <resource filter> "X", "Y" or "XY" -> 1, else 0
		int m_offsetType;						// +0xF8 "offsetType": 0 "independent", 1 "groupRelative"
		float m_autoStop;						// +0xFC "autoStop" (-1)
		float m_startDelay;						// +0x100 "startDelay"
		std::vector<RefPtr<Bitmap> > m_frames;	// +0x104 loaded frame images (hotspot centred)
	};

	// A parsed <particleSystem autoStop warmup>: its stream definitions. Object +0x00, members from +0x0C, then the
	// vtordisp (+0x24) and the Interface subobject (+0x28): 0x2C bytes.
	class ParticleSystemDef : public Object
	{
	public:
		ParticleSystemDef();
		virtual ~ParticleSystemDef();

		void addStream(ParticleStreamDef* stream);
		bool loadFrames();

		float m_autoStop;						// +0x0C "autoStop" (-1: runs until stopped)
		float m_warmup;							// +0x10 "warmup" (0)
		std::vector<RefPtr<ParticleStreamDef> > m_streams;	// +0x14
	};
}
