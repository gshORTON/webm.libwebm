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

class StreamInfo
{
private:
	int m_timebase_rate;
	int m_timebase_scale;
	int m_width;
	int m_height;
public:
	int SetTimebase(int rate, int scale);
	int SetWidth(int);
	int SetHeight(int);
	int GetWidth();
	int GetHeight();
	float GetFrameRate();
	
};

} // namespace WebMuxLib
