// engine::Image: a component that draws an image resource, optionally only a source rectangle of it.
#pragma once

#include <string>

#include "Component.h"
#include "Rect.h"
#include "RefPtr.h"
#include "Surface.h"

namespace engine
{
	class Graphics;

	// Component +0x00, members from +0x108, then the vtordisp (+0x120) and the Interface subobject (+0x124): 0x128
	// bytes. The destructor (0x433140, emitted in another object) is implicit: it stores no vtables (so Bitmap is
	// included: every file that destroys an Image by value instantiates it).
	class Image : public Component
	{
	public:
		Image();
		Image(Bitmap* image);

		virtual std::string getTypeName() const;	// slot 24 (engine::Component): "Image"
		virtual void draw(Graphics& g);				// slot 36 (engine::Component)
		virtual void updateBounds();				// slot 38 (engine::Component): position - pivot, image size, |scale|

		void setUseSourceRect(bool use);
		void setSourceRect(const IntRect& r);
		void setImage(Bitmap* image);
		int getImageWidth() const;					// 0 without an image
		int getImageHeight() const;
		Bitmap* getImage() const;

		IntRect m_sourceRect;					// +0x108 part of the image to draw (pixels)
		bool m_useSourceRect;					// +0x118 draw only m_sourceRect
		RefPtr<Bitmap> m_image;					// +0x11C image resource (Application::getImage)
	};
}
