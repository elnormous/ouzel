// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

#include <objbase.h>

const GUID CLSID_XAudio2 = { 0x5a508685, 0xa254, 0x4fba, 0x9b, 0x82, 0x9a, 0x24, 0xb0, 0x03, 0x06, 0xaf };
const GUID CLSID_IXAudio2 = { 0x8bcf1f58, 0x9fe7, 0x4583, 0x8a, 0xc6, 0xe2, 0xad, 0xc4, 0x65, 0xc8, 0xbb };

struct XAUDIO2_DEVICE_DETAILS;
struct IXAudio2EngineCallback;
struct IXAudio2SourceVoice;
struct IXAudio2VoiceCallback;
struct XAUDIO2_VOICE_SENDS;
struct XAUDIO2_EFFECT_CHAIN;
struct IXAudio2SubmixVoice;
struct IXAudio2MasteringVoice;
struct XAUDIO2_PERFORMANCE_DATA;
struct XAUDIO2_DEBUG_CONFIGURATION;

#define XAUDIO2_DEFAULT_FREQ_RATIO      2.0f
#define XAUDIO2_DEFAULT_CHANNELS        0
#define XAUDIO2_DEFAULT_SAMPLERATE      0

typedef UINT32 XAUDIO2_PROCESSOR;
#define Processor1  0x00000001
#define XAUDIO2_DEFAULT_PROCESSOR Processor1

#define X2DEFAULT(x) =x

#undef INTERFACE
#define INTERFACE IXAudio2
DECLARE_INTERFACE_(IXAudio2, IUnknown)
{
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, __deref_out void** ppvInterface) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS)PURE;
    STDMETHOD_(ULONG, Release) (THIS)PURE;
    STDMETHOD(GetDeviceCount) (THIS_ __out UINT32* pCount) PURE;
    STDMETHOD(GetDeviceDetails) (THIS_ UINT32 Index, __out XAUDIO2_DEVICE_DETAILS* pDeviceDetails) PURE;
    STDMETHOD(Initialize) (THIS_ UINT32 Flags X2DEFAULT(0),
        XAUDIO2_PROCESSOR XAudio2Processor X2DEFAULT(XAUDIO2_DEFAULT_PROCESSOR)) PURE;
    STDMETHOD(RegisterForCallbacks) (__in IXAudio2EngineCallback* pCallback) PURE;
    STDMETHOD_(void, UnregisterForCallbacks) (__in IXAudio2EngineCallback* pCallback) PURE;
    STDMETHOD(CreateSourceVoice) (THIS_ __deref_out IXAudio2SourceVoice** ppSourceVoice,
        __in const WAVEFORMATEX* pSourceFormat,
        UINT32 Flags X2DEFAULT(0),
        float MaxFrequencyRatio X2DEFAULT(XAUDIO2_DEFAULT_FREQ_RATIO),
        __in_opt IXAudio2VoiceCallback* pCallback X2DEFAULT(NULL),
        __in_opt const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
        __in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL)) PURE;
    STDMETHOD(CreateSubmixVoice) (THIS_ __deref_out IXAudio2SubmixVoice** ppSubmixVoice,
        UINT32 InputChannels, UINT32 InputSampleRate,
        UINT32 Flags X2DEFAULT(0), UINT32 ProcessingStage X2DEFAULT(0),
        __in_opt const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
        __in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL)) PURE;
    STDMETHOD(CreateMasteringVoice) (THIS_ __deref_out IXAudio2MasteringVoice** ppMasteringVoice,
        UINT32 InputChannels X2DEFAULT(XAUDIO2_DEFAULT_CHANNELS),
        UINT32 InputSampleRate X2DEFAULT(XAUDIO2_DEFAULT_SAMPLERATE),
        UINT32 Flags X2DEFAULT(0), UINT32 DeviceIndex X2DEFAULT(0),
        __in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL)) PURE;
    STDMETHOD(StartEngine) (THIS)PURE;
    STDMETHOD_(void, StopEngine) (THIS)PURE;
    STDMETHOD(CommitChanges) (THIS_ UINT32 OperationSet) PURE;
    STDMETHOD_(void, GetPerformanceData) (THIS_ __out XAUDIO2_PERFORMANCE_DATA* pPerfData) PURE;
    STDMETHOD_(void, SetDebugConfiguration) (THIS_ __in_opt const XAUDIO2_DEBUG_CONFIGURATION* pDebugConfiguration,
        __in_opt __reserved void* pReserved X2DEFAULT(NULL)) PURE;
};

HRESULT XAudio27CreateProc(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor)
{
    IXAudio2* pXAudio2;

    HRESULT hr = CoCreateInstance(CLSID_XAudio2, NULL, CLSCTX_INPROC_SERVER, CLSID_IXAudio2, reinterpret_cast<LPVOID*>(&pXAudio2));

    if (SUCCEEDED(hr))
    {
        hr = pXAudio2->Initialize(Flags, XAudio2Processor);

        if (SUCCEEDED(hr))
        {
            *ppXAudio2 = pXAudio2;
        }
        else
        {
            pXAudio2->Release();
        }
    }

    return hr;
}

ULONG IXAudio2Release(IXAudio2* pXAudio2)
{
    return pXAudio2->Release();
}

HRESULT IXAudio2CreateMasteringVoice(IXAudio2* pXAudio2, IXAudio2MasteringVoice** ppMasteringVoice)
{
    return pXAudio2->CreateMasteringVoice(ppMasteringVoice);
}

HRESULT IXAudio2CreateSourceVoice(IXAudio2* pXAudio2, IXAudio2SourceVoice** ppSourceVoice, const WAVEFORMATEX* pSourceFormat)
{
    return pXAudio2->CreateSourceVoice(ppSourceVoice, pSourceFormat);
}

#endif
