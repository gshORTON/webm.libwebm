// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include <climits>
#include <cassert>
#include <cstring>
#include "webmmuxcontext.hpp"
#include "webmmuxstreamvideovpx.hpp"
#ifdef _DEBUG
//#include "odbgstream.hpp"
using std::endl;
using std::boolalpha;
using std::hex;
using std::dec;
#endif

namespace webmmux
{

StreamVideoVPx::VPxFrame::VPxFrame(
    MediaSample* pSample,
    StreamVideoVPx* pStream) :
    m_pSample(new MediaSample(*pSample))
{

    assert(m_pSample);
    m_pSample->pData = 0;
    assert(pSample->pData);

    //create a separate copy of the data for the frame
    m_pSample->pData = new unsigned char[pSample->bufLength];
    memcpy(m_pSample->pData, pSample->pData, pSample->bufLength);

    long long st, sp;  //reftime units

    st = m_pSample->startTime;
    assert(st >= 0);

    //const long long ns = st * 100;  //nanoseconds

    const Context& ctx = pStream->m_context;
    const unsigned long scale = ctx.GetTimecodeScale();
    assert(scale >= 1);

    const long long tc = st / scale;
    assert(tc <= ULONG_MAX);

    m_timecode = static_cast<unsigned long>(tc);
}


StreamVideoVPx::VPxFrame::~VPxFrame()
{
    if (m_pSample->pData)
       delete[] m_pSample->pData;
}


unsigned long StreamVideoVPx::VPxFrame::GetTimecode() const
{
    return m_timecode;
}


bool StreamVideoVPx::VPxFrame::IsKey() const
{
    return(m_pSample->extraData & VPX_FRAME_IS_KEY);
}


unsigned long StreamVideoVPx::VPxFrame::GetSize() const
{
    const long result = m_pSample->bufLength;
    assert(result >= 0);

    return result;
}


const unsigned char* StreamVideoVPx::VPxFrame::GetData() const
{
    assert(m_pSample->pData);
    return m_pSample->pData;
}



StreamVideoVPx::StreamVideoVPx(
    Context& c,
    const StreamInfo& mt) :
    StreamVideo(c, mt)
{
}


//void StreamVideoVPx::WriteTrackName()
//{
//    EbmlIO::File& f = m_context.m_file;
//
//    f.WriteID2(0x536E);   //name
//    f.Write1UTF8(L"VP8 video stream");  //TODO
//}



void StreamVideoVPx::WriteTrackCodecID()
{
    EbmlIO::File& f = m_context.m_file;

    f.WriteID1(0x86);  //Codec ID
    f.Write1String("V_VP8");
}


void StreamVideoVPx::WriteTrackCodecName()
{
    EbmlIO::File& f = m_context.m_file;

    f.WriteID3(0x258688);  //Codec Name
    f.Write1UTF8(L"VP8");
}


void StreamVideoVPx::WriteTrackSettings()
{
    EbmlIO::File& f = m_context.m_file;

    f.WriteID1(0xE0);  //video settings

    //allocate 2 bytes of storage for size of settings
    const long long begin_pos = f.SetPosition(2, std::ios_base::cur);

/*    const BITMAPINFOHEADER& bmih = GetBitmapInfoHeader();
    assert(bmih.biSize >= sizeof(BITMAPINFOHEADER));
    assert(bmih.biWidth > 0);
    assert(bmih.biWidth <= USHRT_MAX);
    assert(bmih.biHeight > 0);
    assert(bmih.biHeight <= USHRT_MAX);

    const USHORT width = static_cast<USHORT>(bmih.biWidth);
    const USHORT height = static_cast<USHORT>(bmih.biHeight);*/

    const unsigned short width = static_cast<unsigned short>(m_Info.GetWidth());
    const unsigned short height = static_cast<unsigned short>(m_Info.GetHeight());

    f.WriteID1(0xB0);  //width
    f.Write1UInt(2);
    f.Serialize2UInt(width);

    f.WriteID1(0xBA);  //height
    f.Write1UInt(2);
    f.Serialize2UInt(height);

    const float framerate = GetFramerate();

    if (framerate > 0)
    {
        f.WriteID3(0x2383E3);  //frame rate
        f.Write1UInt(4);
        f.Serialize4Float(framerate);
    }

    const long long end_pos = f.GetPosition();

    const long long size_ = end_pos - begin_pos;
    assert(size_ <= USHRT_MAX);

    const unsigned short size = static_cast<unsigned short>(size_);

    f.SetPosition(begin_pos - 2);
    f.Write2UInt(size);

    f.SetPosition(end_pos);
}


int StreamVideoVPx::Receive(MediaSample* pSample)
{
    assert(pSample);

#if 0
    long long st, sp;
    const HRESULT hrTime = pSample->GetTime(&st, &sp);

    odbgstream os;

    os << "webmmux::vpx::receive: hrTime="
       << hex << hrTime << dec;

    if (SUCCEEDED(hrTime))
    {
        os << " st=" << st
           << " st.ms=" << double(st) / 10000;

        if (hrTime == S_OK)
            os << " sp="
               << sp
               << " sp.ms="
               << double(sp) / 10000
               << " dt.ms="
               << (double(sp-st) / 10000);
    }

    //os << " frame.GetTimecode="
    //   << pFrame->GetTimecode();

    os << endl;
#endif

    EbmlIO::File& file = m_context.m_file;

    if (file.GetStream() == 0)
        return 0;

    VPxFrame* const pFrame = new (std::nothrow) VPxFrame(pSample, this);
    assert(pFrame);  //TODO

    assert(!m_vframes.empty() || pFrame->IsKey());

    m_vframes.push_back(pFrame);

    m_context.NotifyVideoFrame(this, pFrame);

    return 0;
}


int StreamVideoVPx::EndOfStream()
{
    return m_context.NotifyVideoEOS(this);
}


long StreamVideoVPx::GetLastTimecode() const
{
    if (m_vframes.empty())
        return -1;

    VideoFrame* const pFrame = m_vframes.back();
    assert(pFrame);

    return pFrame->GetTimecode();
}


}  //end namespace webmmux

