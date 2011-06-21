// Copyright (c) 2011 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

//#include <stdio.h>

#include <cassert>
//#include <string.h>

#include "mkvmuxer.hpp"
#include "mkvwriter.hpp"
#include "mkvmuxerutil.hpp"

//TODO(fgalligan): Change these tests into gtests.

namespace mkvmuxer
{

void test_value_u64(unsigned long long value, int expected_size) {
  const int rv_1 = mkvmuxer::GetSerializeUIntSize(value);
  //const int rv_2 = mkvmuxer::GetSerializeUIntSize2(value);
  //const int rv_3 = mkvmuxer::GetSerializeUIntSize3(value);
  assert(rv_1 == expected_size);
  //assert(rv_2 == expected_size);
  //assert(rv_3 == expected_size);
}

int test() {
  /*
  for (unsigned long long i=0; i<0x1FF; ++i) {
    const int rv_1 = mkvmuxer::GetSerializeUIntSize(i);
    test_value_u64(i, rv_1);
  }
  */

  test_value_u64(0x0000000000000000ULL, 1);
  test_value_u64(0x0000000000000001ULL, 1);
  test_value_u64(0x000000000000007EULL, 1); // 2^7 - 2
  test_value_u64(0x000000000000007FULL, 2);
  test_value_u64(0x0000000000000100ULL, 2);
  test_value_u64(0x0000000000003FFEULL, 2); // 2^14 - 2
  test_value_u64(0x0000000000003FFFULL, 3);
  test_value_u64(0x00000000001FFFFEULL, 3); // 2^21 - 2
  test_value_u64(0x00000000001FFFFFULL, 4);
  test_value_u64(0x000000000FFFFFFEULL, 4); // 2^28 - 2
  test_value_u64(0x000000000FFFFFFFULL, 5);
  test_value_u64(0x00000007FFFFFFFEULL, 5); // 2^35 - 2
  test_value_u64(0x00000007FFFFFFFFULL, 6);
  test_value_u64(0x000003FFFFFFFFFEULL, 6); // 2^42 - 2
  test_value_u64(0x000003FFFFFFFFFFULL, 7);
  test_value_u64(0x0001FFFFFFFFFFFEULL, 7); // 2^49 - 2
  test_value_u64(0x0001FFFFFFFFFFFFULL, 8);
  test_value_u64(0x00FFFFFFFFFFFFFEULL, 8); // 2^56 - 2
  //test_value_u64(0x00FFFFFFFFFFFFFFULL, 8);

  return 0;
}

/*
int main(int argc, char* argv[])
{
  using namespace mkvmuxer;

  MkvWriter writer;

  if (!writer.Open(argv[1])) {
    printf("\n Filename is invalid or error while opening.\n");
    return -1;
  }

  if (!WriteEbmlHeader(&writer)) {
    printf("\n Could not write EBML header.\n");
    return -1;
  }

  Segment segment(&writer);

  const unsigned long long vid_track = segment.AddVideoTrack(320, 240);
  if (!vid_track) {
    printf("\n Could not add video track.\n");
    return -1;
  }

  const unsigned long long aud_track = segment.AddAudioTrack(44100, 2);
  if (!aud_track) {
    printf("\n Could not add audio track.\n");
    return -1;
  }

  unsigned char video_frame[36];
  const unsigned int length = strlen("When roses are red, Bats are cool.") + 1;
  memcpy(video_frame, "When roses are red, Bats are cool.", length);

  unsigned char audio_frame[36];
  memcpy(audio_frame, "When roses are red, Bats are cool.", length);

  for (int i=0; i<300; ++i) {
    const unsigned long long timestamp = (i * 1000000000ULL) / 30ULL;
    const bool is_key = ((i+1) % 2) ? true : false;

    if (!segment.AddFrame(video_frame, length, vid_track, timestamp, is_key)) {
      printf("\n Could not add video frame #%d.\n", i);
      return -1;
    }

    if (!segment.AddFrame(audio_frame, length, aud_track, timestamp, true)) {
      printf("\n Could not add video frame #%d.\n", i);
      return -1;
    }
  }

  segment.Finalize();

  return 0;
}
*/

}  // namespace mkvmuxer