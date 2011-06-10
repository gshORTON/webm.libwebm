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
class IMkvWriter;

// Returns a random number to be used for the Track UID.
unsigned long long MakeTrackUID();

// Creates an EBML encoded value from |value| and |size|. Returns 0 on success. 
int SerializeInt(IMkvWriter* writer, long long value, int size);

// Returns the size in bytes of the element. |master| must be set to true if
// the element is an Mkv master element.
unsigned long long EbmlElementSize(unsigned long long type,
                                   unsigned long long value,
                                   bool master);
unsigned long long EbmlElementSize(unsigned long long type,
                                   float value,
                                   bool master);
unsigned long long EbmlElementSize(unsigned long long type,
                                   const char* value,
                                   bool master);
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

// Returns the version number of the muxer in |major|, |minor|, |build|,
// and |revision|.
void GetVersion(int& major, int& minor, int& build, int& revision);

}  //end namespace mkvmuxer

#endif //MKVMUXERUTIL_HPP