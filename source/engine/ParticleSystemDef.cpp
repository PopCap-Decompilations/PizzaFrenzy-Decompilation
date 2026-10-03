#include "ParticleSystemDef.h"

#include <cstdio>

#include "Application.h"
#include "Surface.h"

namespace engine
{
	static char s_particleFrameName[256];		// 0x533088 frame file name built by ParticleStreamDef::loadFrames ("%s%d%s")

	// 0x47D870
	ParticleSystemDef::~ParticleSystemDef()
	{
		m_streams.clear();
	}

	// 0x47D960
	ParticleStreamDef::~ParticleStreamDef()
	{
		m_frames.clear();
	}

	// 0x47DDB0
	ParticleSystemDef::ParticleSystemDef()
	{
		// assigned after m_streams is built, warmup first (0x47DE17, 0x47DE1A)
		m_warmup = 0.0f;
		m_autoStop = -1.0f;
	}

	// 0x47DE40
	ParticleStreamDef::ParticleStreamDef()
	{
		m_emitRate = 0.0f;
		m_maxParticles = 0;
		m_velocityX = Range(0.0f, 0.0f);
		m_velocityY = Range(0.0f, 0.0f);
		m_acceleration = Range(0.0f, 0.0f);
		m_positionX = Range(0.0f, 0.0f);
		m_positionY = Range(0.0f, 0.0f);
		m_startScale = Range(1.0f, 1.0f);
		m_endScale = Range(1.0f, 1.0f);
		m_rotation = Range(0.0f, 0.0f);
		m_jitterXPeriod = Range(1.0f, 1.0f);
		m_jitterXAmplitude = Range(0.0f, 0.0f);
		m_jitterYPeriod = Range(1.0f, 1.0f);
		m_jitterYAmplitude = Range(0.0f, 0.0f);
		m_constantScale = false;
		m_constantOpacity = true;
		m_blackHole = false;
		m_blackHoleAccel = 0.0f;
		m_blackHolePos = Vector2(0.0f, 0.0f);
		m_lifeTime = Range(1.0f, 1.0f);
		m_startOpacity = Range(1.0f, 1.0f);
		m_endOpacity = Range(1.0f, 1.0f);
		m_extForce = Vector2(0.0f, 0.0f);
		m_filter = 0;
		m_offsetType = 0;
		m_startDelay = 0.0f;
		m_autoStop = -1.0f;
	}

	// 0x47E220
	void ParticleSystemDef::addStream(ParticleStreamDef* stream)
	{
		m_streams.push_back(stream);
	}

	// 0x47E2A0
	bool ParticleStreamDef::loadFrames()
	{
		m_frames.clear();
		int index = 0;
		Application* application = getApplication();
		Bitmap* bitmap;
		do
		{
			bitmap = 0;
			sprintf(s_particleFrameName, "%s%d%s", m_resourcePath.c_str(), index, ".png");
			if (!application->fileExists(s_particleFrameName))
				sprintf(s_particleFrameName, "%s%d%s", m_resourcePath.c_str(), index, ".jpg");
			if (application->fileExists(s_particleFrameName))
			{
				bitmap = application->getImage(s_particleFrameName);
				bitmap->setPivotType(1);
				m_frames.push_back(bitmap);
			}
			index++;
		}
		while (bitmap != 0);
		return index > 0;
	}

	// 0x47E480
	bool ParticleSystemDef::loadFrames()
	{
		for (std::vector<RefPtr<ParticleStreamDef> >::iterator it = m_streams.begin(); it != m_streams.end(); ++it)
		{
			if (!(*it)->loadFrames())
				return false;
		}
		return true;
	}
}
