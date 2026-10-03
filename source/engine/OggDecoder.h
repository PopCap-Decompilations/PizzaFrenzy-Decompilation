// engine::SoundDecoder (the sound system's abstract decoder) and engine::OggDecoder (Ogg Vorbis through vorbisfile).
#pragma once

#include <stddef.h>

#include <vorbis/vorbisfile.h>

namespace engine
{
	class SoundStream;

	// Abstract decoder with no destructor at all: WaveBufferSource and FileStream close their decoder and delete it
	// through this class, which is a plain operator delete; SimpleSoundDX::createDecoder deletes a failed one
	// through OggDecoder's virtual destructor. Its own vtable never survives (the store in OggDecoder's inlined
	// base constructor is dead). Layout: vfptr +0x00 (4 bytes).
	// The original also declares a pure virtual destructor (slot 9, _purecall here, the scalar deleting destructor
	// in OggDecoder's table), yet every decoder is deleted as an incomplete type: close(), then a plain operator
	// delete that runs no destructor. Leaving the destructor out keeps those deletes (FileStream, WaveBufferSource,
	// SimpleSoundDX) as they are; OggDecoder introduces it at slot 9, so its table is the original's.
	class SoundDecoder
	{
	public:
		virtual bool open(SoundStream* stream) = 0;							// slot 0
		virtual bool close() = 0;											// slot 1
		virtual unsigned int read(void* buffer, unsigned int bytes) = 0;	// slot 2: returns the bytes decoded
		virtual int getSampleRate() = 0;									// slot 3
		virtual int getBitsPerSample() = 0;									// slot 4
		virtual int getRawSize() = 0;										// slot 5: compressed bytes
		virtual int getDecodedSize() = 0;									// slot 6: PCM bytes
		virtual int getChannels() = 0;										// slot 7
		virtual int rewind() = 0;											// slot 8
	};

	// Decodes to 16-bit signed little-endian PCM (ov_read) from a SoundStream through the ov_callbacks below.
	// Created by SimpleSoundDX::createDecoder. Layout: vfptr +0x00, members +0x08 (OggVorbis_File is 8-byte
	// aligned, so +0x04 is padding) (0x2E8 bytes).
	class OggDecoder : public SoundDecoder
	{
	public:
		// m_file.datasource = 0
		OggDecoder();
		// slot 9: close() (inlined in the scalar deleting destructor 0x4976E0, the only copy)
		virtual ~OggDecoder();

		// ov_callbacks.read_func: stream->read(ptr, count) (size is ignored)
		static size_t readCallback(void* ptr, size_t size, size_t count, void* datasource);
		// ov_callbacks.seek_func: stream->seek((long)offset, whence)
		static int seekCallback(void* datasource, ogg_int64_t offset, int whence);
		// ov_callbacks.close_func
		static int closeCallback(void* datasource);
		// ov_callbacks.tell_func
		static long tellCallback(void* datasource);

		// SoundDecoder
		// close(), ov_open_callbacks, then the stream info; stream->addRef() on success
		virtual bool open(SoundStream* stream);
		// ov_clear and stream->release() if open; always true
		virtual bool close();
		// ov_read loop (little-endian, 16-bit, signed)
		virtual unsigned int read(void* buffer, unsigned int bytes);
		virtual int getSampleRate();
		// 16
		virtual int getBitsPerSample();
		virtual int getRawSize();
		virtual int getDecodedSize();
		virtual int getChannels();
		// ov_raw_seek(0)
		virtual int rewind();

		OggVorbis_File m_file;					// +0x008 vorbisfile state; datasource is the SoundStream
		int m_rawSize;							// +0x2D8 ov_raw_total(-1)
		int m_decodedSize;						// +0x2DC 2 * ov_pcm_total(-1) * channels
		int m_sampleRate;						// +0x2E0
		int m_channels;							// +0x2E4
	};
}
