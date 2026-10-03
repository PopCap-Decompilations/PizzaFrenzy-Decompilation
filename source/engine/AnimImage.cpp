#include "AnimImage.h"

#include <algorithm>
#include <cstdio>

#include "Application.h"
#include "Image.h"

namespace engine
{
	static char s_animFrameName[256];		// 0x533188 frame file name built by load ("%s%d%s")

	// 0x47B6D0 (folded)
	int AnimImage::getFrameCount() const
	{
		return m_children.size();
	}

	// 0x481BE0
	AnimImage::~AnimImage()
	{
	}

	// 0x481C10
	void AnimImage::setFrameRate(float framesPerSecond)
	{
		m_frameRate = framesPerSecond;
	}

	// 0x481C20
	float AnimImage::getFrameRate() const
	{
		return m_frameRate;
	}

	// 0x481C30
	void AnimImage::play()
	{
		m_playing = true;
	}

	// 0x481C40
	bool AnimImage::isPlaying() const
	{
		return m_playing;
	}

	// 0x481C50
	void AnimImage::setLooping(bool loop)
	{
		m_loop = loop;
	}

	// 0x481C60
	bool AnimImage::isLooping() const
	{
		return m_loop;
	}

	// 0x481C70
	void AnimImage::stop()
	{
		m_playing = false;
	}

	// 0x481C80
	void AnimImage::update(UpdateContext& context)
	{
		int lastFrame = m_lastFrame;
		int rangeLength = lastFrame - m_firstFrame + 1;
		if (getFrameCount() < 1)
		{
			Selector::update(context);
			return;
		}
		if (m_playing)
		{
			m_frame += m_frameRate * context.elapsed;
			int frame = (int)m_frame;
			if (frame > lastFrame)
			{
				if (m_loop)
				{
					while (m_frame > 0.0f && m_frame > lastFrame)
						m_frame -= rangeLength;
					frame = (int)m_frame;
				}
				else
				{
					frame = m_lastFrame;
					m_frame = (float)frame;
					m_playing = false;
				}
			}
			else if (frame < m_firstFrame)
			{
				if (m_loop)
				{
					while (m_frame > 0.0f && m_frame < m_firstFrame)
						m_frame += rangeLength;
					frame = (int)m_frame;
				}
				else
				{
					frame = m_firstFrame;
					m_frame = (float)frame;
					m_playing = false;
				}
			}
			select(frame, 0.0f, true);
		}
		Selector::update(context);
	}

	// 0x481E20
	int AnimImage::getCurrentFrame() const
	{
		return (int)m_frame;
	}

	// 0x481E30
	void AnimImage::resetFrameRange()
	{
		m_firstFrame = 0;
		m_lastFrame = getFrameCount() - 1;
	}

	// 0x481E50
	AnimImage::AnimImage()
		: m_frameRate(10.0f), m_frame(0.0f), m_playing(false), m_loop(true)
	{
		select(-1, 0.0f, true);
	}

	// 0x481F20
	void AnimImage::setCurrentFrame(int frame)
	{
		m_frame = (float)((unsigned int)frame < m_children.size() ? frame : m_children.size() - 1);
	}

	// 0x481F80
	void AnimImage::setFrameRange(int first, int last)
	{
		m_firstFrame = (std::max)(first, 0);
		m_firstFrame = (std::min)(m_firstFrame, getFrameCount() - 1);
		m_lastFrame = (std::max)(last, 0);
		m_lastFrame = (std::min)(m_lastFrame, getFrameCount() - 1);
	}

	// 0x482000
	void AnimImage::setHotspotMode(int mode)
	{
		for (std::vector<Component*>::iterator it = m_children.begin(); it != m_children.end(); ++it)
		{
			Image* image = static_cast<Image*>(*it);
			image->getImage()->setPivotType(mode);
			image->updateBounds();
		}
	}

	// 0x482050
	std::string AnimImage::getTypeName() const
	{
		return "AnimImage";
	}

	// 0x482080
	int AnimImage::load(const char* prefix, const char* extension, int firstIndex)
	{
		int index = firstIndex;
		Application* application = getApplication();
		std::string ext;
		if (extension == 0)
		{
			sprintf(s_animFrameName, "%s%d%s", prefix, firstIndex, ".jpg");
			if (application->fileExists(s_animFrameName))
				ext = ".jpg";
			else
				ext = ".png";
		}
		else
		{
			ext = extension;
		}
		for (;;)
		{
			sprintf(s_animFrameName, "%s%d%s", prefix, index++, ext.c_str());
			if (!application->fileExists(s_animFrameName))
				break;
			Bitmap* bitmap = application->getImage(s_animFrameName);
			if (bitmap == 0)
				break;
			Image* image = new Image(bitmap);
			image->updateBounds();
			addChild(image);
		}
		onFramesLoaded();
		return getFrameCount();
	}

	// 0x482210
	void AnimImage::onFramesLoaded()
	{
		if (!m_children.empty())
		{
			select(0, 0.0f, true);
			m_bounds = m_children.at(0)->getBounds();
		}
		setFlags(5);
		resetFrameRange();
	}
}
