// Copyright (c) 2011 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#ifndef MKVMUXERUTIL_HPP
#define MKVMUXERUTIL_HPP

namespace mkvmuxer
{
//typedef long long          I64_t;
//typedef unsigned long long U64_t;

class IMkvWriter;

// Writes out |value| in Big Endian order. Returns 0 on success. 
int SerializeInt(IMkvWriter* writer, long long value, int size);

// Returns the size in bytes of the element. |master| must be set to true if
// the element is an Mkv master element.
// TODO: Change these functions so they are master element aware.
unsigned long long EbmlElementSize(unsigned long long type,
                                   unsigned long long value,
                                   bool master);
unsigned long long EbmlElementSize(unsigned long long type,
                                   float value,
                                   bool master);
unsigned long long EbmlElementSize(unsigned long long type,
                                   const char* value,
                                   bool master);
unsigned long long EbmlElementSize(unsigned long long type,
                                   const unsigned char* value,
                                   unsigned long long size,
                                   bool master);

// Creates an EBML coded number from |value| and writes it out. The size of
// the coded number is deteremined by the value of |value|. |value| must not
// be in a coded form. Returns 0 on success.
int WriteUInt(IMkvWriter* pWriter, unsigned long long value);

// Creates an EBML coded number from |value| and writes it out. The size of
// the coded number is deteremined by the value of |size|. |value| must not
// be in a coded form. Returns 0 on success.
int WriteUIntSize(IMkvWriter* pWriter, unsigned long long value, int size);

// Output an Mkv master element. Returns true if the element was written.
bool WriteEbmlMasterElement(IMkvWriter* pWriter,
                            unsigned long long value,
                            unsigned long long size);

// Output an Mkv non-master element. Returns true if the element was written.
bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      unsigned long long value);
bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      float value);
bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      const char* value);
bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      const unsigned char* value,
                      unsigned long long size);

unsigned long long WriteSimpleBlock(IMkvWriter* pWriter,
                      const unsigned char* data,
                      unsigned long long length,
                      char track_number,
                      short timestamp,
                      bool is_key);

unsigned long long WriteVoidElement(IMkvWriter* pWriter,
                                    unsigned long long size);

// Returns the version number of the muxer in |major|, |minor|, |build|,
// and |revision|.
void GetVersion(int& major, int& minor, int& build, int& revision);


int WriteID(IMkvWriter* pWriter, unsigned long long type);

}  //end namespace mkvmuxer

#endif //MKVMUXERUTIL_HPP
