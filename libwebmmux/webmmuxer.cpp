// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#include "webmmuxer.hpp"
#include "webmmuxcontext.hpp"

namespace webmmux
{

WebMuxer::OpenContext(std::basic_iostream<unsigned char> outputStream, const char* writingAppName = void)
{
	//TODO NOT IMPLEMENTED
}


WebMuxer::OpenContext(std::basic_iostream<unsigned char> outputStream, const char* writingAppName = void)
{
	m_pContext = new Context();
	if (writingAppName)
		strcpy(static_cast<Context*>(m_pContext)->m_writing_app, writingAppName);
	static_cast<Context*>(m_pContext)->Open(outputStream);
}

WebMuxer::CloseContext()
{
	if (m_pContext)
	{
		Context* pCtx = static_cast<Context*>(m_pContext);
		pCtx->Close();
		pCtx->SetVideoStream(0);
		pCtx->SetAudioStream(0);
		pCtx = 0;
		delete m_pContext;
}


StreamHandle_t WebMuxer::AddVP8VideoStream(const StreamInfo_t& streaminfo)
{
	static_cast<StreamVideoVPx*>(m_pVideoStream) = new StreamVideoVPx(streaminfo);
	static_cast<Context*>(m_pContext)->SetVideoStream(static_cast<StreamVideoVPx*>(m_pVideoStream));
	return 0;
}

StreamHandle_t WebMuxer::AddVorbisAudioStream(const StreamInfo_t& streaminfo)
{
	static_cast<StreamAudioVorbis*>(m_pAudioStream
    bool WaitOnStream(StreamHandle_t);
    PushSample(StreamHandle_t, Media_sample_t);
    NotifyEOS(StreamHandle_t);

private:
	void* m_Context;
	void* m_pVideoStream;
	void* m_pAudioStream;
	std::basic_iostream<unsigned char> m_outputStream;
};

} // namespace webmmux
