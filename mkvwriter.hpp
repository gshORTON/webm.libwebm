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

// Default implementation of the IMkvWriter interface on Windows.
class MkvWriter : public IMkvWriter
{
 public:
  MkvWriter();
  virtual ~MkvWriter();

  // IMkvWriter interface
  virtual long long Position() const;
  virtual int Position(long long position);
  virtual bool Seekable() const;
  virtual int Write(const void* buffer, unsigned long length);

  // Creates and opens a file for writing. |filename| is the name of the file
  // to open. This function will overwrite the contents of |filename|. Returns
  // true on success.
  bool Open(const char* filename);

  // Closes an opened file.
  void Close();

 private:
  // File handle to output file.
  FILE* file_;

  LIBWEBM_DISALLOW_COPY_AND_ASSIGN(MkvWriter);
};

}  //end namespace mkvmuxer

#endif //MKVWRITER_HPP
