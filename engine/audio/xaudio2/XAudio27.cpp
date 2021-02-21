// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "../../core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include "XAudio2.h"

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
                                  IXAudio2VoiceCallback* pCallback,
                                  const XAUDIO2_VOICE_SENDS* pSendList,
                                  const XAUDIO2_EFFECT_CHAIN* pEffectChain)
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
