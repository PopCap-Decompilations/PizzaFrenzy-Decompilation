// engine::AnimImage: a frame animation whose frames are Image children loaded from numbered files.
#pragma once

#include <string>

#include "Selector.h"

namespace engine
{
	// Frames are loaded from "%s%d%s" (prefix, index, .jpg or .png) and played at a frame rate over a frame range.
	// Selector +0x00, members from +0x13C, then the vtordisp (+0x150) and the Interface subobject (+0x154): 0x158
	// bytes.
	class AnimImage : public Selector
	{
	public:
		AnimImage();
		virtual ~AnimImage();

		virtual std::string getTypeName() const;								// slot 24 (engine::Component)
		virtual void update(UpdateContext& context);							// slot 37 (engine::Component)

		virtual int load(const char* prefix, const char* extension, int firstIndex);	// slot 80: returns the frame count
		virtual void onFramesLoaded();											// slot 81
		virtual void setHotspotMode(int mode);									// slot 82
		virtual int getFrameCount() const;										// slot 83: child count, folded 0x47B6D0
		virtual void setFrameRate(float framesPerSecond);						// slot 84
		virtual float getFrameRate() const;										// slot 85
		virtual int getCurrentFrame() const;									// slot 86
		virtual void play();													// slot 87
		virtual bool isPlaying() const;											// slot 88
		virtual void setLooping(bool loop);										// slot 89
		virtual bool isLooping() const;											// slot 90
		virtual void stop();													// slot 91
		virtual void setCurrentFrame(int frame);								// slot 92: clamped to count - 1
		virtual void setFrameRange(int first, int last);						// slot 93: clamped to [0, count - 1]
		virtual void resetFrameRange();											// slot 94: 0..count - 1

		float m_frameRate;						// +0x13C 10
		float m_frame;							// +0x140 fractional frame position
		bool m_playing;							// +0x144
		bool m_loop;							// +0x145 true
		int m_firstFrame;						// +0x148
		int m_lastFrame;						// +0x14C
	};
}
