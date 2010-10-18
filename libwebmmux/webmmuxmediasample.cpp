// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include <cstring>
#include <new>
#include "webmmuxmediasample.hpp"

namespace webmmux
{

MediaSample::MediaSample() :
	m_pData(0),
	m_bufLength(0),
	m_startTime(0)
//	m_timeStop(0),
//	m_width(0),
//	m_height(0)
{
}

MediaSample::~MediaSample()
{
	if (m_pData != 0)
		delete[] m_pData;
}


long MediaSample::GetBufLength()
{
	return(m_bufLength);
}

long long MediaSample::GetStartTime()
{
	return(m_startTime);
}

long long MediaSample::GetStopTime()
{
	return(m_stopTime);
}

int MediaSample::GetDataBuf(unsigned char** ppBuf)
{
	*ppBuf = m_pData;
	return(0);
}

/*int MediaSample::GetWidth()
{
	return(m_width);
}

int MediaSample::GetHeight()
{
	return(m_height);
}
*/
unsigned int MediaSample::GetFlags()
{
	return(m_flags);
}

int MediaSample::FillDataBuf(unsigned char* pBuf, long numChars)
{
	if (m_pData)
		delete[] m_pData;
	m_bufLength = numChars;
	m_pData = new (std::nothrow) unsigned char [numChars];
	if (m_pData == 0)
		return(-1); //TODO: DW do something with the error return?
	memcpy(m_pData, pBuf, numChars);
	return(0);
}

int MediaSample::SetStartTime(long long startTime_nanoseconds)
{
	m_startTime = startTime_nanoseconds;
	return(0);
}

int MediaSample::SetStopTime(long long stopTime_nanoseconds)
{
	m_stopTime = stopTime_nanoseconds;
	return(0);
}

/*int MediaSample::SetWidth(int width)
{
	m_width= width;
	return(0);
}

int MediaSample::SetHeight(int height)
{
	m_height = height;
	return(0);
}
*/
int MediaSample::SetFlags(unsigned int flags)
{
	m_flags = flags;
	return(0);
}

} // namespace webmmux
