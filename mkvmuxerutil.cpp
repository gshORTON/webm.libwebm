// Copyright (c) 2011 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include "mkvmuxerutil.hpp"
#include "mkvwriter.hpp"
#include "webmids.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <new>
#include <string.h>
#include <time.h>

namespace mkvmuxer {

int GetCodedUIntSize(unsigned long long value) {

  if (value < 0x000000000000007FULL)
    return 1;
  else if (value < 0x0000000000003FFFULL)
    return 2;
  else if (value < 0x00000000001FFFFFULL)
    return 3;
  else if (value < 0x000000000FFFFFFFULL)
    return 4;
  else if (value < 0x00000007FFFFFFFFULL)
    return 5;
  else if (value < 0x000003FFFFFFFFFFULL)
    return 6;
  else if (value < 0x0001FFFFFFFFFFFFULL)
    return 7;
  return 8;
}

int GetUIntSize(unsigned long long value) {

  if (value < 0x0000000000000100ULL)
    return 1;
  else if (value < 0x0000000000010000ULL)
    return 2;
  else if (value < 0x0000000001000000ULL)
    return 3;
  else if (value < 0x0000001000000000ULL)
    return 4;
  else if (value < 0x0000100000000000ULL)
    return 5;
  else if (value < 0x0010000000000000ULL)
    return 6;
  else if (value < 0x1000000000000000ULL)
    return 7;
  return 8;
}

unsigned long long EbmlElementSize(unsigned long long type,
                                   unsigned long long value,
                                   bool master) {
  // Size of EBML ID
  int ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += GetUIntSize(value);

  // Size of Datasize
  if (!master)
    ebml_size++;

  return ebml_size;
}

unsigned long long EbmlElementSize(unsigned long long type,
                                   float value,
                                   bool master) {
  // Size of EBML ID
  unsigned long long ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += 4;

  // Size of Datasize
  if (!master)
    ebml_size++;

  return ebml_size;
}

unsigned long long EbmlElementSize(unsigned long long type,
                                   const char* value,
                                   bool master) {
  assert(value != NULL);

  // Size of EBML ID
  unsigned long long ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += strlen(value);

  // Size of Datasize
  if (!master)
    ebml_size++;

  return ebml_size;
}

unsigned long long EbmlElementSize(unsigned long long type,
                                   const unsigned char* value,
                                   unsigned long long size,
                                   bool master) {
  assert(value != NULL);

  // Size of EBML ID
  unsigned long long ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += size;

  // Size of Datasize
  if (!master)
    ebml_size += GetCodedUIntSize(size);

  return ebml_size;
}

int SerializeInt(
    IMkvWriter* pWriter,
    long long value,
    int size) {
  assert(pWriter);
  assert(size >= 0);
  assert(size <= 8);

  for (int i = 1; i <= size; ++i) {
    const int byte_count = size - i;
    const int bit_count = byte_count * 8;

    const long long bb = value >> bit_count;
    const unsigned char b = static_cast<unsigned char>(bb);

    const int status = pWriter->Write(&b, 1);

    if (status < 0)
      return status;
  }

  return 0;
}

int SerializeFloat(IMkvWriter* pWriter, float f) {
  assert(pWriter);
  //COMPILE_ASSERT(sizeof(f) == 4, size_of_float_is_not_4_bytes);

  const unsigned long& val = reinterpret_cast<const unsigned long&>(f);

  for (int i = 1; i <= 4; ++i) {
    const int byte_count = 4 - i;
    const int bit_count = byte_count * 8;

    const unsigned long bb = val >> bit_count;
    const unsigned char b = static_cast<unsigned char>(bb);

    const int status = pWriter->Write(&b, 1);

    if (status < 0)
      return status;
  }

  return 0;
}

int WriteUInt(
    IMkvWriter* pWriter,
    unsigned long long value) {
  assert(pWriter);
  assert(value >= 0);
  int size = GetCodedUIntSize(value);

  return WriteUIntSize(pWriter, value, size);
}

int WriteUIntSize(
    IMkvWriter* pWriter,
    unsigned long long value,
    int size) {
  assert(pWriter);
  assert(value >= 0);
  assert(size >= 0);

  if (size > 0) {
    assert(size <= 8);

    const unsigned long long bit = 1LL << (size * 7);
    assert(value <= (bit - 2));

    value |= bit;
  } else {
    size = 1;
    long long bit;

    for (;;) {
      bit = 1LL << (size * 7);
      const unsigned long long max = bit - 2;

      if (value <= max)
        break;

      ++size;
    }

    assert(size <= 8);
    value |= bit;
  }

  return SerializeInt(pWriter, value, size);
}

int WriteID(IMkvWriter* pWriter, unsigned long long type) {
  assert(pWriter);
  const int size = GetUIntSize(type);

  return SerializeInt(pWriter, type, size);
}

bool WriteEbmlMasterElement(IMkvWriter* pWriter,
                            unsigned long long type,
                            unsigned long long size) {
  assert(pWriter);

  if (WriteID(pWriter, type))
    return false;

  if (WriteUInt(pWriter, size))
    return false;

  return true;
}

bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      unsigned long long value) {
  assert(pWriter);

  if (WriteID(pWriter, type))
    return false;

  const unsigned long long size = GetUIntSize(value);
  if (WriteUInt(pWriter, size))
    return false;

  if (SerializeInt(pWriter, value, static_cast<int>(size)))
    return false;

  return true;
}

bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      float value) {
  assert(pWriter);

  if (WriteID(pWriter, type))
    return false;

  if (WriteUInt(pWriter, 4))
    return false;

  if (SerializeFloat(pWriter, value))
    return false;

  return true;
}

bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      const char* value) {
  assert(pWriter);
  assert(value != NULL);

  if (WriteID(pWriter, type))
    return false;

  const int length = strlen(value);
  if (WriteUInt(pWriter, length))
    return false;

  if (pWriter->Write(value, length))
    return false;

  return true;
}

bool WriteEbmlElement(IMkvWriter* pWriter,
                      unsigned long long type,
                      const unsigned char* value,
                      unsigned long long size) {
  assert(pWriter);
  assert(value != NULL);
  assert(size > 0);

  if (WriteID(pWriter, type))
    return false;

  if (WriteUInt(pWriter, size))
    return false;

  if (pWriter->Write(value, static_cast<unsigned long>(size)))
    return false;

  return true;
}

unsigned long long WriteSimpleBlock(IMkvWriter* pWriter,
                      const unsigned char* data,
                      unsigned long long length,
                      char track_number,
                      short timestamp,
                      bool is_key) {
  assert(pWriter);
  assert(data != NULL);
  assert(length > 0);
  assert(track_number > 0 && track_number < 128);
  assert(timestamp >= 0);

  if (WriteID(pWriter, kMkvSimpleBlock))
    return 0;

  const int size = static_cast<int>(length) + 4;
  if (WriteUInt(pWriter, size))
    return 0;

  if (WriteUInt(pWriter, static_cast<unsigned long long>(track_number)))
    return 0;

  if (SerializeInt(pWriter, static_cast<unsigned long long>(timestamp), 2))
    return 0;

  unsigned long long flags = 0;
  if(is_key)
    flags |= 0x80;

  if (SerializeInt(pWriter, flags, 1))
    return 0;

  if (pWriter->Write(data, static_cast<unsigned long>(length)))
    return 0;

  const unsigned long long element_size =
    GetUIntSize(kMkvSimpleBlock) + GetCodedUIntSize(length) + 4 + length;

  return element_size;
}

void GetVersion(int& major, int& minor, int& build, int& revision)
{
    major = 0;
    minor = 0;
    build = 0;
    revision = 1;
}

}  // namespace mkvmuxer
