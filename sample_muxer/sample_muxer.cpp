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

  if (!segment.AddVideoTrack(320, 240)) {
    printf("\n Could not add video track.\n");
    return -1;
  }

  if (!segment.WriteSegmentHeader()) {
    printf("\n Could not write main Segment header.\n");
    return -1;
  }

  return 0;
}

