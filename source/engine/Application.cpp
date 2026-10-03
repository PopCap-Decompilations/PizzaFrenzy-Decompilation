#include "Application.h"

#include <map>
#include <string>
#include <utility>

#include "Component.h"
#include "FileSystem.h"
#include "Font.h"
#include "InputStreamReader.h"
#include "SimpleSound.h"
#include "SoundStreamAdapter.h"
#include "Surface.h"
#include "Xml.h"

namespace engine
{
	char g_textBuffer[1024];
	Application* g_application;
	SimpleSound* g_soundSystem;

	// 0x461740
	Application* getApplication()
	{
		return g_application;
	}

	// 0x461750
	char* getTextBuffer()
	{
		return g_textBuffer;
	}

	// 0x461760
	int getTextBufferSize()
	{
		return 1024;
	}

	// 0x461770
	SimpleSound* getSoundSystem()
	{
		return g_soundSystem;
	}

	// 0x461780
	void Application::setSoundSystem(SimpleSound* soundSystem)
	{
		g_soundSystem = soundSystem;
	}

	// 0x4617F0
	bool Application::addArchive(const char* path)
	{
		return m_fileSystem->addArchive(path);
	}

	// 0x461800
	void Application::setReadFromDisk(int readFromDisk)
	{
		m_fileSystem->setUseDisk(readFromDisk);
	}

	// 0x461810
	bool Application::fileExists(const char* path)
	{
		return m_fileSystem->exists(path);
	}

	// 0x461820
	bool Application::diskFileExists(const char* path)
	{
		return m_fileSystem->diskFileExists(path);
	}

	// 0x461830
	SeekableInputStream* Application::getInputStream(const char* path)
	{
		return m_fileSystem->getInputStream(path);
	}

	// 0x461840
	Reader* Application::getReader(const char* path)
	{
		return m_fileSystem->getReader(path);
	}

	// 0x461850
	SeekableInputStream* Application::getDiskInputStream(const char* path)
	{
		return m_fileSystem->openDiskFile(path);
	}

	// 0x461860
	Reader* Application::getDiskReader(const char* path)
	{
		return m_fileSystem->openDiskReader(path);
	}

	// 0x461870
	void Application::loadXml(const std::string& path, XmlHandler* handler)
	{
		XmlParseScope scope;
		RefPtr<Reader> reader = getReader(path.c_str());
		parseXml(handler, reader);
	}

	// 0x461960
	void Application::loadDiskXml(const std::string& path, XmlHandler* handler)
	{
		XmlParseScope scope;
		RefPtr<Reader> reader = getDiskReader(path.c_str());
		parseXml(handler, reader);
	}

	// 0x461A50
	bool Application::loadSound(const std::string& name, const std::string& path)
	{
		if (g_soundSystem)
		{
			if (g_soundSystem->hasSound(name.c_str()))
				return true;
			SeekableInputStream* stream = getInputStream(path.c_str());
			return g_soundSystem->loadStaticSound(name.c_str(), new SoundStreamAdapter(stream));
		}
		return false;
	}

	// 0x461B70
	bool Application::loadSoundStream(const std::string& name, const std::string& path)
	{
		if (g_soundSystem)
		{
			if (g_soundSystem->hasSound(name.c_str()))
				return true;
			SeekableInputStream* stream = getInputStream(path.c_str());
			return g_soundSystem->loadStreamingSound(name.c_str(), new SoundStreamAdapter(stream));
		}
		return false;
	}

	// 0x461C90
	SoundHandle* Application::createSound(const std::string& name, bool softwareBuffer, int category)
	{
		if (g_soundSystem)
			return g_soundSystem->createHandle(name.c_str(), softwareBuffer, category);
		return 0;
	}

	// 0x461DD0
	void Application::dumpImages()
	{
		log("name\twidth\theight\t#components\tbytes\n");
		for (std::map<std::string, Bitmap*>::iterator it = m_images.m_resources.begin(); it != m_images.m_resources.end(); ++it)
		{
			int components = it->second->getAlphaType() ? 4 : 3;
			int height = it->second->getHeight();
			int width = it->second->getWidth();
			log("%s\t%d\t%d\t%d\t%d\n", it->first.c_str(), width, height, components, width * height * components);
		}
	}

	// 0x461F40
	Component* Application::findComponent(std::string name)
	{
		std::multimap<std::string, Component*>::iterator it = m_components.find(name);
		if (it == m_components.end())
			return 0;
		return it->second;
	}

	// 0x462380 (ResourceCache<Bitmap>; no other instance)
	template <class T>
	void ResourceCache<T>::purgeUnused()
	{
		typename std::map<std::string, T*>::iterator it = m_resources.begin();
		while (it != m_resources.end())
		{
			T* resource = it->second;
			if (resource->getRefCount() == 1)
			{
				resource->release();
				it = m_resources.erase(it);
			}
			else
				++it;
		}
	}

	// 0x462480
	Bitmap* Application::findImage(const char* name)
	{
		return m_images.find(name);
	}

	// 0x462530
	void Application::purgeImages()
	{
		m_images.purgeUnused();
	}

	// 0x4627E0 (ResourceCache<Bitmap>)
	// 0x462840 (ResourceCache<Font>)
	template <class T>
	void ResourceCache<T>::releaseAll()
	{
		for (typename std::map<std::string, T*>::iterator it = m_resources.begin(); it != m_resources.end(); ++it)
			it->second->release();
		m_resources.clear();
	}

	// 0x4628B0
	void Application::unregisterComponent(Component* component)
	{
		std::multimap<std::string, Component*>::iterator it = m_components.lower_bound(component->getName());
		std::multimap<std::string, Component*>::iterator last = m_components.upper_bound(component->getName());
		if (it != m_components.end())
		{
			for (; it != last; ++it)
			{
				if (it->second == component)
				{
					m_components.erase(it);
					break;
				}
			}
		}
	}

	// 0x463310 (folded)
	template <class T>
	void ResourceCache<T>::add(const std::string& name, T* resource)
	{
		resource->addRef();
		m_resources.insert(typename std::map<std::string, T*>::value_type(name, resource));
	}

	// 0x4633B0
	Bitmap* Application::getImage(const char* path)
	{
		std::string name = path;
		Bitmap* image = findImage(path);
		if (!image)
		{
			image = createImage(path);
			m_images.add(name, image);
		}
		return image;
	}

	// 0x463470
	Bitmap* Application::getDiskImage(const char* path)
	{
		std::string name = path;
		Bitmap* image = findImage(path);
		if (!image)
		{
			image = createDiskImage(path);
			m_images.add(name, image);
		}
		return image;
	}

	// 0x463530
	bool Application::addImage(Bitmap* image, const char* name)
	{
		if (m_images.find(name))
			return false;
		m_images.add(name, image);
		return true;
	}

	// 0x463650
	Font* Application::getFont(const char* path)
	{
		std::string name = path;
		Font* font = m_fonts.find(name);
		if (!font)
		{
			font = createFont(path);
			m_fonts.add(name, font);
		}
		return font;
	}

	// 0x463720
	Font* Application::getDiskFont(const char* path)
	{
		std::string name = path;
		Font* font = m_fonts.find(name);
		if (!font)
		{
			font = createDiskFont(path);
			m_fonts.add(name, font);
		}
		return font;
	}

	// 0x463880: the entry is default-constructed (second = 0), then assigned
	void Application::registerComponent(Component* component)
	{
		std::pair<std::string, Component*> entry;
		entry.first = component->getName();
		entry.second = component;
		m_components.insert(entry);
	}

	// 0x463970 (ResourceCache<Bitmap>)
	// 0x4639E0 (ResourceCache<Font>)
	template <class T>
	ResourceCache<T>::~ResourceCache()
	{
		releaseAll();
	}

	// 0x463A50
	Application::~Application()
	{
	}

	// 0x463C10
	Application::Application()
	{
		m_fileSystem = new FileSystem();
	}
}
