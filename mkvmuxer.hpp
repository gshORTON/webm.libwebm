// Copyright (c) 2011 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#ifndef MKVMUXER_HPP
#define MKVMUXER_HPP

namespace mkvmuxer
{

// Interface used by the mkvmuxer to write out the Mkv data.
class IMkvWriter {
 public:
  virtual int Write(const void* buf, unsigned long len) = 0;
  virtual long long Position() const = 0;

 protected:
  IMkvWriter();
  virtual ~IMkvWriter();

 private:
    // DISALLOW_COPY_AND_ASSIGN
    IMkvWriter(const IMkvWriter&);
    IMkvWriter& operator=(const IMkvWriter&);
};

// Writes out the EBML header for a WebM file. This function must be called
// first before any other libwebm writing functions are called.
bool WriteEbmlHeader(IMkvWriter* writer);

class Track {
public:
  Track();
  virtual ~Track();

  virtual unsigned long long Size() const;
  virtual unsigned long long PayloadSize() const;

  virtual bool Write(IMkvWriter* writer) const;

  void SetCodecPrivate(const unsigned char* codec_private, int length);

  unsigned long long number() const {return number_;}
  void number(unsigned long long number) {number_ = number;}
  
  unsigned long long uid() const {return uid_;}
  //void uid(unsigned long long uid) {uid_ = uid;}
  
  unsigned long long type() const {return type_;}
  void type(unsigned long long type) {type_ = type;}

  const char* codec_id() const {return codec_id_;}
  void codec_id(const char* codec_id);

  const unsigned char* codec_private() const {return codec_private_;}

private:
  unsigned long long number_;
  const unsigned long long uid_;
  unsigned long long type_;

  char* codec_id_;
  unsigned char* codec_private_;

  // DISALLOW_COPY_AND_ASSIGN
  Track(const Track&);
  Track& operator=(const Track&);
};


class VideoTrack : public Track {
public:
  VideoTrack();
  virtual ~VideoTrack();

  virtual unsigned long long Size() const;
  virtual unsigned long long PayloadSize() const;

  virtual bool Write(IMkvWriter* writer) const;

  unsigned long long width() const {return width_;}
  void width(unsigned long long width) {width_ = width;}

  unsigned long long height() const {return height_;}
  void height(unsigned long long height) {height_ = height;}

private:
  unsigned long long width_;
  unsigned long long height_;

  // DISALLOW_COPY_AND_ASSIGN
  VideoTrack(const VideoTrack&);
  VideoTrack& operator=(const VideoTrack&);
};

/*
class AudioTrack : public Track
{
    AudioTrack(const AudioTrack&);
    AudioTrack& operator=(const AudioTrack&);

public:
    AudioTrack(
        Segment*,
        const Info&,
        long long element_start,
        long long element_size);
    double GetSamplingRate() const;
    long long GetChannels() const;
    long long GetBitDepth() const;
    bool VetEntry(const BlockEntry*) const;
    long Seek(long long time_ns, const BlockEntry*&) const;

private:
    double m_rate;
    long long m_channels;
    long long m_bitDepth;
};
*/


class SegmentInfo {
public:
  SegmentInfo();
  ~SegmentInfo();

  // Sets |muxing_app_| and |writing_app_|.
  bool Init();

  bool Write(IMkvWriter* writer) const;

  unsigned long long timecode_scale() const {return timecode_scale_;}
  void timecode_scale(unsigned long long scale) {timecode_scale_ = scale;}
  double duration() const {return duration_;}
  void duration(double duration) {duration_ = duration;}

  const char* muxing_app() const {return muxing_app_;}
  const char* writing_app() const {return writing_app_;}
  void writing_app(const char* app);

private:
  // For a description of the WebM elements see
  // http://www.webmproject.org/code/specs/container/.

  unsigned long long timecode_scale_;
  // Initially set to -1 to signfy that a duration has not been set and should
  // not be written out.
  double duration_;
  // Initially set to libwebm-%d.%d.%d.%d, major, minor, build, revision.
  char* muxing_app_;
  // Initially set to libwebm-%d.%d.%d.%d, major, minor, build, revision.
  char* writing_app_;

  // DISALLOW_COPY_AND_ASSIGN
  SegmentInfo(const SegmentInfo&);
  SegmentInfo& operator=(const SegmentInfo&);
};

class Tracks {
public:
    Tracks();
    ~Tracks();

    static const int kVideo = 0x1;
    static const int kAudio = 0x2;

    bool AddTrack(Track* track);

    unsigned long GetTracksCount() const;

    //const Track* GetTrackByNumber(unsigned long tn) const;
    const Track* GetTrackByIndex(unsigned long idx) const;

    bool Write(IMkvWriter* writer) const;

private:
    unsigned int m_trackEntriesSize;
    Track** m_trackEntries;

    // DISALLOW_COPY_AND_ASSIGN
    Tracks(const Tracks&);
    Tracks& operator=(const Tracks&);
};

// This class represents the main segment in a WebM file.
class Segment {
public:
  explicit Segment(IMkvWriter* writer);
  virtual ~Segment();

  enum Mode {
    kLive = 0x1,
    kFile = 0x2
  };

  bool AddVideoTrack(int width, int height);

  bool WriteSegmentHeader();

  const SegmentInfo* segment_info() const {return &segment_info_;}

  Mode mode() const {return mode_;}
  void mode(Mode mode) {mode_ = mode;}

private:
  SegmentInfo segment_info_;
  Tracks m_tracks_;
  IMkvWriter* writer_;

  // The mode that segment is in. If set to |kLive| the writer must not
  // seek backwards.
  Mode mode_;

  // DISALLOW_COPY_AND_ASSIGN
  Segment(const Segment&);
  Segment& operator=(const Segment&);
};

}  //end namespace mkvmuxer

#endif //MKVMUXER_HPP