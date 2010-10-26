// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <limits>
#include <malloc.h>  //_malloca
#include "webmmuxebmlio.hpp"

webmmux::File::File() : m_pStream(0)
{
}


webmmux::File::~File()
{
    assert(m_pStream == 0);
}


void webmmux::File::SetStream(webmmux::EbmlStream* p)
{
    assert((m_pStream == 0) || (p == 0));
    m_pStream = p;
}


webmmux::EbmlStream* webmmux::File::GetStream() const
{
    return m_pStream;
}


int32_t webmmux::File::SetSize(int64_t size)
{
    return webmmux::SetSize(m_pStream, size);
}


int64_t webmmux::File::SetPosition(
    int64_t pos,
    EbmlIOSeekPosition origin)
{
    return webmmux::SetPosition(m_pStream, pos, origin);
}


int64_t webmmux::File::GetPosition() const
{
    File* const const_file = const_cast<File*>(this);
    return const_file->SetPosition(0, EBMLIO_SEEK_CURRENT);
}



//void webmmux::File::Serialize(
//    const void* buf,
//    uint32_t len)
//{
//    webmmux::Serialize(m_pStream, buf, len);
//}


void webmmux::File::Write(const void* buf, uint32_t cb)
{
    webmmux::Write(m_pStream, buf, cb);
}


void webmmux::File::Serialize8UInt(uint64_t val)
{
    webmmux::Serialize(m_pStream, &val, 8);
}



void webmmux::File::Serialize4UInt(uint32_t val)
{
    webmmux::Serialize(m_pStream, &val, 4);
}


void webmmux::File::Serialize2UInt(uint16_t val)
{
    webmmux::Serialize(m_pStream, &val, 2);
}


void webmmux::File::Serialize1UInt(uint8_t val)
{
    webmmux::Serialize(m_pStream, &val, 1);
}


void webmmux::File::Serialize2SInt(int16_t val)
{
    webmmux::Serialize(m_pStream, &val, 2);
}


void webmmux::File::Serialize4Float(float val)
{
    webmmux::Serialize(m_pStream, &val, 4);
}


void webmmux::File::WriteID4(uint32_t id)
{
    webmmux::WriteID4(m_pStream, id);
}


void webmmux::File::WriteID3(uint32_t id)
{
    webmmux::WriteID3(m_pStream, id);
}


void webmmux::File::WriteID2(uint16_t id)
{
    webmmux::WriteID2(m_pStream, id);
}


void webmmux::File::WriteID1(uint8_t id)
{
    webmmux::WriteID1(m_pStream, id);
}


uint32_t webmmux::File::ReadID4()
{
    return webmmux::ReadID4(m_pStream);
}


void webmmux::File::Write8UInt(uint64_t val)
{
    webmmux::Write8UInt(m_pStream, val);
}


void webmmux::File::Write4UInt(uint32_t val)
{
    webmmux::Write4UInt(m_pStream, val);
}


void webmmux::File::Write2UInt(uint16_t val)
{
    webmmux::Write2UInt(m_pStream, val);
}


void webmmux::File::Write1UInt(uint8_t val)
{
    webmmux::Write1UInt(m_pStream, val);
}


void webmmux::File::Write1String(const char* str)
{
    webmmux::Write1String(m_pStream, str);
}


//void webmmux::File::Write1String(const char* str, size_t len)
//{
//    webmmux::Write1String(m_pStream, str, len);
//}


void webmmux::File::Write1UTF8(const wchar_t* str)
{
    webmmux::Write1UTF8(m_pStream, str);
}


int32_t webmmux::SetSize(webmmux::EbmlStream* pStream, int64_t size_)
{
    assert(pStream);
    assert(size_ >= 0);

    return pStream->SetSize(size_);
}


int64_t webmmux::SetPosition(
    webmmux::EbmlStream* pStream,
    int64_t move_,
    EbmlIOSeekPosition origin)
{
    assert(pStream);

    uint64_t newpos;

    const int res = pStream->Seek(move_, origin, &newpos);
    assert(res);

    return newpos;
}


void webmmux::Write(
    webmmux::EbmlStream* pStream,
    const void* buf,
    uint32_t cb)
{
    assert(pStream);

    uint32_t cbWritten;

    const int res = pStream->Write(buf, cb, &cbWritten);
    assert(res);
    assert(cbWritten == cb);
}



void webmmux::Write8UInt(webmmux::EbmlStream* pStream, uint64_t val)
{
    assert(val <= 0x00FFFFFFFFFFFFFEULL);  //0000 000x 1111 1111 ...
    val |= 0x0100000000000000ULL;          //always write 8 unsigned chars

    Serialize(pStream, &val, 8);
}


void webmmux::Write4UInt(webmmux::EbmlStream* pStream, uint32_t val)
{
    assert(val <= 0x0FFFFFFE);  //000x 1111 1111 ...
    val |= 0x10000000;  //always write 4 unsigned chars

    Serialize(pStream, &val, 4);
}


void webmmux::Write2UInt(webmmux::EbmlStream* pStream, uint16_t val)
{
    assert(val <= 0x3FFE);  //0x11 1111 1111 1110
    val |= 0x4000;          //always write 2 unsigned chars

    Serialize(pStream, &val, 2);
}


void webmmux::Write1UInt(webmmux::EbmlStream* pStream, uint8_t val)
{
    assert(val <= 0x7E);  //x111 1110
    val |= 0x80;          //always write 1 unsigned char

    Serialize(pStream, &val, 1);
}


void webmmux::WriteID4(webmmux::EbmlStream* pStream, uint32_t id)
{
    assert(pStream);
    assert(id & 0x10000000);  //always write 4 unsigned chars
    assert(id <= 0x1FFFFFFE);

    Serialize(pStream, &id, 4);
}


uint32_t webmmux::ReadID4(webmmux::EbmlStream* pStream)
{
    assert(pStream);

    uint32_t id;

    uint8_t* const p = reinterpret_cast<uint8_t*>(&id);
    uint8_t* q = p + 4;

    for (;;)
    {
        uint32_t cb;

        const int res = pStream->Read(--q, 1, &cb);
        assert(res);
        assert(cb == 1);

        if (q == p)
            break;
    }

    assert(id & 0x10000000);
    assert(id <= 0x1FFFFFFE);

    return id;
}


void webmmux::WriteID3(webmmux::EbmlStream* pStream, uint32_t id)
{
    assert(pStream);
    assert(id & 0x200000);  //always write 3 unsigned chars
    assert(id <= 0x3FFFFE);

    Serialize(pStream, &id, 3);
}


void webmmux::WriteID2(webmmux::EbmlStream* pStream, uint16_t id)
{
    assert(pStream);
    assert(id & 0x4000);  //always write 2 unsigned chars
    assert(id <= 0x7FFE);

    Serialize(pStream, &id, 2);
}


void webmmux::WriteID1(webmmux::EbmlStream* pStream, uint8_t id)
{
    assert(pStream);
    assert(id & 0x80);  //always write 1 unsigned char
    assert(id <= 0xFE);

    Serialize(pStream, &id, 1);
}


void webmmux::Write1String(
    webmmux::EbmlStream* pStream,
    const char* str)
{
    assert(pStream);
    assert(str);

    const size_t size_ = strlen(str);
    assert(size_ <= 255);

    const uint8_t size = static_cast<unsigned char>(size_);

    Write1UInt(pStream, size);
    Write(pStream, str, size);
}


#if 0
void webmmux::Write1String(
    webmmux::EbmlStream* pStream,
    const char* str,
    size_t buflen)
{
    assert(pStream);
    assert(str);

    const size_t strlen_ = strlen(str);
    const size_t size_ = (strlen_ >= buflen) ? strlen_ : buflen;
    assert(size_ <= 255);

    Write1UInt(pStream, static_cast<unsigned char>(size_));
    Write(pStream, str, static_cast<unsigned long>(strlen_));

    if (strlen_ >= buflen)
        return;

    const uint8_t b = 0;

    const size_t count = buflen - strlen_;

    for (size_t i = 0; i < count; ++i)
        Write(pStream, &b, 1);
}
#endif



void webmmux::Write1UTF8(
    webmmux::EbmlStream* pStream,
    const wchar_t* str)
{
    assert(pStream);
    assert(str);

    const int64_t cb = wcstombs(0, str, 0);
    assert(cb > 0);

    char* const buf = (char*)malloc(cb + 1);

    const int64_t n = wcstombs(buf, str, cb + 1);

    assert(n == cb);
    assert(n > 0);
    assert(n <= 255);

#ifdef _DEBUG
    const size_t strlen_ = strlen(buf);
    assert(strlen_ == n);
    assert(buf[n] == '\0');
#endif

    const unsigned char size = static_cast<unsigned char>(n);

    Write1UInt(pStream, size);
    Write(pStream, buf, size);
}


void webmmux::Serialize(
    webmmux::EbmlStream* pStream,
    const uint8_t* p,
    const uint8_t* q)
{
    assert(pStream);
    assert(p);
    assert(q);
    assert(q >= p);

    while (q != p)
    {
        --q;

        uint32_t cbWritten;

        const int res = pStream->Write(q, 1, &cbWritten);
        assert(res);
        assert(cbWritten == 1);
    }
}


void webmmux::Serialize(
    webmmux::EbmlStream* pStream,
    const void* buf,
    uint32_t len)
{
    assert(buf);

    const uint8_t* const p = static_cast<const unsigned char*>(buf);
    const uint8_t* const q = p + len;

    Serialize(pStream, p, q);
}
