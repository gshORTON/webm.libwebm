// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#pragma once
#include <iostream>


namespace EbmlIO
{

	//TODO: DW is there an issue with char potentially being different sizes on different systems?
	typedef std::basic_iostream<unsigned char> EbmlStream;

    class File
    {
        File(const File&);
        File& operator=(const File&);

    public:

        File();
        ~File();

		void SetStream(EbmlIO::EbmlStream*);
        EbmlIO::EbmlStream* GetStream() const;

        int SetSize(long long);

		long long SetPosition(long long, std::ios_base::seekdir = std::ios_base::beg);
        long long GetPosition() const;

        void Write(const void*, unsigned long);

        void Serialize8UInt(long long);
        void Serialize4UInt(unsigned long);
        void Serialize2UInt(unsigned short);
        void Serialize1UInt(unsigned char);
        void Serialize2SInt(short);
        void Serialize4Float(float);

        void WriteID4(unsigned long);
        void WriteID3(unsigned long);
        void WriteID2(unsigned short);
        void WriteID1(unsigned char);

        unsigned long ReadID4();

        void Write8UInt(long long);
        void Write4UInt(unsigned long);
        void Write2UInt(unsigned short);
        void Write1UInt(unsigned char);

        void Write1String(const char*);
        //void Write1String(const char* str, size_t len);
        void Write1UTF8(const wchar_t*);

    private:
		EbmlIO::EbmlStream* m_pStream;

    };

    int SetSize(EbmlIO::EbmlStream*, long long);
	long long SetPosition(EbmlIO::EbmlStream*, long long, std::ios_base::seekdir);

    void Serialize(EbmlIO::EbmlStream*, const unsigned char*, const unsigned char*);
    void Serialize(EbmlIO::EbmlStream*, const void*, unsigned long);
    void Write(EbmlIO::EbmlStream*, const void*, unsigned long);

    void WriteID4(EbmlIO::EbmlStream*, unsigned long);
    void WriteID3(EbmlIO::EbmlStream*, unsigned long);
    void WriteID2(EbmlIO::EbmlStream*, unsigned short);
    void WriteID1(EbmlIO::EbmlStream*, unsigned char);

    unsigned long ReadID4(EbmlIO::EbmlStream*);

    void Write8UInt(EbmlIO::EbmlStream*, long long);
    void Write4UInt(EbmlIO::EbmlStream*, unsigned long);
    void Write2UInt(EbmlIO::EbmlStream*, unsigned short);
    void Write1UInt(EbmlIO::EbmlStream*, unsigned char);

    void Write1String(EbmlIO::EbmlStream*, const char*);
    void Write1String(EbmlIO::EbmlStream*, const char*, size_t);
    void Write1UTF8(EbmlIO::EbmlStream*, const wchar_t*);

} //end namespace EbmlIO
