// Copyright (c) 2011 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include <stdio.h>

#include "mkvreader.hpp"
#include "mkvparser.hpp"

#include "mkvmuxer.hpp"
#include "mkvwriter.hpp"
#include "mkvmuxerutil.hpp"

#include <string.h>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage: stream_switch <input file> <output file>\n");
    return -1;
  }

  mkvparser::MkvReader reader;

  if (reader.Open(argv[1])) {
    printf("\n Filename is invalid or error while opening.\n");
    return -1;
  }

  long long pos = 0;
  mkvparser::EBMLHeader ebmlHeader;
  ebmlHeader.Parse(&reader, pos);

  mkvparser::Segment* pSegment;
  long long ret = mkvparser::Segment::CreateInstance(&reader, pos, pSegment);
  if (ret) {
    printf("\n Segment::CreateInstance() failed.");
    return -1;
  }

  ret  = pSegment->Load();
  if (ret < 0) {
    printf("\n Segment::Load() failed.");
    return -1;
  }

  mkvmuxer::MkvWriter writer;

  if (!writer.Open(argv[2])) {
    printf("\n Filename is invalid or error while opening.\n");
    return -1;
  }

  if (!WriteEbmlHeader(&writer)) {
    printf("\n Could not write EBML header.\n");
    return -1;
  }

  mkvmuxer::Segment segment(&writer);

  // Set SegmentInfo
  const mkvparser::SegmentInfo* const pSegmentInfo = pSegment->GetInfo();
  const long long timeCodeScale = pSegmentInfo->GetTimeCodeScale();

  mkvmuxer::SegmentInfo* info = segment.GetSegmentInfo();
  info->timecode_scale(timeCodeScale);
  info->writing_app("stream_switch");

  // Set Tracks
  enum { VIDEO_TRACK = 1, AUDIO_TRACK = 2 };
  const mkvparser::Tracks* pTracks = pSegment->GetTracks();
  unsigned long i = 0;
  const unsigned long j = pTracks->GetTracksCount();
  unsigned long long vid_track;
  unsigned long long aud_track;

  // Swicth the streams.
  i = j;

  while (i != 0) {
    const mkvparser::Track* const pTrack = pTracks->GetTrackByIndex(i-1);
    --i;

    if (pTrack == NULL)
      continue;

    const long long trackType = pTrack->GetType();

    if (trackType == VIDEO_TRACK) {
      const mkvparser::VideoTrack* const pVideoTrack =
        static_cast<const mkvparser::VideoTrack*>(pTrack);

      const long long width =  pVideoTrack->GetWidth();
      const long long height = pVideoTrack->GetHeight();
      //const double rate = pVideoTrack->GetFrameRate();

      vid_track = segment.AddVideoTrack(static_cast<int>(width), static_cast<int>(height));
      if (!vid_track) {
        printf("\n Could not add video track.\n");
        return -1;
      }
    }

    if (trackType == AUDIO_TRACK) {
      const mkvparser::AudioTrack* const pAudioTrack =
        static_cast<const mkvparser::AudioTrack*>(pTrack);

      const long long channels =  pAudioTrack->GetChannels();
      //const long long bitDepth = pAudioTrack->GetBitDepth();
      const double sampleRate = pAudioTrack->GetSamplingRate();
      size_t private_size;
      const unsigned char* private_data =
        pAudioTrack->GetCodecPrivate(private_size);

      aud_track = segment.AddAudioTrack(static_cast<int>(sampleRate), static_cast<int>(channels));
      if (!aud_track) {
        printf("\n Could not add audio track.\n");
        return -1;
      }

      mkvmuxer::Track* audio = segment.GetTrackByNumber(aud_track);
      if (!audio) {
        printf("\n Could not get audio track.\n");
        return -1;
      }

      if (!audio->SetCodecPrivate(private_data, private_size)) {
        printf("\n Could not add audio private data.\n");
        return -1;
      }
    }
  }

  // Write clusters
  const unsigned long clusterCount = pSegment->GetCount();
  if (clusterCount == 0) {
    printf("\t\tSegment has no clusters.\n");
    delete pSegment;
    return -1;
  }

  unsigned char* data = NULL;
  int data_len = 0;

  const mkvparser::Cluster* pCluster = pSegment->GetFirst();

  while ((pCluster != NULL) && !pCluster->EOS()) {

    const mkvparser::BlockEntry* pBlockEntry = pCluster->GetFirst();

    while ((pBlockEntry != NULL) && !pBlockEntry->EOS()) {
      const mkvparser::Block* const pBlock  = pBlockEntry->GetBlock();
      const long long trackNum = pBlock->GetTrackNumber();
      const mkvparser::Track* const pTrack =
        pTracks->GetTrackByNumber(static_cast<unsigned long>(trackNum));
      const long long trackType = pTrack->GetType();
      const int frameCount = pBlock->GetFrameCount();
      const long long time_ns = pBlock->GetTime(pCluster);
      const bool is_key = pBlock->IsKey();

      for (int i = 0; i < frameCount; ++i) {
        const mkvparser::Block::Frame& theFrame = pBlock->GetFrame(i);
        const long size = theFrame.len;

        if (theFrame.len > data_len) {
          delete [] data;
          data = new unsigned char[theFrame.len];
          if (!data)
            return -1;
          data_len = theFrame.len;
        }

        if (theFrame.Read(&reader, data))
          return -1;

        unsigned long long track_num = vid_track;
        if (trackType == AUDIO_TRACK)
          track_num = aud_track;

        if (!segment.AddFrame(data, theFrame.len, track_num, time_ns, is_key)) {
          printf("\n Could not add frame.\n");
          return -1;
        }
      }

      pBlockEntry = pCluster->GetNext(pBlockEntry);
    }

    pCluster = pSegment->GetNext(pCluster);
  }

  segment.Finalize();

  if (data)
    delete [] data;

  delete pSegment;
  return 0;
}

