// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#pragma once
#include "webmmuxsizedints.hpp"


namespace webmmux
{

    enum EbmlIOSeekPosition {
            EBMLIO_SEEK_BEGINNING,
            EBMLIO_SEEK_CURRENT,
            EBMLIO_SEEK_END
        };

    class EbmlStream
    {
    public:

        // Expands the size of the stream, padding junk bytes if the SetSize is longer than the current stream. Seek pointer must be unaffected. Return nonzero success.
        virtual int SetSize(uint64_t size) = 0;

        // Seeks to offset from startPos.  offset may be past end of stream, with automatic expansion of stream  newPos is a pointer to the new postion. Return nonzero success.
        virtual int Seek(int64_t offset, EbmlIOSeekPosition startPos, uint64_t* newPos = NULL) = 0;

        // Write bytesToWrite bytes from buf to stream.  bytesWritten is self-explanantory.  Return nonzero success.
        virtual int Write(void const* buf, uint32_t bytesToWrite, uint32_t* bytesWritten = NULL) = 0;

        // Read bytesToRead bytes from stream into buf.  bytesRead is self-explanatory.  Return nonzero success.
        virtual int Read(void* buf, uint32_t bytesToRead, uint32_t* bytesRead = NULL) = 0;
    };


    class File
    {
        File(const File&);
        File& operator=(const File&);

    public:

        File();
        ~File();

        void SetStream(webmmux::EbmlStream*);
        webmmux::EbmlStream* GetStream() const;

        int32_t SetSize(int64_t);

        int64_t SetPosition(int64_t, EbmlIOSeekPosition = EBMLIO_SEEK_BEGINNING );
        int64_t GetPosition() const;

        void Write(const void*, uint32_t);

        void Serialize8UInt(uint64_t);
        void Serialize4UInt(uint32_t);
        void Serialize2UInt(uint16_t);
        void Serialize1UInt(uint8_t);
        void Serialize2SInt(int16_t);
        void Serialize4Float(float);

        void WriteID4(uint32_t);
        void WriteID3(uint32_t);
        void WriteID2(uint16_t);
        void WriteID1(uint8_t);

        uint32_t ReadID4();

        void Write8UInt(uint64_t);
        void Write4UInt(uint32_t);
        void Write2UInt(uint16_t);
        void Write1UInt(uint8_t);

        void Write1String(const char*);
        //void Write1String(const int8_t* str, size_t len);
        void Write1UTF8(const wchar_t*);

    private:
        webmmux::EbmlStream* m_pStream;

    };

    int32_t SetSize(webmmux::EbmlStream*, int64_t);
    int64_t SetPosition(webmmux::EbmlStream*, int64_t, EbmlIOSeekPosition);

    void Serialize(webmmux::EbmlStream*, const unsigned char*, const unsigned char*);
    void Serialize(webmmux::EbmlStream*, const void*, uint32_t);
    void Write(webmmux::EbmlStream*, const void*, uint32_t);

    void WriteID4(webmmux::EbmlStream*, uint32_t);
    void WriteID3(webmmux::EbmlStream*, uint32_t);
    void WriteID2(webmmux::EbmlStream*, uint16_t);
    void WriteID1(webmmux::EbmlStream*, uint8_t);

    uint32_t ReadID4(webmmux::EbmlStream*);

    void Write8UInt(webmmux::EbmlStream*, uint64_t);
    void Write4UInt(webmmux::EbmlStream*, uint32_t);
    void Write2UInt(webmmux::EbmlStream*, uint16_t);
    void Write1UInt(webmmux::EbmlStream*, uint8_t);

    void Write1String(webmmux::EbmlStream*, const char*);
    void Write1String(webmmux::EbmlStream*, const char*, size_t);
    void Write1UTF8(webmmux::EbmlStream*, const wchar_t*);

} //end namespace EbmlIO
