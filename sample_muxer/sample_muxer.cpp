// Copyright (c) 2011 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include <stdio.h>

#include "mkvmuxer.hpp"
#include "mkvwriter.hpp"
#include "mkvmuxerutil.hpp"

#include <string.h>

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

