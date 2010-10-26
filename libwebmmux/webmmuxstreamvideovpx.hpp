// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#pragma once
#include <vector>
#include "webmmuxstreamvideo.hpp"

namespace webmmux
{

typedef unsigned long vpx_codec_frame_flags_t;
#define VPX_FRAME_IS_KEY       0x1 /**< frame is the start of a GOP */
#define VPX_FRAME_IS_DROPPABLE 0x2 /**< frame can be dropped without affecting
    the stream (no future frame depends on this one) */
#define VPX_FRAME_IS_INVISIBLE 0x4 /**< frame should be decoded but will not
    be shown */

typedef struct {
   void *buf;      /**< compressed data buffer */
   size_t sz;       /**< length of compressed data */
   unsigned long pts;      /**< time stamp to show frame (in timebase units) */
   unsigned long duration; /**< duration to show frame (in timebase units) */
   vpx_codec_frame_flags_t  flags;    /**< flags for this frame */
} VPxFrame_t;

class StreamVideoVPx : public StreamVideo
{
    StreamVideoVPx(const StreamVideoVPx&);
    StreamVideoVPx& operator=(const StreamVideoVPx&);

public:
    StreamVideoVPx(Context&, const StreamInfo&);

    int Receive(MediaSample*);
    int EndOfStream();

protected:

    //void WriteTrackName();
    void WriteTrackCodecID();
    void WriteTrackCodecName();
    void WriteTrackSettings();

private:

    class VPxFrame : public VideoFrame
    {
        VPxFrame(const VPxFrame&);
        VPxFrame& operator=(const VPxFrame&);

    private:
        MediaSample* const m_pSample;
        unsigned long m_timecode;

    public:
        explicit VPxFrame(MediaSample*, StreamVideoVPx*);
        ~VPxFrame();

        bool IsKey() const;
        unsigned long GetTimecode() const;
        unsigned long GetSize() const;
        const unsigned char* GetData() const;

    };

public:

    long GetLastTimecode() const;

};

}  //end namespace webmmux
