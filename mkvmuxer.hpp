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

  // Set the current File position. Returns 0 on success.
  virtual int Position(long long position) = 0;
  
  virtual bool Seekable() const = 0;

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
bool WriteEbmlHeader(IMkvWriter* pWriter);

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

class AudioTrack : public Track {
public:
  AudioTrack();
  virtual ~AudioTrack();

  virtual unsigned long long Size() const;
  virtual unsigned long long PayloadSize() const;
  virtual bool Write(IMkvWriter* writer) const;

  double sample_rate() const {return sample_rate_;}
  void sample_rate(double sample_rate) {sample_rate_ = sample_rate;}

  unsigned long long channels() const {return channels_;}
  void channels(unsigned long long channels) {channels_ = channels;}

  unsigned long long bit_depth() const {return bit_depth_;}
  void bit_depth(unsigned long long bit_depth) {bit_depth_ = bit_depth;}

private:
  double sample_rate_;
  unsigned long long channels_;
  unsigned long long bit_depth_;

  // DISALLOW_COPY_AND_ASSIGN
  AudioTrack(const AudioTrack&);
  AudioTrack& operator=(const AudioTrack&);
};

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
  // Set to libwebm-%d.%d.%d.%d, major, minor, build, revision.
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

class Cluster {
public:
    explicit Cluster(unsigned long long timecode);
    ~Cluster();

    void AddPayloadSize(unsigned long long size);

     unsigned long long timecode() const {return timecode_;}

     unsigned long long size_position() const {return size_position_;}
     void size_position(unsigned long long pos) {size_position_ = pos;}

     unsigned long long payload_size() const {return payload_size_;}

private:
    // The timecode of the cluster.
    const unsigned long long timecode_;

    // The file position of the size.
    long long size_position_;

    unsigned long long payload_size_;

    // DISALLOW_COPY_AND_ASSIGN
    Cluster(const Cluster&);
    Cluster& operator=(const Cluster&);
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

  // Adds a video track to the segment. Returns the number of the track on
  // success, 0 on error.
  unsigned long long AddVideoTrack(int width, int height);

  // Adds an audio track to the segment. Returns the number of the track on
  // success, 0 on error.
  unsigned long long AddAudioTrack(int sample_rate, int channels);

  // Adds a frame to be output in the file. Returns true on success.
  // Inputs:
  //   frame: Pointer to the data
  //   length: Length of the data
  //   track_number: Track to add the data to. Value returned by Add track
  //                 functions.
  //   timestamp:    Timestamp of the frame in nanoseconds from 0.
  //   is_key:       Flag telling whter or not this frame is a key frame.
  bool AddFrame(unsigned char* frame,
                unsigned long long length,
                unsigned long long track_number,
                unsigned long long timestamp,
                bool is_key);

  bool WriteSegmentHeader();

  // TODO: Change this!!!
  bool Finalize();

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

  int cluster_list_size_;
  int cluster_list_capacity_;
  Cluster** cluster_list_;
  bool new_cluster_;
  unsigned long long last_timestamp_;

  // DISALLOW_COPY_AND_ASSIGN
  Segment(const Segment&);
  Segment& operator=(const Segment&);
};

}  //end namespace mkvmuxer

#endif //MKVMUXER_HPP