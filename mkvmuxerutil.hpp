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

// TODO Change these so the values match the WebM spec!!!!
enum MkvId {
  kMkvSegment = 0x08538067,
  kMkvSegmentInfo = 0x0549A966,
  kMkvTimeCodeScale = 0x0AD7B1,
  kMkvDuration = 0x0489,
  kMkvMuxingApp = 0x0D80,
  kMkvWritingApp = 0x1741,
  kMkvTracks = 0x0654AE6B,
  kMkvTrackEntry = 0x2E,
  kMkvTrackNumber = 0x57,
  kMkvTrackType = 0x03,
  kMkvTrackCodec = 0x06,
  kMkvVideoSettings = 0x60,
  kMkvPixelWidth = 0x30,
  kMkvPixelHeight = 0x3A,
  kMkvCluster = 0x0F43B675,
  kMkvTimeCode = 0x67,
  kMkvSimpleBlock = 0x23,
  kMkvTrackUID = 0x33C5
};

// Returns a random number to be used for the Track UID.
unsigned long long MakeTrackUID();

int GetSerializeUIntSize(unsigned long long value);

unsigned long long EbmlElementSize(unsigned long long type,
                                   unsigned long long value,
                                   bool master);
unsigned long long EbmlElementSize(unsigned long long type,
                                   float value,
                                   bool master);
unsigned long long EbmlElementSize(unsigned long long type,
                                   const char* value,
                                   bool master);

int SerializeInt(IMkvWriter* writer, long long value, int size);
int SerializeFloat(IMkvWriter* writer, float value);
int WriteUInt(IMkvWriter* writer, long long value, int size);

bool WriteEbmlMasterElement(IMkvWriter* pWriter,
                            unsigned long long value,
                            unsigned long long size);
bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      unsigned long long value);
bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      float value);
bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      const char* value);

void GetVersion(int& major, int& minor, int& build, int& revision);

}  //end namespace mkvmuxer

#endif //MKVMUXERUTIL_HPP