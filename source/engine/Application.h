// engine::Application: the abstract application/platform (input and frame signals, image and font caches, named
// components, the archive file system), the resource cache it keeps images and fonts in, and the application
// globals (getApplication, the shared text buffer, the sound system).
#pragma once

#include <map>
#include <string>

#include <windows.h>

#include "RefPtr.h"
#include "sigslot.h"

namespace engine
{
	class Bitmap;
	class Component;
	class FileSystem;
	class Font;
	class Graphics;
	class Reader;
	class RegistryKey;
	class Runnable;
	class SeekableInputStream;
	class SimpleSound;
	class SoundHandle;
	class Thread;
	class URLConnection;
	class XmlHandler;
	struct Point;
	struct UpdateContext;

	// Resources by name, each referenced (addRef) by the cache. Application::m_images and m_fonts; the functions are
	// in Application.cpp (only ResourceCache<Bitmap> has a purgeUnused).
	template <class T>
	class ResourceCache
	{
	public:
		~ResourceCache();								// releaseAll, then the map
		void add(const std::string& name, T* resource);	// resource->addRef(), then insert (0x463310, one body for both)
		void purgeUnused();								// releases and erases the resources only the cache references
		void releaseAll();								// releases every resource and clears the map

		// No out-of-line copy: inlined in findImage, addImage and getFont/getDiskFont (the string a const char*
		// name converts to lives until the result is taken).
		T* find(const std::string& name)
		{
			typename std::map<std::string, T*>::iterator it = m_resources.find(name);
			if (it == m_resources.end())
				return 0;
			return it->second;
		}

		std::map<std::string, T*> m_resources;			// +0x00
	};

	// Not an engine::Object (no vbptr, no reference count; Win32Application adds has_slots<> at +0x14C): vfptr +0x00,
	// members from +0x04, 0x14C bytes. The destructor is slot 52. Win32Application implements the pure slots.
	class Application
	{
	public:
		Application();

		virtual std::string getVersion() = 0;											// slot 0: FileVersion + SpecialBuild
		virtual bool createDisplay(bool fullScreen) = 0;								// slot 1
		virtual bool setFullScreen(bool fullScreen) = 0;								// slot 2
		virtual bool isFullScreen() = 0;												// slot 3
		virtual int getWidth() = 0;														// slot 4
		virtual int getHeight() = 0;													// slot 5
		virtual HWND getWindowHandle() = 0;												// slot 6
		virtual HINSTANCE getInstanceHandle() = 0;										// slot 7
		virtual bool addArchive(const char* path);										// slot 8: m_fileSystem
		virtual bool fileExists(const char* path);										// slot 9: archives or disk
		virtual bool diskFileExists(const char* path);									// slot 10: nativeFileExists
		virtual void setReadFromDisk(int readFromDisk);									// slot 11: loose files first
		virtual SeekableInputStream* getInputStream(const char* path);					// slot 12
		virtual Reader* getReader(const char* path);									// slot 13
		virtual SeekableInputStream* getDiskInputStream(const char* path);				// slot 14: openNativeFile
		virtual Reader* getDiskReader(const char* path);								// slot 15
		// The handler is a pointer: the game's callers convert their parser stack to it with a null check.
		virtual void loadXml(const std::string& path, XmlHandler* handler);				// slot 16: parses getReader(path)
		virtual void loadDiskXml(const std::string& path, XmlHandler* handler);			// slot 17: getDiskReader(path)
		virtual Bitmap* getImage(const char* path);										// slot 18: cached createImage
		virtual Bitmap* getDiskImage(const char* path);									// slot 19: cached createDiskImage
		virtual Font* getFont(const char* path);										// slot 20: cached createFont
		virtual Font* getDiskFont(const char* path);									// slot 21: cached createDiskFont
		virtual bool loadSound(const std::string& name, const std::string& path);		// slot 22: sound system slot 1
		virtual bool loadSoundStream(const std::string& name, const std::string& path);	// slot 23: sound system slot 2
		virtual SoundHandle* createSound(const std::string& name, bool softwareBuffer, int category);	// slot 24
		virtual Bitmap* createImage(const char* path) = 0;								// slot 25: .jpg/.png (+ _alpha.png)
		virtual Bitmap* createDiskImage(const char* path) = 0;							// slot 26
		virtual bool addImage(Bitmap* image, const char* name);							// slot 27: false if the name is taken
		virtual Bitmap* findImage(const char* name);									// slot 28
		virtual void dumpImages();														// slot 29: the image cache through log
		virtual Bitmap* createBlankImage(int width, int height) = 0;					// slot 30
		virtual Graphics* createGraphics(Bitmap* image) = 0;							// slot 31: draws into an image
		virtual bool nativeFileExists(const std::string& path) = 0;						// slot 32
		virtual SeekableInputStream* openNativeFile(const std::string& path) = 0;		// slot 33
		virtual URLConnection* openUrl(const std::string& url) = 0;						// slot 34
		virtual std::string loadString(unsigned int id) = 0;							// slot 35
		virtual std::string formatString(unsigned int id, ...) = 0;						// slot 36
		virtual RegistryKey* openRegistryKey(RegistryKey* parent, const std::string& name) = 0;	// slot 37
		virtual RegistryKey* openRootRegistryKey(int root, const std::string& path) = 0;	// slot 38
		virtual Thread* createThread(Runnable* runnable) = 0;							// slot 39
		virtual void log(const char* format, ...) = 0;									// slot 40
		virtual void logSystem(const char* format, ...) = 0;							// slot 41
		virtual std::string getErrorMessage(unsigned long errorCode) = 0;				// slot 42: FormatMessage
		virtual __int64 getTicks() = 0;													// slot 43
		virtual __int64 getTicksPerSecond() = 0;										// slot 44
		virtual float getFrameTime() = 0;												// slot 45
		virtual float getFrameRate() = 0;												// slot 46
		virtual int getCountPerSecond() = 0;											// slot 47
		virtual void quit(int exitCode) = 0;											// slot 48
		virtual void setSoundSystem(SimpleSound* soundSystem);							// slot 49: g_soundSystem
		virtual const Point& getMousePosition() = 0;									// slot 50
		virtual void invokeCallback(int a, int b) = 0;									// slot 51
		virtual ~Application();															// slot 52
		virtual Font* createFont(const char* path) = 0;									// slot 53
		virtual Font* createDiskFont(const char* path) = 0;								// slot 54

		Component* findComponent(std::string name);			// the first component registered under the name
		void purgeImages();									// m_images.purgeUnused()
		void unregisterComponent(Component* component);
		void registerComponent(Component* component);		// under component->getName()

		sigslot::signal1<UpdateContext&> m_updateSignal;			// +0x004 every frame
		sigslot::signal1<Graphics&> m_drawSignal;					// +0x014
		sigslot::signal1<bool> m_activateSignal;					// +0x024 WM_ACTIVATE
		sigslot::signal1<int> m_keyDownSignal;						// +0x034 WM_KEYDOWN virtual key
		sigslot::signal2<int, int> m_keyRepeatSignal;				// +0x044 key, repeat count
		sigslot::signal1<int> m_keyUpSignal;						// +0x054
		sigslot::signal1<char> m_charSignal;						// +0x064 WM_CHAR
		sigslot::signal1<const Point&> m_mouseDownSignal;			// +0x074 left button (and double-click)
		sigslot::signal1<const Point&> m_mouseUpSignal;				// +0x084
		sigslot::signal1<const Point&> m_rightMouseDownSignal;		// +0x094
		sigslot::signal1<const Point&> m_rightMouseUpSignal;		// +0x0A4
		sigslot::signal1<const Point&> m_mouseMoveSignal;			// +0x0B4
		sigslot::signal0<> m_closeSignal;							// +0x0C4 SC_CLOSE
		sigslot::signal0<> m_minimizeSignal;						// +0x0D4 SC_MINIMIZE
		sigslot::signal0<> m_maximizeSignal;						// +0x0E4 SC_MAXIMIZE
		sigslot::signal0<> m_restoreSignal;							// +0x0F4 SC_RESTORE
		sigslot::signal1<int> m_modalTimerSignal;					// +0x104 100 (ms) while the window is moved or sized
		// no emitter and no connection: the argument type is unknown, only that it differs from the other signals'
		// (its own ~_signal_base1 instance 0x462BD0)
		sigslot::signal1<unsigned int> m_unusedSignal;				// +0x114
		ResourceCache<Bitmap> m_images;								// +0x124
		ResourceCache<Font> m_fonts;								// +0x130
		std::multimap<std::string, Component*> m_components;		// +0x13C components by name
		RefPtr<FileSystem> m_fileSystem;							// +0x148 archives and loose files
	};

	Application* getApplication();			// g_application
	char* getTextBuffer();					// g_textBuffer
	int getTextBufferSize();				// 1024
	SimpleSound* getSoundSystem();			// g_soundSystem

	extern char g_textBuffer[1024];			// shared formatting buffer (log, format, LoadString)
	extern Application* g_application;		// the Win32Application: set by createApplication, cleared by WinMain
	extern SimpleSound* g_soundSystem;		// set by setSoundSystem
}
