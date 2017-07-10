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

HRESULT IXAudio2CreateMasteringVoice(IXAudio2* pXAudio2,
                                     IXAudio2MasteringVoice** ppMasteringVoice,
                                     UINT32 InputChannels,
                                     UINT32 InputSampleRate,
                                     UINT32 Flags,
                                     UINT32 DeviceIndex,
                                     const XAUDIO2_EFFECT_CHAIN* pEffectChain)
{
    return pXAudio2->CreateMasteringVoice(ppMasteringVoice,
                                          InputChannels,
                                          InputSampleRate,
                                          Flags,
                                          DeviceIndex,
                                          pEffectChain);
}

HRESULT IXAudio2CreateSourceVoice(IXAudio2* pXAudio2,
                                  IXAudio2SourceVoice** ppSourceVoice,
                                  const WAVEFORMATEX* pSourceFormat,
                                  UINT32 Flags,
                                  float MaxFrequencyRatio,
                                  IXAudio2VoiceCallback* pCallback = nullptr,
                                  const XAUDIO2_VOICE_SENDS* pSendList = nullptr,
                                  const XAUDIO2_EFFECT_CHAIN* pEffectChain = nullptr)
{
    return pXAudio2->CreateSourceVoice(ppSourceVoice,
                                       pSourceFormat,
                                       Flags,
                                       MaxFrequencyRatio,
                                       pCallback,
                                       pSendList,
                                       pEffectChain);
}

#endif
