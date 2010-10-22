// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#pragma once
//#include <inttypes.h>
#include "webmmuxebmlio.hpp"
#include "webmmuxstreamvideo.hpp"
#include "webmmuxstreamaudio.hpp"
#include <list>

#include <iostream>

namespace webmmux
{

class Context
{
   Context(const Context&);
   Context& operator=(const Context&);

public:

   EbmlIO::File m_file;
   std::wstring m_writing_app;

   Context();
   ~Context();

   void GetVersion(int& major, int& minor, int& build, int& revision);

   void SetVideoStream(StreamVideo*);

   //TODO: this needs to liberalized to handle multiple audio streams.
   void SetAudioStream(StreamAudio*);

   void Open(std::basic_iostream<unsigned char>*);
   void Close();

    void NotifyVideoFrame(StreamVideo*, StreamVideo::VideoFrame*);
    int NotifyVideoEOS(StreamVideo*);
    void FlushVideo(StreamVideo*);
    bool WaitVideo() const;

    void NotifyAudioFrame(StreamAudio*, StreamAudio::AudioFrame*);
    int NotifyAudioEOS(StreamAudio*);
    void FlushAudio(StreamAudio*);
    bool WaitAudio() const;

    unsigned long GetTimecodeScale() const;
    unsigned long GetTimecode() const;  //of frame most recently written to file

private:

   StreamVideo* m_pVideo;
   StreamAudio* m_pAudio;  //TODO: accept multiple audio streams

   void Final();

   void WriteEbmlHeader();

   void InitSegment();
   void FinalSegment();

   void InitFirstSeekHead();
   void FinalFirstSeekHead();
   void WriteSeekEntry(unsigned long, long long);

   void InitInfo();
   void FinalInfo();

   void WriteTrack();

   long long m_segment_pos;
   long long m_first_seekhead_pos;
   //long long m_second_seekhead_pos;
   long long m_info_pos;
   long long m_track_pos;
   long long m_cues_pos;
   long long m_duration_pos;
   const unsigned long m_timecode_scale;  //TODO: video vs. audio
   unsigned long m_max_timecode;  //unscaled

    struct Keyframe
    {
        unsigned long m_timecode;    //unscaled
        unsigned long m_block;       //1-based number of block within cluster

        Keyframe();   //needed for std::list
        Keyframe(unsigned long timecode, unsigned long block);
    };

    struct Cluster
    {
        //absolute pos within file (NOT offset relative to segment)
        long long m_pos;

        unsigned long m_timecode;

        typedef std::list<Keyframe> keyframes_t;
        keyframes_t m_keyframes;
    };

   typedef std::list<Cluster> clusters_t;
   clusters_t m_clusters;

   //void WriteSecondSeekHead();
   void WriteCues();
   //void FinalClusters(long long pos);

    //bool ReadyToCreateNewClusterVideo(const StreamVideo::VideoFrame&) const;
    void CreateNewCluster(const StreamVideo::VideoFrame*);
    void CreateNewClusterAudioOnly();

    void WriteVideoFrame(Cluster&, unsigned long&);
    void WriteAudioFrame(Cluster&, unsigned long&);

    void WriteCuePoints(const Cluster&);
    void WriteCuePoint(const Cluster&, const Keyframe&);

    //EOS can happen either because we receive a notification from the stream,
    //or because the graph was stopped (before reaching end-of-stream proper).
    //The following flags are use to keep track of whether we've seen
    //EOS already.

    bool m_bEOSVideo;
    bool m_bEOSAudio;
    int m_cEOS;
    int EOS(Stream*);

};


}  //end namespace webmmux
