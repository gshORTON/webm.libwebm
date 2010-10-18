// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

//#include <strmif.h>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <limits>
#include <malloc.h>  //_malloca
#include "webmmuxebmlio.hpp"
//#include <cstdlib>

EbmlIO::File::File() : m_pStream(0)
{
}


EbmlIO::File::~File()
{
    assert(m_pStream == 0);
}


void EbmlIO::File::SetStream(EbmlIO::EbmlStream* p)
{
    assert((m_pStream == 0) || (p == 0));
    m_pStream = p;
}


EbmlIO::EbmlStream* EbmlIO::File::GetStream() const
{
    return m_pStream;
}


int EbmlIO::File::SetSize(long long size)
{
    return EbmlIO::SetSize(m_pStream, size);
}


long long EbmlIO::File::SetPosition(
    long long pos,
	std::ios_base::seekdir origin)
{
    return EbmlIO::SetPosition(m_pStream, pos, origin);
}


long long EbmlIO::File::GetPosition() const
{
    File* const const_file = const_cast<File*>(this);
    return const_file->SetPosition(0, std::ios_base::cur);
}



//void EbmlIO::File::Serialize(
//    const void* buf,
//    unsigned long len)
//{
//    EbmlIO::Serialize(m_pStream, buf, len);
//}


void EbmlIO::File::Write(const void* buf, unsigned long cb)
{
    EbmlIO::Write(m_pStream, buf, cb);
}


void EbmlIO::File::Serialize8UInt(long long val)
{
    EbmlIO::Serialize(m_pStream, &val, 8);
}



void EbmlIO::File::Serialize4UInt(unsigned long val)
{
    EbmlIO::Serialize(m_pStream, &val, 4);
}


void EbmlIO::File::Serialize2UInt(unsigned short val)
{
    EbmlIO::Serialize(m_pStream, &val, 2);
}


void EbmlIO::File::Serialize1UInt(unsigned char val)
{
    EbmlIO::Serialize(m_pStream, &val, 1);
}


void EbmlIO::File::Serialize2SInt(short val)
{
    EbmlIO::Serialize(m_pStream, &val, 2);
}


void EbmlIO::File::Serialize4Float(float val)
{
    EbmlIO::Serialize(m_pStream, &val, 4);
}


void EbmlIO::File::WriteID4(unsigned long id)
{
    EbmlIO::WriteID4(m_pStream, id);
}


void EbmlIO::File::WriteID3(unsigned long id)
{
    EbmlIO::WriteID3(m_pStream, id);
}


void EbmlIO::File::WriteID2(unsigned short id)
{
    EbmlIO::WriteID2(m_pStream, id);
}


void EbmlIO::File::WriteID1(unsigned char id)
{
    EbmlIO::WriteID1(m_pStream, id);
}


unsigned long EbmlIO::File::ReadID4()
{
    return EbmlIO::ReadID4(m_pStream);
}


void EbmlIO::File::Write8UInt(long long val)
{
    EbmlIO::Write8UInt(m_pStream, val);
}


void EbmlIO::File::Write4UInt(unsigned long val)
{
    EbmlIO::Write4UInt(m_pStream, val);
}


void EbmlIO::File::Write2UInt(unsigned short val)
{
    EbmlIO::Write2UInt(m_pStream, val);
}


void EbmlIO::File::Write1UInt(unsigned char val)
{
    EbmlIO::Write1UInt(m_pStream, val);
}


void EbmlIO::File::Write1String(const char* str)
{
    EbmlIO::Write1String(m_pStream, str);
}


//void EbmlIO::File::Write1String(const char* str, size_t len)
//{
//    EbmlIO::Write1String(m_pStream, str, len);
//}


void EbmlIO::File::Write1UTF8(const wchar_t* str)
{
    EbmlIO::Write1UTF8(m_pStream, str);
}


int EbmlIO::SetSize(EbmlIO::EbmlStream* pStream, long long size_)
{
    assert(pStream);
    assert(size_ >= 0);

	std::streampos currentPos = pStream->tellp();
	pStream->seekp(0, std::ios_base::end);

	if (size_ > pStream->tellp())
	{
    unsigned long long size;
    size = size_ - pStream->tellp();
	long currentWidth = pStream->width();
	pStream->width(size);
	*pStream << ' ';
	pStream->width(currentWidth);
	}

	pStream->seekp(currentPos);
	return 0; //TODO: DW check for failure

//    return pStream->SetSize(size);

}


long long EbmlIO::SetPosition(
    EbmlIO::EbmlStream* pStream,
    long long move_,
	std::ios_base::seekdir origin)
{
    assert(pStream);

    long long move;
    move = move_;


    unsigned long long newpos;

	//pStream->seekg(static_cast<std::streamoff>(move.QuadPart), origin);
	pStream->seekp(static_cast<std::streamoff>(move), origin);

	if (pStream->fail())
	{
		pStream->clear();
		SetSize(pStream, move);
		pStream->seekp(std::ios_base::end);
	}

    //TODO: DW add error checking

	newpos = pStream->tellp();

    return newpos;
}


void EbmlIO::Write(
    EbmlIO::EbmlStream* pStream,
    const void* buf,
    unsigned long cb)
{
    assert(pStream);

    unsigned long cbWritten;

	//TODO: DW  see if const cast can be avoided
	unsigned char *charbuf = static_cast<unsigned char*>(const_cast<void*>(buf));
    pStream->write(charbuf, cb);
	assert(pStream->fail() == false);
	//todo: DW flush for degugging purposes
	pStream->flush();


}



void EbmlIO::Write8UInt(EbmlIO::EbmlStream* pStream, long long val)
{
    assert(val <= 0x00FFFFFFFFFFFFFE);  //0000 000x 1111 1111 ...
    val |= 0x0100000000000000;          //always write 8 unsigned chars

    Serialize(pStream, &val, 8);
}


void EbmlIO::Write4UInt(EbmlIO::EbmlStream* pStream, unsigned long val)
{
    assert(val <= 0x0FFFFFFE);  //000x 1111 1111 ...
    val |= 0x10000000;  //always write 4 unsigned chars

    Serialize(pStream, &val, 4);
}


void EbmlIO::Write2UInt(EbmlIO::EbmlStream* pStream, unsigned short val)
{
    assert(val <= 0x3FFE);  //0x11 1111 1111 1110
    val |= 0x4000;          //always write 2 unsigned chars

    Serialize(pStream, &val, 2);
}


void EbmlIO::Write1UInt(EbmlIO::EbmlStream* pStream, unsigned char val)
{
    assert(val <= 0x7E);  //x111 1110
    val |= 0x80;          //always write 1 unsigned char

    Serialize(pStream, &val, 1);
}


void EbmlIO::WriteID4(EbmlIO::EbmlStream* pStream, unsigned long id)
{
    assert(pStream);
    assert(id & 0x10000000);  //always write 4 unsigned chars
    assert(id <= 0x1FFFFFFE);

    Serialize(pStream, &id, 4);
}


unsigned long EbmlIO::ReadID4(EbmlIO::EbmlStream* pStream)
{
    assert(pStream);

    unsigned long id;

    unsigned char* const p = reinterpret_cast<unsigned char*>(&id);
    unsigned char* q = p + 4;

	pStream->seekg(pStream->tellp());  // use current put pointer for reads

    for (;;)
    {
        unsigned long cb;

		//TODO: DW clean up
		pStream->read(--q, 1);
		assert(pStream->good());
//        const HRESULT hr = pStream->Read(--q, 1, &cb);
//        assert(hr == S_OK);
//        assert(cb == 1);
//        hr;

        if (q == p)
            break;
    }

    assert(id & 0x10000000);
    assert(id <= 0x1FFFFFFE);

	pStream->seekp(pStream->tellg()); // after read, set put pointer to last read position

    return id;
}


void EbmlIO::WriteID3(EbmlIO::EbmlStream* pStream, unsigned long id)
{
    assert(pStream);
    assert(id & 0x200000);  //always write 3 unsigned chars
    assert(id <= 0x3FFFFE);

    Serialize(pStream, &id, 3);
}


void EbmlIO::WriteID2(EbmlIO::EbmlStream* pStream, unsigned short id)
{
    assert(pStream);
    assert(id & 0x4000);  //always write 2 unsigned chars
    assert(id <= 0x7FFE);

    Serialize(pStream, &id, 2);
}


void EbmlIO::WriteID1(EbmlIO::EbmlStream* pStream, unsigned char id)
{
    assert(pStream);
    assert(id & 0x80);  //always write 1 unsigned char
    assert(id <= 0xFE);

    Serialize(pStream, &id, 1);
}


void EbmlIO::Write1String(
    EbmlIO::EbmlStream* pStream,
    const char* str)
{
    assert(pStream);
    assert(str);

    const size_t size_ = strlen(str);
    assert(size_ <= 255);

    const unsigned char size = static_cast<unsigned char>(size_);

    Write1UInt(pStream, size);
    Write(pStream, str, size);
}


#if 0
void EbmlIO::Write1String(
    EbmlIO::EbmlStream* pStream,
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

    const unsigned char b = 0;

    const size_t count = buflen - strlen_;

    for (size_t i = 0; i < count; ++i)
        Write(pStream, &b, 1);
}
#endif



void EbmlIO::Write1UTF8(
    EbmlIO::EbmlStream* pStream,
    const wchar_t* str)
{
    assert(pStream);
    assert(str);

	const int cb = wcstombs(0, str, 0);
    assert(cb > 0);

    char* const buf = (char*)malloc(cb + 1);

    const int n = wcstombs(buf, str, cb + 1);
		
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


void EbmlIO::Serialize(
    EbmlIO::EbmlStream* pStream,
    const unsigned char* p,
    const unsigned char* q)
{
    assert(pStream);
    assert(p);
    assert(q);
    assert(q >= p);

    while (q != p)
    {
        --q;

        unsigned long cbWritten;

		//TODO: DW clean up
        pStream->write(q, 1);
		//pStream->put(*q);
		assert(pStream->fail() == false);
        //assert(SUCCEEDED(hr));
        //assert(cbWritten == 1);
        //hr;
    }
}


void EbmlIO::Serialize(
    EbmlIO::EbmlStream* pStream,
    const void* buf,
    unsigned long len)
{
    assert(buf);

    const unsigned char* const p = static_cast<const unsigned char*>(buf);
    const unsigned char* const q = p + len;

    Serialize(pStream, p, q);
}
