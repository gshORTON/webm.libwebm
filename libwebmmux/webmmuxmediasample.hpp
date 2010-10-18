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
	
class MediaSample
{

public:
	//TODO: DW Should this class control the data or just hold a pointer to the data?
	MediaSample();
	~MediaSample();

	long GetBufLength();
	long long GetStartTime();
	long long GetStopTime();
	int GetDataBuf(unsigned char** ppBuf);
	//int GetWidth();
	//int GetHeight();
	unsigned int GetFlags();

	//SetBufLength(long);
	int FillDataBuf(unsigned char* pBuf, long numChars);
	int SetStartTime(long long startTime_nanoseconds); // in absolute nanoseconds
	int SetStopTime(long long stopTime_nanoseconds); // in absolute nanoseconds
	//int SetWidth(int width);
	//int SetHeight(int height);
	int SetFlags(unsigned int);

private:
   unsigned char* m_pData;
   long m_bufLength;
   long long m_startTime;
   long long m_stopTime;
   //int m_width;
   //int m_height;
   //TODO: DW samplespersecond for audio?
   int m_samplesPerSecond;
   unsigned int m_flags;
};

}  //end namespace webmmux
