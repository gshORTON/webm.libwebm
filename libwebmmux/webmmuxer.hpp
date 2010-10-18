// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#pragma once

namespace webmmux
{

typedef struct {
    int timebase_rate;
    int timebase_scale;
    int width;
    int height;
} StreamInfo_t;

typedef struct {
    unsigned char* pDataBuf;
    long dataBufLength;
    long long startTime; // in timebase units
    long long stopTime; // in timebase units
    int samplesPerSecond; //TODO: DW for audio?
    unsigned int flags;
} MediaSample_t;

typedef int StreamHandle_t;

class WebmMuxer
{
public:
    OpenContext(char* filename, const char* writingAppName = void);
    OpenContext(istd::basic_iostream<unsigned char>, const char* writingAppName = void);
    CloseContext();

    StreamHandle_t AddVP8VideoStream(const StreamInfo_t&);
    StreamHandle_t AddVorbisAudioStream(const StreamInfo_t&);
    bool WaitOnStream(StreamHandle_t);
    PushSample(StreamHandle_t, Media_sample_t);
    NotifyEOS(StreamHandle_t);

private:
	void* m_pContext;
	void* m_pVideoStream;
	void* m_pAudioStream;
	std::basic_iostream<unsigned char> m_outputStream;
};

} // namespace webmmux

