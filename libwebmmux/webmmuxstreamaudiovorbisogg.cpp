// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include <cstring>
#include <cassert>
#include <climits>
#include "webmmuxcontext.hpp"
#include "webmmuxstreamaudiovorbisogg.hpp"
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

// |StreamAudioVorbisOgg::WriteTrackCodecPrivate| and
// |StreamAudioVorbisOgg::FinalizeTrackCodecPrivate| use
// kPRIVATE_DATA_BYTES_RESERVED to preallocate and write (respectively) the
// track's codec private data block.
enum { kPRIVATE_DATA_BYTES_RESERVED = 8000 };

StreamAudio* StreamAudioVorbisOgg::CreateStream(
    Context& ctx,
    const VorbisTypes::VORBISFORMAT& mt)
{
    //assert(QueryAccept(mt));

    const unsigned char* const pb = static_cast<const unsigned char*>(static_cast<const void*>(&mt));
    const unsigned long cb = sizeof(VorbisTypes::VORBISFORMAT);

    return new (std::nothrow) StreamAudioVorbisOgg(ctx, pb, cb);
}


#if 0
StreamAudioVorbisOgg::VorbisFrame::VorbisFrame(
    IMediaSample* pSample,
    ULONG sample_rate,
    ULONG timecode_scale) :
    m_pSample(pSample),
    m_timecode(CalculateTimecode(pSample, sample_rate, timecode_scale))
{
    m_pSample->AddRef();
}


ULONG StreamAudioVorbisOgg::VorbisFrame::CalculateTimecode(
    IMediaSample* pSample,
    ULONG sample_rate,
    ULONG timecode_scale)
{
    ULONG timecode = 0;

    assert(pSample);

    REFERENCE_TIME sampletime_start, sampletime_end;

    HRESULT status = pSample->GetTime(&sampletime_start, &sampletime_end);
    assert(SUCCEEDED(status));

    double time_in_seconds = static_cast<double>(sampletime_start);

    const double dsample_rate = sample_rate;
    time_in_seconds /= dsample_rate;

    const double nanoseconds_per_second = 1000000000.0; // 10^9

    const double time_in_nanoseconds =
        time_in_seconds * nanoseconds_per_second;

    const double dtimecode_scale = timecode_scale;
    assert(timecode_scale > 0);

    const double dtimecode = time_in_nanoseconds / dtimecode_scale;
    assert(timecode <= ULONG_MAX);

    timecode = static_cast<ULONG>(dtimecode);

#if 0 //def _DEBUG
    odbgstream dbg;
    dbg << "["__FUNCTION__"] "
        << " sampletime_start=" << sampletime_start
        << " sampletime_end=" << sampletime_end
        << " duration[samples]=" << sampletime_end - sampletime_start
        << endl
        << "   timecode=" << timecode
        << " time[seconds]=" << time_in_seconds
        << " time[nanoseconds]=" << time_in_nanoseconds
        << endl;
#endif

    return timecode;
}


StreamAudioVorbisOgg::VorbisFrame::~VorbisFrame()
{
    const ULONG n = m_pSample->Release();
    n;
}
#else
StreamAudioVorbisOgg::VorbisFrame::VorbisFrame(
    MediaSample* pSample,
    StreamAudioVorbisOgg* pStream)
{

    long long st = pSample->startTime;
    assert(st >= 0);

    const double samples = double(st);

    const unsigned long samplesPerSec = pStream->GetSamplesPerSec();
    assert(samplesPerSec > 0);

    //secs [=] samples / samples/sec
    const double secs = samples / double(samplesPerSec);

    //const double ns = secs * 1000000000.0;

    const Context& ctx = pStream->m_context;
    const unsigned long scale = ctx.GetTimecodeScale();
    assert(scale >= 1);

    const double tc = secs / scale;
    assert(tc <= ULONG_MAX);

    m_timecode = static_cast<unsigned long>(tc);

    const long size = pSample->bufLength;
    assert(size > 0);

    m_size = size;

    unsigned char* ptr = pSample->pData;
    assert(ptr);

    m_data = new (std::nothrow) unsigned char[m_size];
    assert(m_data);  //TODO

    memcpy(m_data, ptr, m_size);
}


StreamAudioVorbisOgg::VorbisFrame::~VorbisFrame()
{
    delete[] m_data;
}
#endif


const VorbisTypes::VORBISFORMAT&
StreamAudioVorbisOgg::GetFormat() const
{
    using VorbisTypes::VORBISFORMAT;

    unsigned long cb;
    const void* const pv = StreamAudio::GetFormat(cb);
    assert(pv);
    assert(cb >= sizeof(VORBISFORMAT));

    const VORBISFORMAT* const pfmt = static_cast<const VORBISFORMAT*>(pv);
    assert(pfmt);

    const VORBISFORMAT& fmt = *pfmt;
    return fmt;
}


unsigned long StreamAudioVorbisOgg::GetSamplesPerSec() const
{
    const VorbisTypes::VORBISFORMAT& fmt = GetFormat();
    assert(fmt.samplesPerSec > 0);

    return fmt.samplesPerSec;
}



BYTE StreamAudioVorbisOgg::GetChannels() const
{
    const VorbisTypes::VORBISFORMAT& fmt = GetFormat();
    assert(fmt.numChannels > 0);

    return fmt.numChannels;
}


unsigned long StreamAudioVorbisOgg::VorbisFrame::GetTimecode() const
{
    return m_timecode;
}


unsigned long StreamAudioVorbisOgg::VorbisFrame::GetSize() const
{
#if 0
   const long result = m_pSample->GetActualDataLength();
   assert(result >= 0);

   return result;
#else
    return m_size;
#endif
}


const unsigned char* StreamAudioVorbisOgg::VorbisFrame::GetData() const
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


StreamAudioVorbisOgg::StreamAudioVorbisOgg(
    Context& ctx,
    const unsigned char* pb,
    unsigned long cb) :
    StreamAudio(ctx, pb, cb)
{
}


void StreamAudioVorbisOgg::Final()
{
    FinalizeTrackCodecPrivate();
}



void StreamAudioVorbisOgg::WriteTrackCodecID()
{
    webmmux::File& f = m_context.m_file;

    f.WriteID1(0x86);  //Codec ID
    f.Write1String("A_VORBIS");
}


void StreamAudioVorbisOgg::WriteTrackCodecName()
{
    webmmux::File& f = m_context.m_file;

    f.WriteID3(0x258688);  //Codec Name
    f.Write1UTF8(L"VORBIS");
}


void StreamAudioVorbisOgg::WriteTrackCodecPrivate()
{
    webmmux::File& file = m_context.m_file;

    m_codec_private_data_pos = file.GetPosition();

    file.SetPosition(kPRIVATE_DATA_BYTES_RESERVED, webmmux::EBMLIO_SEEK_CURRENT);

#if 0 //def _DEBUG
    odbgstream ods;
    ods << "["__FUNCTION__"] " << "RESERVED SPACE! m_codec_private_data_pos="
        << m_codec_private_data_pos << " kPRIVATE_DATA_BYTES_RESERVED="
        << kPRIVATE_DATA_BYTES_RESERVED << std::endl;
#endif
}


int StreamAudioVorbisOgg::FinalizeTrackCodecPrivate()
{
    webmmux::File& file = m_context.m_file;

    const long long old_pos = file.GetPosition();
    file.SetPosition(m_codec_private_data_pos);

    if (m_ident.empty() || m_comment.empty() || m_setup.empty())
    {
        //debit allocated size by Void type (1) and length (2)
        const int bytes_to_write_ = kPRIVATE_DATA_BYTES_RESERVED - 1 - 2;
        assert(bytes_to_write_ <= USHRT_MAX);

        const unsigned short bytes_to_write = static_cast<unsigned short>(bytes_to_write_);

        file.WriteID1(0xEC); // Void
        file.Write2UInt(bytes_to_write);

        file.SetPosition(old_pos);

        return 0;
    }

    const unsigned long ident_len = static_cast<const unsigned long>(m_ident.size());
    assert(ident_len > 0);
    assert(ident_len <= 255);

    const unsigned long comment_len = static_cast<const unsigned long>(m_comment.size());
    assert(comment_len > 0);
    assert(comment_len <= 255);

    const unsigned long setup_len = static_cast<const unsigned long>(m_setup.size());
    assert(setup_len > 0);

    const unsigned long hdr_len = ident_len + comment_len + setup_len;

    const unsigned long len = 1 + 1 + 1 + hdr_len;
    //1 byte = number of headers - 1
    //1 byte = ident len
    //1 byte = comment len
    //(len of setup_len is implied by total len)

    file.WriteID2(0x63A2);  //Codec Private
    file.Write4UInt(len);

    unsigned char val = 2;  //number of headers - 1
    file.Write(&val, 1);

    val = static_cast<unsigned char>(ident_len);
    file.Write(&val, 1);

    val = static_cast<unsigned char>(comment_len);
    file.Write(&val, 1);
    file.Write(&m_ident[0], ident_len);

    // TODO(tomfinegan): if |PRIVATE_DATA_BYTES_RESERVED| did not allow for
    // enough storage, we could write an empty comment header to avoid
    // trouble. (assuming that buys us enough space)
    file.Write(&m_comment[0], comment_len);
    file.Write(&m_setup[0], setup_len);

    // fill in any remaining space with a Void element
    const long long& private_begin_pos = m_codec_private_data_pos;
    const long long private_end_pos =
        private_begin_pos + kPRIVATE_DATA_BYTES_RESERVED;

    if (file.GetPosition() < private_end_pos)
    {
        long long llbytes_to_write = private_end_pos - file.GetPosition();

        assert(llbytes_to_write >= 3); // 1 for id, 2 for len
        assert(llbytes_to_write < kPRIVATE_DATA_BYTES_RESERVED);

        unsigned short bytes_to_write = static_cast<unsigned short>(llbytes_to_write);

        bytes_to_write = bytes_to_write - 1 - 2; // Void type, length

        // create a void element and set its size (no need to fill)
        file.WriteID1(0xEC); // Void
        file.Write2UInt(bytes_to_write);
    }

    const long long actual_bytes_written =
        file.GetPosition() - private_begin_pos;

    assert(actual_bytes_written <= kPRIVATE_DATA_BYTES_RESERVED);

    file.SetPosition(old_pos);

    return 0;
}


int StreamAudioVorbisOgg::Receive(MediaSample* pSample)
{
    if (pSample == 0)
        return -1;

    unsigned char* buf = pSample->pData;
    assert(buf);

    const long len = pSample->bufLength;

    if (len < 0)
        return -1;

    if (len == 0)
        return 0;  //?

    unsigned char* const buf_end = buf + len;

    if (m_ident.empty())
    {
        assert(len >= 7);
        assert(buf[0] == 1);
        assert(memcmp(buf + 1, "vorbis", 6) == 0);
        m_ident.assign(buf, buf_end);

        return 0;
    }

    if (m_comment.empty())
    {
        assert(len >= 7);
        assert(buf[0] == 3);
        assert(memcmp(buf + 1, "vorbis", 6) == 0);
        m_comment.assign(buf, buf_end);

        return 0;
    }

    if (m_setup.empty())
    {
        assert(len >= 7);
        assert(buf[0] == 5);
        assert(memcmp(buf + 1, "vorbis", 6) == 0);
        m_setup.assign(buf, buf_end);

        return 0;
    }

    webmmux::File& file = m_context.m_file;

    if (file.GetStream() == 0)
        return 0;

    //In order to construct a frame, we need to have
    //both the start and stop times, so we check it
    //here before calling the ctor.

    long long st, sp;

    st = pSample->startTime;
    sp = pSample->stopTime;

    if (st >= sp)
        return 0;  //throw away this sample

    VorbisFrame* const pFrame = new (std::nothrow) VorbisFrame(pSample, this);
    assert(pFrame);

    m_context.NotifyAudioFrame(this, pFrame);

    return 0;
}


int StreamAudioVorbisOgg::EndOfStream()
{
    return m_context.NotifyAudioEOS(this);
}

}  //end namespace webmmux
