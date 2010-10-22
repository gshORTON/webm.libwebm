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

struct MediaSample {
   long long startTime;
   long long stopTime;
   unsigned int extraData;
   long bufLength;
   unsigned char* pData;
};

}  //end namespace webmmux
