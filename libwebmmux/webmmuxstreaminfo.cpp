// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include "webmmuxstreaminfo.hpp"

namespace webmmux
{
int StreamInfo::SetTimebase(int rate, int scale)
{
	m_timebase_rate = rate;
	m_timebase_scale = scale;
	return(0);
}

int StreamInfo::SetWidth(int width)
{
	m_width = width;
	return(0);
}

int StreamInfo::SetHeight(int height)
{
	m_height = height;
	return(0);
}

int StreamInfo::GetWidth()
{
	return(m_width);
}

int StreamInfo::GetHeight()
{
	return(m_height);
}

float StreamInfo::GetFrameRate()
{
	return((float)m_timebase_scale/(float)m_timebase_rate);
}

} // WebmuxLib namespace