// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

#include <xaudio2.h>

HRESULT XAudio27CreateProc(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);
ULONG IXAudio2Release(IXAudio2* pXAudio2);
HRESULT IXAudio2CreateMasteringVoice(IXAudio2* pXAudio2, IXAudio2MasteringVoice** ppMasteringVoice);
HRESULT IXAudio2CreateSourceVoice(IXAudio2* pXAudio2, IXAudio2SourceVoice** ppSourceVoice, const WAVEFORMATEX* pSourceFormat);

#endif
