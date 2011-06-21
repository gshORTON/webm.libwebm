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

int GetCodedUIntSize(uint64 value) {

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

int GetUIntSize(uint64 value) {

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

uint64 EbmlElementSize(uint64 type, uint64 value, bool master) {
  // Size of EBML ID
  int ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += GetUIntSize(value);

  // Size of Datasize
  if (!master)
    ebml_size++;

  return ebml_size;
}

uint64 EbmlElementSize(uint64 type, float value, bool master) {
  // Size of EBML ID
  uint64 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += sizeof(value);

  // Size of Datasize
  if (!master)
    ebml_size++;

  return ebml_size;
}

uint64 EbmlElementSize(uint64 type, const char* value, bool master) {
  assert(value != NULL);

  // Size of EBML ID
  uint64 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += strlen(value);

  // Size of Datasize
  if (!master)
    ebml_size++;

  return ebml_size;
}

uint64 EbmlElementSize(uint64 type,
                       const uint8* value,
                       uint64 size,
                       bool master) {
  assert(value != NULL);

  // Size of EBML ID
  uint64 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += size;

  // Size of Datasize
  if (!master)
    ebml_size += GetCodedUIntSize(size);

  return ebml_size;
}

int SerializeInt(
    IMkvWriter* writer,
    int64 value,
    int size) {
  assert(writer);
  assert(size >= 0);
  assert(size <= 8);

  for (int i = 1; i <= size; ++i) {
    const int byte_count = size - i;
    const int bit_count = byte_count * 8;

    const int64 bb = value >> bit_count;
    const uint8 b = static_cast<uint8>(bb);

    const int status = writer->Write(&b, 1);

    if (status < 0)
      return status;
  }

  return 0;
}

int SerializeFloat(IMkvWriter* writer, float f) {
  assert(writer);
  const unsigned long& val = reinterpret_cast<const unsigned long&>(f);

  for (int i = 1; i <= 4; ++i) {
    const int byte_count = 4 - i;
    const int bit_count = byte_count * 8;

    const unsigned long bb = val >> bit_count;
    const uint8 b = static_cast<uint8>(bb);

    const int status = writer->Write(&b, 1);

    if (status < 0)
      return status;
  }

  return 0;
}

int WriteUInt(IMkvWriter* writer, uint64 value) {
  assert(writer);
  int size = GetCodedUIntSize(value);

  return WriteUIntSize(writer, value, size);
}

int WriteUIntSize(IMkvWriter* writer, uint64 value, int size) {
  assert(writer);
  assert(size >= 0);

  if (size > 0) {
    assert(size <= 8);

    const uint64 bit = 1LL << (size * 7);
    assert(value <= (bit - 2));

    value |= bit;
  } else {
    size = 1;
    int64 bit;

    for (;;) {
      bit = 1LL << (size * 7);
      const uint64 max = bit - 2;

      if (value <= max)
        break;

      ++size;
    }

    assert(size <= 8);
    value |= bit;
  }

  return SerializeInt(writer, value, size);
}

int WriteID(IMkvWriter* writer, uint64 type) {
  assert(writer);
  const int size = GetUIntSize(type);

  return SerializeInt(writer, type, size);
}

bool WriteEbmlMasterElement(IMkvWriter* writer, uint64 type, uint64 size) {
  assert(writer);

  if (WriteID(writer, type))
    return false;

  if (WriteUInt(writer, size))
    return false;

  return true;
}

bool WriteEbmlElement(IMkvWriter* writer, uint64 type, uint64 value) {
  assert(writer);

  if (WriteID(writer, type))
    return false;

  const uint64 size = GetUIntSize(value);
  if (WriteUInt(writer, size))
    return false;

  if (SerializeInt(writer, value, static_cast<int>(size)))
    return false;

  return true;
}

bool WriteEbmlElement(IMkvWriter* writer, uint64 type, float value) {
  assert(writer);

  if (WriteID(writer, type))
    return false;

  if (WriteUInt(writer, 4))
    return false;

  if (SerializeFloat(writer, value))
    return false;

  return true;
}

bool WriteEbmlElement(IMkvWriter* writer, uint64 type, const char* value) {
  assert(writer);
  assert(value != NULL);

  if (WriteID(writer, type))
    return false;

  const int length = strlen(value);
  if (WriteUInt(writer, length))
    return false;

  if (writer->Write(value, length))
    return false;

  return true;
}

bool WriteEbmlElement(IMkvWriter* writer,
                      uint64 type,
                      const uint8* value,
                      uint64 size) {
  assert(writer);
  assert(value != NULL);
  assert(size > 0);

  if (WriteID(writer, type))
    return false;

  if (WriteUInt(writer, size))
    return false;

  if (writer->Write(value, static_cast<unsigned long>(size)))
    return false;

  return true;
}

uint64 WriteSimpleBlock(IMkvWriter* writer,
                        const uint8* data,
                        uint64 length,
                        char track_number,
                        short timecode,
                        bool is_key) {
  assert(writer);
  assert(data != NULL);
  assert(length > 0);
  assert(track_number > 0);
  assert(timecode >= 0);

  if (WriteID(writer, kMkvSimpleBlock))
    return 0;

  const int size = static_cast<int>(length) + 4;
  if (WriteUInt(writer, size))
    return 0;

  if (WriteUInt(writer, static_cast<uint64>(track_number)))
    return 0;

  if (SerializeInt(writer, static_cast<uint64>(timecode), 2))
    return 0;

  uint64 flags = 0;
  if(is_key)
    flags |= 0x80;

  if (SerializeInt(writer, flags, 1))
    return 0;

  if (writer->Write(data, static_cast<unsigned long>(length)))
    return 0;

  const uint64 element_size =
    GetUIntSize(kMkvSimpleBlock) + GetCodedUIntSize(length) + 4 + length;

  return element_size;
}

uint64 WriteVoidElement(IMkvWriter* writer, uint64 size) {
  // Subtract one for the void ID and the coded size.
  uint64 void_entry_size = size - 1 - GetCodedUIntSize(size-1);
  uint64 void_size = EbmlElementSize(kMkvVoid, void_entry_size, true) +
                     void_entry_size;
  assert(void_size == size);

  const int64 payload_position = writer->Position();
  if (payload_position < 0)
    return 0;

  if (WriteID(writer, kMkvVoid))
    return 0;

  if (WriteUInt(writer, void_entry_size))
    return 0;

  uint8 value = 0;
  for (int i=0; i<static_cast<int>(void_entry_size); ++i) {
    if (writer->Write(&value, 1))
      return 0;
  }

  const int64 stop_position = writer->Position();
  if (stop_position < 0)
    return 0;
  assert(stop_position - payload_position == static_cast<int64>(void_size));

  return void_size;
}

void GetVersion(int& major, int& minor, int& build, int& revision)
{
    major = 0;
    minor = 0;
    build = 0;
    revision = 1;
}

}  // namespace mkvmuxer
