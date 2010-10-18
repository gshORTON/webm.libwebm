// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include "vorbistypes.hpp"
#include "webmmuxstreamaudio.hpp"
#include "webmmuxcontext.hpp"

#include <vector>

class CMediaTypes;

namespace VorbisTypes
{
struct VORBISFORMAT;
}

namespace webmmux
{

class StreamAudioVorbisOgg : public StreamAudio
{
    StreamAudioVorbisOgg(const StreamAudioVorbisOgg&);
    StreamAudioVorbisOgg& operator=(const StreamAudioVorbisOgg&);

protected:
    StreamAudioVorbisOgg(Context&, const unsigned char*, unsigned long);
    virtual void Final();  //grant last wishes

    unsigned long GetSamplesPerSec() const;
    unsigned char GetChannels() const;

    void WriteTrackCodecID();
    void WriteTrackCodecName();
    void WriteTrackCodecPrivate();

public:
    //static void GetMediaTypes(CMediaTypes&);
    //static bool QueryAccept(const AM_MEDIA_TYPE&);

    //static HRESULT GetAllocatorRequirements(
    //                const AM_MEDIA_TYPE&,
    //                ALLOCATOR_PROPERTIES&);

	static StreamAudio* CreateStream(Context&, const VorbisTypes::VORBISFORMAT&);

    int Receive(MediaSample*);
    int EndOfStream();

private:
    typedef std::vector<unsigned char> header_t;
    header_t m_ident;
    header_t m_comment;
    header_t m_setup;

    const VorbisTypes::VORBISFORMAT& GetFormat() const;

    class VorbisFrame : public AudioFrame
    {
        VorbisFrame& operator=(const VorbisFrame&);
        VorbisFrame(const VorbisFrame&);

    private:
        unsigned long m_timecode;
        unsigned char* m_data;
        unsigned long m_size;

    public:
        VorbisFrame(MediaSample*, StreamAudioVorbisOgg*);
        ~VorbisFrame();

        unsigned long GetTimecode() const;
        unsigned long GetSize() const;
        const unsigned char* GetData() const;
    };

    long long m_codec_private_data_pos;

    int FinalizeTrackCodecPrivate();
};

}  //end namespace webmmux
