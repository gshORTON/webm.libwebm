// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include "webmmuxstreamaudio.hpp"
#include "webmmuxcontext.hpp"

class CMediaTypes;

namespace VorbisTypes
{
struct VORBISFORMAT2;
}

namespace webmmux
{

class StreamAudioVorbis : public StreamAudio
{
    StreamAudioVorbis(const StreamAudioVorbis&);
    StreamAudioVorbis& operator=(const StreamAudioVorbis&);

protected:
    StreamAudioVorbis(Context&, const unsigned char*, unsigned long);

    unsigned long GetSamplesPerSec() const;
    unsigned char GetChannels() const;

    void WriteTrackCodecID();
    void WriteTrackCodecName();
    void WriteTrackCodecPrivate();

public:
    //static void GetMediaTypes(CMediaTypes&);
    //static bool QueryAccept(const AM_MEDIA_TYPE&);
    //static bool OnReceiveConnection(const AM_MEDIA_TYPE&);

    //static HRESULT GetAllocatorRequirements(
    //                const AM_MEDIA_TYPE&,
    //                ALLOCATOR_PROPERTIES&);

	static StreamAudio* CreateStream(Context&, const VorbisTypes::VORBISFORMAT2&);

    int Receive(MediaSample*);
    int EndOfStream();

private:
    const VorbisTypes::VORBISFORMAT2& GetFormat() const;

    class VorbisFrame : public AudioFrame
    {
        VorbisFrame& operator=(const VorbisFrame&);
        VorbisFrame(const VorbisFrame&);

    private:
        unsigned long m_timecode;
        unsigned char* m_data;
        unsigned long m_size;

    public:
        explicit VorbisFrame(MediaSample*, StreamAudioVorbis*);
        ~VorbisFrame();

        unsigned long GetTimecode() const;
        unsigned long GetSize() const;
        const unsigned char* GetData() const;

    };

};

}  //end namespace webmmux
