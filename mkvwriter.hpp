// Copyright (c) 2011 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#ifndef MKVWRITER_HPP
#define MKVWRITER_HPP

#include "mkvmuxer.hpp"
#include <stdio.h>

namespace mkvmuxer
{

class MkvWriter : public IMkvWriter
{
public:
    MkvWriter();
    virtual ~MkvWriter();

    // IMkvWriter Implementation
    virtual int Write(const void* buffer, unsigned long length);
    virtual long long Position() const;
    virtual int Position(long long position);

    virtual bool Seekable() const;

    bool Open(const char* filename);
    void Close();
    

private:
    FILE* file_;

    DISALLOW_COPY_AND_ASSIGN(MkvWriter);
};

}  //end namespace mkvmuxer

#endif //MKVWRITER_HPP
