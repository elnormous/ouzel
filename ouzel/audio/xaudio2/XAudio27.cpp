// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

#include "Xaudio2.h"

HRESULT XAudio27CreateProc(IXAudio2** ppXAudio2, UINT32 Flags, UINT32 XAudio2Processor)
{
    return XAudio2Create(ppXAudio2, Flags, static_cast<XAUDIO2_PROCESSOR>(XAudio2Processor));
}

ULONG IXAudio2Release(IXAudio2* pXAudio2)
{
    return pXAudio2->Release();
}

HRESULT IXAudio2CreateMasteringVoice(IXAudio2* pXAudio2, IXAudio2MasteringVoice** ppMasteringVoice)
{
    return pXAudio2->CreateMasteringVoice(ppMasteringVoice);
}

HRESULT IXAudio2CreateSourceVoice(IXAudio2* pXAudio2,
                                  IXAudio2SourceVoice** ppSourceVoice,
                                  const WAVEFORMATEX* pSourceFormat,
                                  UINT32 Flags)
{
    return pXAudio2->CreateSourceVoice(ppSourceVoice, pSourceFormat, Flags);
}

#endif
