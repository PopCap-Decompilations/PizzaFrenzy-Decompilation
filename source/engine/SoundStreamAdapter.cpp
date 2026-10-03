#include "SoundStreamAdapter.h"

#include "InputStream.h"

namespace engine
{
	// 0x484820
	void SoundStreamAdapter::addRef()
	{
		Object::addRef();
	}

	// 0x484830
	void SoundStreamAdapter::release()
	{
		Object::release();
	}

	// 0x484840
	SoundStreamAdapter::SoundStreamAdapter(SeekableInputStream* stream)
		: m_stream(stream)
	{
	}

	// 0x4849A0
	unsigned int SoundStreamAdapter::read(void* buffer, unsigned int bytes)
	{
		return m_stream->read(buffer, bytes);
	}

	// 0x4849B0
	int SoundStreamAdapter::seek(long offset, int origin)
	{
		return m_stream->seek(offset, origin) ? 0 : -1;
	}

	// 0x4849D0
	long SoundStreamAdapter::tell()
	{
		return m_stream->tell();
	}

	// 0x4849E0
	int SoundStreamAdapter::close()
	{
		return m_stream->close() ? 0 : -1;
	}
}
