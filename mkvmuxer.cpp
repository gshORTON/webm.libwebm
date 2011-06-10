// Copyright (c) 2011 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include "mkvmuxer.hpp"
#include "mkvmuxerutil.hpp"
#include "webmids.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <new>
#include <string.h>
#include <time.h>

namespace mkvmuxer {

IMkvWriter::IMkvWriter() {
}

IMkvWriter::~IMkvWriter() {
}

bool WriteEbmlHeader(IMkvWriter* pWriter) {
  // Level 0
  unsigned long long size = EbmlElementSize(kMkvEBMLVersion, 1ULL, false);
  size += EbmlElementSize(kMkvEBMLReadVersion, 1ULL, false);
  size += EbmlElementSize(kMkvEBMLMaxIDLength, 4ULL, false);
  size += EbmlElementSize(kMkvEBMLMaxSizeLength, 8ULL, false);
  size += EbmlElementSize(kMkvDocType, "webm", false);
  size += EbmlElementSize(kMkvDocTypeVersion, 2ULL, false);
  size += EbmlElementSize(kMkvDocTypeReadVersion, 2ULL, false);

  if (!WriteEbmlMasterElement(pWriter, kMkvEBML, size))
    return false;

  if (!WriteEbmlElement(pWriter, kMkvEBMLVersion, 1ULL))
    return false;
  if (!WriteEbmlElement(pWriter, kMkvEBMLReadVersion, 1ULL))
    return false;
  if (!WriteEbmlElement(pWriter, kMkvEBMLMaxIDLength, 4ULL))
    return false;
  if (!WriteEbmlElement(pWriter, kMkvEBMLMaxSizeLength, 8ULL))
    return false;
  if (!WriteEbmlElement(pWriter, kMkvDocType, "webm"))
    return false;

  /*
  // Void element (11 bytes total)
  if (WriteUInt(pWriter, 0x6C, 1))  // Void element
    return false;

  if (WriteUInt(pWriter, 9, 1))
    return false;

  const unsigned char c = 0;
  for (int i = 0; i < 9; ++i) {
    if (pWriter->Write(&c, 1))
      return false;
  }
  */

  if (!WriteEbmlElement(pWriter, kMkvDocTypeVersion, 2ULL))
    return false;
  if (!WriteEbmlElement(pWriter, kMkvDocTypeReadVersion, 2ULL))
    return false;

  return true;
}

VideoTrack::VideoTrack()
    : width_(0),
      height_(0) {
}

VideoTrack::~VideoTrack() {
}

unsigned long long VideoTrack::Size() const {
  const unsigned long long parent_size = Track::Size();

  unsigned long long size = EbmlElementSize(kMkvPixelWidth, width_, false);
  size += EbmlElementSize(kMkvPixelHeight, height_, false);
  size += EbmlElementSize(kMkvVideo, size, true);

  return parent_size + size;
}

unsigned long long VideoTrack::PayloadSize() const {
  const unsigned long long parent_size = Track::PayloadSize();

  unsigned long long size = EbmlElementSize(kMkvPixelWidth, width_, false);
  size += EbmlElementSize(kMkvPixelHeight, height_, false);
  size += EbmlElementSize(kMkvVideo, size, true);

  return parent_size + size;
}

bool VideoTrack::Write(IMkvWriter* writer) const {
  assert(writer);

  if (!Track::Write(writer))
    return false;

  // Calculate VideoSettings size.
  unsigned long long size = EbmlElementSize(kMkvPixelWidth, width_, false);
  size += EbmlElementSize(kMkvPixelHeight, height_, false);

  if (!WriteEbmlMasterElement(writer, kMkvVideo, size))
    return false;

  const long long payload_position = writer->Position();
  if (payload_position < 0)
    return false;

  if (!WriteEbmlElement(writer, kMkvPixelWidth, width_))
    return false;
  if (!WriteEbmlElement(writer, kMkvPixelHeight, height_))
    return false;

  const long long stop_position = writer->Position();
  if (stop_position < 0)
    return false;
  assert(stop_position - payload_position == size);

  return true;
}

AudioTrack::AudioTrack()
    : sample_rate_(0.0),
      channels_(1),
      bit_depth_(0) {
}

AudioTrack::~AudioTrack() {
}

unsigned long long AudioTrack::Size() const {
  const unsigned long long parent_size = Track::Size();

  unsigned long long size = EbmlElementSize(kMkvSamplingFrequency,
                                            static_cast<float>(sample_rate_),
                                            false);
  size += EbmlElementSize(kMkvChannels, channels_, false);
  if (bit_depth_ > 0)
    size += EbmlElementSize(kMkvBitDepth, bit_depth_, false);
  size += EbmlElementSize(kMkvAudio, size, true);

  return parent_size + size;
}

unsigned long long AudioTrack::PayloadSize() const {
  const unsigned long long parent_size = Track::PayloadSize();

  unsigned long long size = EbmlElementSize(kMkvSamplingFrequency,
                                            static_cast<float>(sample_rate_),
                                            false);
  size += EbmlElementSize(kMkvChannels, channels_, false);
  if (bit_depth_ > 0)
    size += EbmlElementSize(kMkvBitDepth, bit_depth_, false);
  size += EbmlElementSize(kMkvAudio, size, true);

  return parent_size + size;
}

bool AudioTrack::Write(IMkvWriter* writer) const {
  assert(writer);

  if (!Track::Write(writer))
    return false;

  // Calculate AudioSettings size.
  unsigned long long size = EbmlElementSize(kMkvSamplingFrequency,
                                            static_cast<float>(sample_rate_),
                                            false);
  size += EbmlElementSize(kMkvChannels, channels_, false);
  if (bit_depth_ > 0)
    size += EbmlElementSize(kMkvBitDepth, bit_depth_, false);

  if (!WriteEbmlMasterElement(writer, kMkvAudio, size))
    return false;

  const long long payload_position = writer->Position();
  if (payload_position < 0)
    return false;

  if (!WriteEbmlElement(writer,
                        kMkvSamplingFrequency,
                        static_cast<float>(sample_rate_)))
    return false;
  if (!WriteEbmlElement(writer, kMkvChannels, channels_))
    return false;
  if (bit_depth_ > 0)
    if (!WriteEbmlElement(writer, kMkvBitDepth, bit_depth_))
      return false;

  const long long stop_position = writer->Position();
  if (stop_position < 0)
    return false;
  assert(stop_position - payload_position == size);

  return true;
}

Track::Track()
    : number_(0),
      uid_(MakeTrackUID()),
      type_(0),
      codec_id_(NULL),
      codec_private_(NULL) {
}

Track::~Track() {
  if (codec_id_) {
    delete [] codec_id_;
    codec_id_ = NULL;
  }

  if (codec_private_) {
    delete [] codec_private_;
    codec_private_ = NULL;
  }
}

unsigned long long Track::Size() const {
  unsigned long long size = Track::PayloadSize();
  size += EbmlElementSize(kMkvTrackEntry, size, true);

  return size;
}

unsigned long long Track::PayloadSize() const {
  unsigned long long size = EbmlElementSize(kMkvTrackNumber, number_, false);
  size += EbmlElementSize(kMkvTrackUID, uid_, false);
  size += EbmlElementSize(kMkvTrackType, type_, false);
  if (codec_id_)
    size += EbmlElementSize(kMkvCodecID, codec_id_, false);
  //if (codec_private_)
  //  size += EbmlElementSize(, codec_private_, false);

  return size;
}

bool Track::Write(IMkvWriter* writer) const {
  assert(writer);

  // |size| may be bigger than what is written out in this function because
  // derived classes may write out more data in the Track element.
  const unsigned long long size = PayloadSize();

  if (!WriteEbmlMasterElement(writer, kMkvTrackEntry, size))
    return false;

  unsigned long long test = EbmlElementSize(kMkvTrackNumber, number_, false);
  test += EbmlElementSize(kMkvTrackUID, uid_, false);
  test += EbmlElementSize(kMkvTrackType, type_, false);
  if (codec_id_)
    test += EbmlElementSize(kMkvCodecID, codec_id_, false);

  const long long payload_position = writer->Position();
  if (payload_position < 0)
    return false;

  if (!WriteEbmlElement(writer, kMkvTrackNumber, number_))
    return false;
  if (!WriteEbmlElement(writer, kMkvTrackUID, uid_))
    return false;
  if (!WriteEbmlElement(writer, kMkvTrackType, type_))
    return false;
  if (codec_id_) {
    if (!WriteEbmlElement(writer, kMkvCodecID, codec_id_))
      return false;
  }

  const long long stop_position = writer->Position();
  if (stop_position < 0)
    return false;
  assert(stop_position - payload_position == test);

  return true;
}

void Track::SetCodecPrivate(const unsigned char* codec_private, int length) {
  assert(codec_private);
  assert(length > 0);

  if (codec_private_)
    delete [] codec_private_;

  codec_private_ = new (std::nothrow) unsigned char[length];
  if (codec_private_)
    memcpy(codec_private_, codec_private, length);
}

void Track::codec_id(const char* codec_id) {
  assert(codec_id);

  if (codec_id_)
    delete [] codec_id_;

  int length = strlen(codec_id) + 1;
  codec_id_ = new (std::nothrow) char[length];
  if (codec_id_) {
#ifdef WIN32
    strcpy_s(codec_id_, length, codec_id);
#else
    strcpy(codec_id_, codec_id);
#endif
  }
}

SegmentInfo::SegmentInfo()
    : timecode_scale_(1000000ULL),
      duration_(-1.0),
      muxing_app_(NULL),
      writing_app_(NULL) {
}

SegmentInfo::~SegmentInfo() {
  if (muxing_app_) {
    delete [] muxing_app_;
    muxing_app_ = NULL;
  }

  if (writing_app_) {
    delete [] writing_app_;
    writing_app_ = NULL;
  }
}

bool SegmentInfo::Init() {
  int major;
  int minor;
  int build;
  int revision;
  GetVersion(major, minor, build, revision);
  char temp[256];
#ifdef WIN32
  sprintf_s(temp, 64, "libwebm-%d.%d.%d.%d", major, minor, build, revision);
#else
  sprintf(temp, "libwebm-%d.%d.%d.%d", major, minor, build, revision);
#endif

  const int app_len = strlen(temp);

  if (muxing_app_)
    delete [] muxing_app_;

  muxing_app_ = new (std::nothrow) char[app_len + 1];
  if (!muxing_app_)
    return false;

#ifdef WIN32
  strcpy_s(muxing_app_, app_len + 1, temp);
#else
  strcpy(muxing_app_, temp);
#endif

  writing_app(temp);
  if (!writing_app_)
    return false;
  return true;
}

bool SegmentInfo::Write(IMkvWriter* writer) const {
  assert(writer);

  if (!muxing_app_ || !writing_app_)
    return false;

  unsigned long long size = EbmlElementSize(kMkvTimecodeScale, timecode_scale_, false);
  if (duration_ > 0.0)
    size += EbmlElementSize(kMkvDuration, static_cast<float>(duration_), false);
  size += EbmlElementSize(kMkvMuxingApp, muxing_app_, false);
  size += EbmlElementSize(kMkvWritingApp, writing_app_, false);

  if (!WriteEbmlMasterElement(writer, kMkvInfo, size))
    return false;

  const long long payload_position = writer->Position();
  if (payload_position < 0)
    return false;

  if (!WriteEbmlElement(writer, kMkvTimecodeScale, timecode_scale_))
    return false;
  if (duration_ > 0.0)
    if (!WriteEbmlElement(writer, kMkvDuration, static_cast<float>(duration_)))
      return false;
  if (!WriteEbmlElement(writer, kMkvMuxingApp, muxing_app_))
    return false;
  if (!WriteEbmlElement(writer, kMkvWritingApp, writing_app_))
    return false;

  const long long stop_position = writer->Position();
  if (stop_position < 0)
    return false;
  assert(stop_position - payload_position == size);

  return true;
}

void SegmentInfo::writing_app(const char* app) {
  assert(app);

  if (writing_app_)
    delete [] writing_app_;

  int length = strlen(app) + 1;
  writing_app_ = new (std::nothrow) char[length];
  if (writing_app_) {
#ifdef WIN32
    strcpy_s(writing_app_, length, app);
#else
    strcpy(writing_app_, app);
#endif
  }
}

Tracks::Tracks()
    : m_trackEntries(NULL),
      m_trackEntriesSize(0) {
}

Tracks::~Tracks() {
  if (m_trackEntries) {
    for (unsigned int i=0; i<m_trackEntriesSize; ++i) {
      Track* const pTrack = m_trackEntries[i];
      delete pTrack;
    }
    delete[] m_trackEntries;
  }
}

bool Tracks::AddTrack(Track* track) {
  const unsigned int count = m_trackEntriesSize+1;

  Track** track_entries = new (std::nothrow) Track*[count];
  if (!track_entries)
    return false;

  for (unsigned int i=0; i<m_trackEntriesSize; ++i) {
    track_entries[i] = m_trackEntries[i];
  }

  delete [] m_trackEntries;

  track->number(count);

  m_trackEntries = track_entries;
  m_trackEntries[m_trackEntriesSize] = track;
  m_trackEntriesSize = count;
  return true;
}

unsigned long Tracks::GetTracksCount() const {
  return m_trackEntriesSize;
}

const Track* Tracks::GetTrackByIndex(unsigned long index) const {
  if (m_trackEntries == NULL)
    return NULL;

  if (index >= m_trackEntriesSize)
    return NULL;

  return m_trackEntries[index];
}

bool Tracks::Write(IMkvWriter* writer) const {
  assert(writer);

  unsigned long long size = 0;
  const int count = GetTracksCount();
  for (int i=0; i<count; ++i) {
    const Track* pTrack = GetTrackByIndex(i);
    assert(pTrack);
    size += pTrack->Size();
  }

  if (!WriteEbmlMasterElement(writer, kMkvTracks, size))
    return false;

  const long long payload_position = writer->Position();
  if (payload_position < 0)
    return false;

  for (int i=0; i<count; ++i) {
    const Track* pTrack = GetTrackByIndex(i);
    if (!pTrack->Write(writer))
      return false;
  }

  const long long stop_position = writer->Position();
  if (stop_position < 0)
    return false;
  assert(stop_position - payload_position == size);

  return true;
}

Segment::Segment(IMkvWriter* writer)
    : writer_(writer) {
  assert(writer_);

  // TODO: Create an Init function for Segment.
  segment_info_.Init();
}

Segment::~Segment() {
}

bool Segment::AddVideoTrack(int width, int height) {
  VideoTrack* vid_track = new (std::nothrow) VideoTrack();
  if (!vid_track)
    return false;

  vid_track->type(Tracks::kVideo);
  vid_track->codec_id("V_VP8");
  vid_track->width(width);
  vid_track->height(height);

  m_tracks_.AddTrack(vid_track);

  return true;
}

bool Segment::AddAudioTrack(int sample_rate, int channels) {
  AudioTrack* aud_track = new (std::nothrow) AudioTrack();
  if (!aud_track)
    return false;

  aud_track->type(Tracks::kAudio);
  aud_track->codec_id("A_VORBIS");
  aud_track->sample_rate(sample_rate);
  aud_track->channels(channels);

  m_tracks_.AddTrack(aud_track);

  return true;
}

bool Segment::WriteSegmentHeader() {
  if (SerializeInt(writer_, kMkvSegment, 4)) {
    return false;
  }

  // Write "unknown" (-1) as segment size value. If mode is kFile, Segment
  // will write over duration when the file is finalized.
  if (SerializeInt(writer_, -1, 1)) {
    return false;
  }

  if (!segment_info_.Write(writer_))
    return false;

  if (!m_tracks_.Write(writer_))
    return false;

  return true;
}

}  // namespace mkvmuxer
