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

static void Usage() {
  printf("Usage: sample_muxer -i input -o output [options]\n");
  printf("\n");
  printf("Main options:\n");
  printf("-h | -?                     show help\n");
  printf("-video <int>                >0 outputs video\n");
  printf("-audio <int>                >0 outputs audio\n");
  printf("-live <int>                 >0 puts the muxer into live mode\n");    
  printf("                            0 puts the muxer into file mode\n");
  printf("-output_cues <int>          >0 outputs cues element\n");
  printf("-cues_on_video_track <int>  >0 outputs cues on video track\n");
  printf("                            0 outputs cues on audio track\n");
  printf("-max_cluster_duration <double> in seconds\n");
  printf("-max_cluster_size <int>     in bytes\n");
  printf("\n");
  printf("Video options:\n");
  printf("-display_width <int>        Display width in pixels\n");
  printf("-display_height <int>       Display height in pixels\n");
  printf("-stereo_mode <int>          3D video mode\n");
  printf("\n");
  printf("Cues options:\n");
  printf("-output_cues_block_number <int> >0 outputs cue block number\n");
}

int main(int argc, char* argv[]) {
  char* input = NULL;
  char* output = NULL;

  // Segment variables
  bool output_video = true;
  bool output_audio = true;
  bool live_mode = false;
  bool output_cues = true;
  bool cues_on_video_track = true;
  mkvmuxer::uint64 max_cluster_duration = 0;
  mkvmuxer::uint64 max_cluster_size = 0;

  bool output_cues_block_number = true;

  unsigned long long display_width = 0;
  unsigned long long display_height = 0;
  unsigned long long stereo_mode = 0;

  for (int i=1; i<argc; ++i) {
    char* end;

    if ( (!strcmp("-h", argv[i])) || (!strcmp("-?", argv[i])) ) {
      Usage();
      return 0;
    } else if (!strcmp("-i", argv[i])) {
      input = argv[++i];
    } else if (!strcmp("-o", argv[i])) {
      output = argv[++i];
    } else if (!strcmp("-video", argv[i])) {
      output_video = strtol(argv[++i], &end, 10) == 0 ? false : true;
    } else if (!strcmp("-audio", argv[i])) {
      output_audio = strtol(argv[++i], &end, 10) == 0 ? false : true;
    } else if (!strcmp("-live", argv[i])) {
      live_mode = strtol(argv[++i], &end, 10) == 0 ? false : true;
    } else if (!strcmp("-output_cues", argv[i])) {
      output_cues = strtol(argv[++i], &end, 10) == 0 ? false : true;
    } else if (!strcmp("-cues_on_video_track", argv[i])) {
      cues_on_video_track = strtol(argv[++i], &end, 10) == 0 ? false : true;
    } else if (!strcmp("-max_cluster_duration", argv[i])) {
      const double seconds = strtod(argv[++i], &end);
      max_cluster_duration =
        static_cast<unsigned long long>(seconds * 1000000000.0);
    } else if (!strcmp("-max_cluster_size", argv[i])) {
      max_cluster_size = strtol(argv[++i], &end, 10);
    } else if (!strcmp("-display_width", argv[i])) {
      display_width = strtol(argv[++i], &end, 10);
    } else if (!strcmp("-display_height", argv[i])) {
      display_height = strtol(argv[++i], &end, 10);
    } else if (!strcmp("-stereo_mode", argv[i])) {
      stereo_mode = strtol(argv[++i], &end, 10);
    } else if (!strcmp("-output_cues_block_number", argv[i])) {
      output_cues_block_number =
        strtol(argv[++i], &end, 10) == 0 ? false : true;
    }
  }

  if (input == NULL || output == NULL) {
    Usage();
    return 0;
  }

  // Get parser header info
  mkvparser::MkvReader reader;

  if (reader.Open(input)) {
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

  const mkvparser::SegmentInfo* const pSegmentInfo = pSegment->GetInfo();
  const long long timeCodeScale = pSegmentInfo->GetTimeCodeScale();

  // Set muxer header info
  mkvmuxer::MkvWriter writer;

  if (!writer.Open(output)) {
    printf("\n Filename is invalid or error while opening.\n");
    return -1;
  }

  if (!WriteEbmlHeader(&writer)) {
    printf("\n Could not write EBML header.\n");
    return -1;
  }

  // Set Segment element attributes
  mkvmuxer::Segment segment(&writer);
  if (live_mode)
    segment.mode(mkvmuxer::Segment::kLive);
  else
    segment.mode(mkvmuxer::Segment::kFile);

  if (max_cluster_duration > 0)
    segment.max_cluster_duration(max_cluster_duration);
  if (max_cluster_size > 0)
    segment.max_cluster_size(max_cluster_size);
  segment.OutputCues(output_cues);

  // Set SegmentInfo element attributes
  mkvmuxer::SegmentInfo* info = segment.GetSegmentInfo();
  info->timecode_scale(timeCodeScale);
  info->writing_app("sample_muxer");
  
  // Set Tracks element attributes
  enum { VIDEO_TRACK = 1, AUDIO_TRACK = 2 };
  const mkvparser::Tracks* pTracks = pSegment->GetTracks();
  unsigned long i = 0;
  unsigned long long vid_track = 0; // no track added
  unsigned long long aud_track = 0; // no track added

  while (i != pTracks->GetTracksCount()) {
    const mkvparser::Track* const pTrack = pTracks->GetTrackByIndex(i++);

    if (pTrack == NULL)
      continue;

    // TODO: Add support for language to parser.
    const char* track_name = pTrack->GetNameAsUTF8();

    const long long trackType = pTrack->GetType();

    if (trackType == VIDEO_TRACK && output_video) {
      // Get the video track from the parser
      const mkvparser::VideoTrack* const pVideoTrack =
        static_cast<const mkvparser::VideoTrack*>(pTrack);
      const long long width =  pVideoTrack->GetWidth();
      const long long height = pVideoTrack->GetHeight();
      
      // Add the video track to the muxer
      vid_track = segment.AddVideoTrack(static_cast<int>(width),
                                        static_cast<int>(height));
      if (!vid_track) {
        printf("\n Could not add video track.\n");
        return -1;
      }

      mkvmuxer::VideoTrack* video =
        static_cast<mkvmuxer::VideoTrack*>(
            segment.GetTrackByNumber(vid_track));
      if (!video) {
        printf("\n Could not get video track.\n");
        return -1;
      }

      if (track_name)
        video->name(track_name);

      if (display_width > 0)
        video->display_width(display_width);
      if (display_height > 0)
        video->display_height(display_height);
      if (stereo_mode > 0)
        video->SetStereoMode(stereo_mode);

      const double rate = pVideoTrack->GetFrameRate();
      if (rate > 0.0) {
        video->frame_rate(rate);
      }
    }
    else if (trackType == AUDIO_TRACK && output_audio) {
      // Get the audio track from the parser
      const mkvparser::AudioTrack* const pAudioTrack =
        static_cast<const mkvparser::AudioTrack*>(pTrack);
      const long long channels =  pAudioTrack->GetChannels();
      const double sampleRate = pAudioTrack->GetSamplingRate();

      // Add the audio track to the muxer
      aud_track = segment.AddAudioTrack(static_cast<int>(sampleRate),
                                        static_cast<int>(channels));
      if (!aud_track) {
        printf("\n Could not add audio track.\n");
        return -1;
      }

      mkvmuxer::AudioTrack* audio =
        static_cast<mkvmuxer::AudioTrack*>(
            segment.GetTrackByNumber(aud_track));
      if (!audio) {
        printf("\n Could not get audio track.\n");
        return -1;
      }

      if (track_name)
        audio->name(track_name);

      size_t private_size;
      const unsigned char* private_data =
        pAudioTrack->GetCodecPrivate(private_size);
      if (private_size > 0) {
        if (!audio->SetCodecPrivate(private_data, private_size)) {
          printf("\n Could not add audio private data.\n");
          return -1;
        }
      }

      const long long bitDepth = pAudioTrack->GetBitDepth();
      if (bitDepth > 0)
        audio->bit_depth(bitDepth);
    }
  }

  // Set Cues element attributes
  mkvmuxer::Cues* cues = segment.GetCues();
  cues->output_block_number(output_cues_block_number);
  if (cues_on_video_track) {
    if (vid_track)
      segment.CuesTrack(vid_track);
  } else {
    if (aud_track)
      segment.CuesTrack(aud_track);
  }

  // Write clusters
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

      if ( (trackType == AUDIO_TRACK && output_audio) ||
          (trackType == VIDEO_TRACK && output_video) ) {
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

          if (!segment.AddFrame(data,
            theFrame.len,
            track_num,
            time_ns,
            is_key)) {
              printf("\n Could not add frame.\n");
              return -1;
          }
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



