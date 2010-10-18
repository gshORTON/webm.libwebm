// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include <cassert>
#include <cstring>
#include <climits>
//#include <uuids.h>
#include "webmmuxcontext.hpp"
#include "webmmuxstreamaudiovorbis.hpp"
#include "vorbistypes.hpp"
#if 0 //def _DEBUG
//#include <odbgstream.hpp>
#include <iomanip>
using std::endl;
using std::hex;
using std::dec;
using std::setfill;
using std::setw;
#endif

namespace webmmux
{


#if 0
	//TODO: DW GetMediaTypes needed for generic lib?
void StreamAudioVorbis::GetMediaTypes(CMediaTypes& mtv)
{
    AM_MEDIA_TYPE mt;

    mt.majortype = MEDIATYPE_Audio;
    mt.subtype = VorbisTypes::MEDIASUBTYPE_Vorbis2;
    mt.bFixedSizeSamples = FALSE;
    mt.bTemporalCompression = FALSE;
    mt.lSampleSize = 0;
    mt.formattype = GUID_NULL;
    mt.pUnk = 0;
    mt.cbFormat = 0;
    mt.pbFormat = 0;

    mtv.Add(mt);
}
#endif

#if 0
//TODO: DW QueryAccept needed for generic lib?
bool StreamAudioVorbis::QueryAccept(const AM_MEDIA_TYPE& mt)
{
    if (mt.majortype != MEDIATYPE_Audio)
        return false;

    if (mt.subtype != VorbisTypes::MEDIASUBTYPE_Vorbis2)
        return false;

    if (mt.formattype != VorbisTypes::FORMAT_Vorbis2)
        return false;

    using VorbisTypes::VORBISFORMAT2;

    if (mt.cbFormat < sizeof(VORBISFORMAT2))
        return false;

    if (mt.pbFormat == 0)
        return false;

    const VORBISFORMAT2& fmt = (VORBISFORMAT2&)(*mt.pbFormat);

    if (fmt.channels == 0)
        return false;

    if (fmt.channels > 255)
        return false;

    if (fmt.samplesPerSec == 0)
        return false;

    //TODO: fmt.dwBitsPerSample

    const unsigned long ident_len = fmt.headerSize[0];

    if (ident_len == 0)  //TODO: should be 30?
        return false;

    const unsigned long comments_len = fmt.headerSize[1];

    //TODO: I think the comments header is optional
    if (comments_len == 0)
        return false;

    const unsigned long setup_len = fmt.headerSize[2];

    if (setup_len == 0)
        return false;

    const unsigned long hdr_len = ident_len + comments_len + setup_len;
    const unsigned long cbFormat = sizeof(VORBISFORMAT2) + hdr_len;

    if (mt.cbFormat < cbFormat)
        return false;

    return true;
}
#endif


#if 0
HRESULT StreamAudioVorbis::GetAllocatorRequirements(
    const AM_MEDIA_TYPE&,
    ALLOCATOR_PROPERTIES&)
{
    //Here we assume a Vorbis audio packet is 20ms in duration,
    //which makes 50 packets per one second of audio.  We then buffer
    //3 seconds worth of audio.
    //TODO: determine what the duration of a Vorbis packet actually is.
    //UPDATE: James Z. said that Vorbis packets have powers-of-two
    //number of samples, from 64 - 8192.  The number of buffers
    //could be calculated as:
    //   number of buffers [=] samples/sec / 64 samples/buffer
    //which is about 750 buffers/sec.  We could either choose to
    //fix the number of buffers we allocated, which determines
    //how much we're able to buffer, or fix how much we want to
    //buffer, and then copy the audio data from the media samples
    //into a local buffer.  (The latter approach is less likely
    //to stall the graph, but it's not as efficient.)

    props.cBuffers = 3 * 50;
    props.cbBuffer = 0;
    props.cbAlign = 0;
    props.cbPrefix = 0;

    return S_OK;
}
#endif


StreamAudio* StreamAudioVorbis::CreateStream(
    Context& ctx,
	const VorbisTypes::VORBISFORMAT2& mt)
{
	//TODO: DW Re-enable if QueryAccept is needed for generic lib
    //assert(QueryAccept(mt));

	//TODO: DW casting to void to unsigned char* seems suspect, but it works...
    const unsigned char* const pb = static_cast<const unsigned char*>(static_cast<const void*>(&mt));
	const unsigned long cb = sizeof(VorbisTypes::VORBISFORMAT2);

    return new (std::nothrow) StreamAudioVorbis(ctx, pb, cb);
}



#if 0
StreamAudioVorbis::VorbisFrame::VorbisFrame(
    IMediaSample* pSample,
    StreamAudioVorbis* pStream) :
    m_pSample(pSample)
{
    m_pSample->AddRef();

    long long st, sp;  //reftime units

    const HRESULT hr = m_pSample->GetTime(&st, &sp);
    assert(SUCCEEDED(hr));
    assert(st >= 0);

    const long long ns = st * 100;  //nanoseconds

    const Context& ctx = pStream->m_context;
    const ULONG scale = ctx.GetTimecodeScale();
    assert(scale >= 1);

    //TODO: verify this when scale equals audio sampling rate
    const long long tc = ns / scale;
    assert(tc <= ULONG_MAX);

    m_timecode = static_cast<ULONG>(tc);
}
#else
StreamAudioVorbis::VorbisFrame::VorbisFrame(
    MediaSample* pSample,
    StreamAudioVorbis* pStream)
{
    long long st, sp;  //reftime units

    st = pSample->GetStartTime();
    assert(st >= 0);

    const long long ns = st * 100;  //nanoseconds

    const Context& ctx = pStream->m_context;
    const unsigned long scale = ctx.GetTimecodeScale();
    assert(scale >= 1);

    //TODO: verify this when scale equals audio sampling rate
    const long long tc = ns / scale;
    assert(tc <= ULONG_MAX);

    m_timecode = static_cast<unsigned long>(tc);

    const long size = pSample->GetBufLength();
    assert(size > 0);

    m_size = size;

    unsigned char* ptr;

    int res = pSample->GetDataBuf(&ptr);
    assert(res == 0);
    assert(ptr);

    m_data = new (std::nothrow) unsigned char[m_size];
    assert(m_data);  //TODO

    memcpy(m_data, ptr, m_size);
}
#endif


StreamAudioVorbis::VorbisFrame::~VorbisFrame()
{
#if 0
    const ULONG n = m_pSample->Release();
    n;
#else
    delete[] m_data;
#endif
}


const VorbisTypes::VORBISFORMAT2&
StreamAudioVorbis::GetFormat() const
{
    using VorbisTypes::VORBISFORMAT2;

    unsigned long cb;
    const void* const pv = StreamAudio::GetFormat(cb);
    assert(pv);
    assert(cb >= sizeof(VORBISFORMAT2));

    const VORBISFORMAT2* const pfmt = static_cast<const VORBISFORMAT2*>(pv);
    assert(pfmt);

    const VORBISFORMAT2& fmt = *pfmt;
    return fmt;
}


unsigned long StreamAudioVorbis::GetSamplesPerSec() const
{
    const VorbisTypes::VORBISFORMAT2& fmt = GetFormat();
    assert(fmt.samplesPerSec > 0);

    return fmt.samplesPerSec;
}


unsigned char StreamAudioVorbis::GetChannels() const
{
    const VorbisTypes::VORBISFORMAT2& fmt = GetFormat();
    assert(fmt.channels > 0);
    assert(fmt.channels <= 255);

    return static_cast<unsigned char>(fmt.channels);
}



unsigned long StreamAudioVorbis::VorbisFrame::GetTimecode() const
{
    return m_timecode;
}


unsigned long StreamAudioVorbis::VorbisFrame::GetSize() const
{
#if 0
   const long result = m_pSample->GetActualDataLength();
   assert(result >= 0);

   return result;
#else
    return m_size;
#endif
}


const unsigned char* StreamAudioVorbis::VorbisFrame::GetData() const
{
#if 0
    unsigned char* ptr;

    const HRESULT hr = m_pSample->GetPointer(&ptr);
    assert(SUCCEEDED(hr));
    assert(ptr);

    return ptr;
#else
    return m_data;
#endif
}


StreamAudioVorbis::StreamAudioVorbis(
    Context& ctx,
    const unsigned char* pb,
    unsigned long cb) :
    StreamAudio(ctx, pb, cb)
{
}


void StreamAudioVorbis::WriteTrackCodecID()
{
    EbmlIO::File& f = m_context.m_file;

    f.WriteID1(0x86);  //Codec ID
    f.Write1String("A_VORBIS");
}


void StreamAudioVorbis::WriteTrackCodecName()
{
    EbmlIO::File& f = m_context.m_file;

    f.WriteID3(0x258688);  //Codec Name
    f.Write1UTF8(L"VORBIS");
}


void StreamAudioVorbis::WriteTrackCodecPrivate()
{
    typedef VorbisTypes::VORBISFORMAT2 F;

    unsigned long cb;
    const void* const pv = StreamAudio::GetFormat(cb);
    assert(cb >= sizeof(F));
    assert(pv);

    const F* const pf = static_cast<const F*>(pv);
    const F& f = *pf;
    assert(f.channels > 0);
    assert(f.samplesPerSec > 0);

    const unsigned long ident_len = f.headerSize[0];
    assert(ident_len > 0);
    assert(ident_len <= 255);

    const unsigned long comment_len = f.headerSize[1];
    assert(comment_len > 0);
    assert(comment_len <= 255);

    const unsigned long setup_len = f.headerSize[2];
    assert(setup_len > 0);

    const unsigned long hdr_len = ident_len + comment_len + setup_len;
    assert(cb == (sizeof(F) + hdr_len));

    const unsigned char* const pb = static_cast<const unsigned char*>(pv);
    const unsigned char* const hdr_ptr = pb + sizeof(F);

    const unsigned long len = 1 + 1 + 1 + hdr_len;
    //1 unsigned char = number of headers - 1
    //1 unsigned char = ident len
    //1 unsigned char = comment len
    //(len of setup_len is implied by total len)

    EbmlIO::File& file = m_context.m_file;

    file.WriteID2(0x63A2);  //Codec Private
    file.Write4UInt(len);

    unsigned char val = 2;  //number of headers - 1
    file.Write(&val, 1);

    val = static_cast<unsigned char>(ident_len);
    file.Write(&val, 1);

    val = static_cast<unsigned char>(comment_len);
    file.Write(&val, 1);

    file.Write(hdr_ptr, hdr_len);
}


int StreamAudioVorbis::Receive(MediaSample* pSample)
{
    assert(pSample);

    EbmlIO::File& file = m_context.m_file;

    if (file.GetStream() == 0)
        return 0;

    VorbisFrame* const pFrame = new (std::nothrow) VorbisFrame(pSample, this);
    assert(pFrame);  //TODO

    m_context.NotifyAudioFrame(this, pFrame);

    return 0;
}


int StreamAudioVorbis::EndOfStream()
{
    return m_context.NotifyAudioEOS(this);
}


}  //end namespace webmmux
