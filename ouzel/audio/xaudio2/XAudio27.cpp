// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <objbase.h>    // Windows COM declarations

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

#define XAUDIO2_DEFAULT_FREQ_RATIO      2.0f          // Default MaxFrequencyRatio argument
#define XAUDIO2_DEFAULT_CHANNELS        0             // Used in CreateMasteringVoice
#define XAUDIO2_DEFAULT_SAMPLERATE      0             // Used in CreateMasteringVoice

typedef UINT32 XAUDIO2_PROCESSOR;
#define Processor1  0x00000001
#define XAUDIO2_DEFAULT_PROCESSOR Processor1

// Use default arguments if compiling as C++
#ifdef __cplusplus
#define X2DEFAULT(x) =x
#else
#define X2DEFAULT(x)
#endif

#undef INTERFACE
#define INTERFACE IXAudio2
DECLARE_INTERFACE_(IXAudio2, IUnknown)
{
	// NAME: IXAudio2::QueryInterface
	// DESCRIPTION: Queries for a given COM interface on the XAudio2 object.
	//              Only IID_IUnknown and IID_IXAudio2 are supported.
	//
	// ARGUMENTS:
	//  riid - IID of the interface to be obtained.
	//  ppvInterface - Returns a pointer to the requested interface.
	//
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, __deref_out void** ppvInterface) PURE;

	// NAME: IXAudio2::AddRef
	// DESCRIPTION: Adds a reference to the XAudio2 object.
	//
	STDMETHOD_(ULONG, AddRef) (THIS)PURE;

	// NAME: IXAudio2::Release
	// DESCRIPTION: Releases a reference to the XAudio2 object.
	//
	STDMETHOD_(ULONG, Release) (THIS)PURE;

	// NAME: IXAudio2::GetDeviceCount
	// DESCRIPTION: Returns the number of audio output devices available.
	//
	// ARGUMENTS:
	//  pCount - Returns the device count.
	//
	STDMETHOD(GetDeviceCount) (THIS_ __out UINT32* pCount) PURE;

	// NAME: IXAudio2::GetDeviceDetails
	// DESCRIPTION: Returns information about the device with the given index.
	//
	// ARGUMENTS:
	//  Index - Index of the device to be queried.
	//  pDeviceDetails - Returns the device details.
	//
	STDMETHOD(GetDeviceDetails) (THIS_ UINT32 Index, __out XAUDIO2_DEVICE_DETAILS* pDeviceDetails) PURE;

	// NAME: IXAudio2::Initialize
	// DESCRIPTION: Sets global XAudio2 parameters and prepares it for use.
	//
	// ARGUMENTS:
	//  Flags - Flags specifying the XAudio2 object's behavior.  Currently unused.
	//  XAudio2Processor - An XAUDIO2_PROCESSOR enumeration value that specifies
	//  the hardware thread (Xbox) or processor (Windows) that XAudio2 will use.
	//  The enumeration values are platform-specific; platform-independent code
	//  can use XAUDIO2_DEFAULT_PROCESSOR to use the default on each platform.
	//
	STDMETHOD(Initialize) (THIS_ UINT32 Flags X2DEFAULT(0),
		XAUDIO2_PROCESSOR XAudio2Processor X2DEFAULT(XAUDIO2_DEFAULT_PROCESSOR)) PURE;

	// NAME: IXAudio2::RegisterForCallbacks
	// DESCRIPTION: Adds a new client to receive XAudio2's engine callbacks.
	//
	// ARGUMENTS:
	//  pCallback - Callback interface to be called during each processing pass.
	//
	STDMETHOD(RegisterForCallbacks) (__in IXAudio2EngineCallback* pCallback) PURE;

	// NAME: IXAudio2::UnregisterForCallbacks
	// DESCRIPTION: Removes an existing receiver of XAudio2 engine callbacks.
	//
	// ARGUMENTS:
	//  pCallback - Previously registered callback interface to be removed.
	//
	STDMETHOD_(void, UnregisterForCallbacks) (__in IXAudio2EngineCallback* pCallback) PURE;

	// NAME: IXAudio2::CreateSourceVoice
	// DESCRIPTION: Creates and configures a source voice.
	//
	// ARGUMENTS:
	//  ppSourceVoice - Returns the new object's IXAudio2SourceVoice interface.
	//  pSourceFormat - Format of the audio that will be fed to the voice.
	//  Flags - XAUDIO2_VOICE flags specifying the source voice's behavior.
	//  MaxFrequencyRatio - Maximum SetFrequencyRatio argument to be allowed.
	//  pCallback - Optional pointer to a client-provided callback interface.
	//  pSendList - Optional list of voices this voice should send audio to.
	//  pEffectChain - Optional list of effects to apply to the audio data.
	//
	STDMETHOD(CreateSourceVoice) (THIS_ __deref_out IXAudio2SourceVoice** ppSourceVoice,
		__in const WAVEFORMATEX* pSourceFormat,
		UINT32 Flags X2DEFAULT(0),
		float MaxFrequencyRatio X2DEFAULT(XAUDIO2_DEFAULT_FREQ_RATIO),
		__in_opt IXAudio2VoiceCallback* pCallback X2DEFAULT(NULL),
		__in_opt const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
		__in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL)) PURE;

	// NAME: IXAudio2::CreateSubmixVoice
	// DESCRIPTION: Creates and configures a submix voice.
	//
	// ARGUMENTS:
	//  ppSubmixVoice - Returns the new object's IXAudio2SubmixVoice interface.
	//  InputChannels - Number of channels in this voice's input audio data.
	//  InputSampleRate - Sample rate of this voice's input audio data.
	//  Flags - XAUDIO2_VOICE flags specifying the submix voice's behavior.
	//  ProcessingStage - Arbitrary number that determines the processing order.
	//  pSendList - Optional list of voices this voice should send audio to.
	//  pEffectChain - Optional list of effects to apply to the audio data.
	//
	STDMETHOD(CreateSubmixVoice) (THIS_ __deref_out IXAudio2SubmixVoice** ppSubmixVoice,
		UINT32 InputChannels, UINT32 InputSampleRate,
		UINT32 Flags X2DEFAULT(0), UINT32 ProcessingStage X2DEFAULT(0),
		__in_opt const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
		__in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL)) PURE;


	// NAME: IXAudio2::CreateMasteringVoice
	// DESCRIPTION: Creates and configures a mastering voice.
	//
	// ARGUMENTS:
	//  ppMasteringVoice - Returns the new object's IXAudio2MasteringVoice interface.
	//  InputChannels - Number of channels in this voice's input audio data.
	//  InputSampleRate - Sample rate of this voice's input audio data.
	//  Flags - XAUDIO2_VOICE flags specifying the mastering voice's behavior.
	//  DeviceIndex - Identifier of the device to receive the output audio.
	//  pEffectChain - Optional list of effects to apply to the audio data.
	//
	STDMETHOD(CreateMasteringVoice) (THIS_ __deref_out IXAudio2MasteringVoice** ppMasteringVoice,
		UINT32 InputChannels X2DEFAULT(XAUDIO2_DEFAULT_CHANNELS),
		UINT32 InputSampleRate X2DEFAULT(XAUDIO2_DEFAULT_SAMPLERATE),
		UINT32 Flags X2DEFAULT(0), UINT32 DeviceIndex X2DEFAULT(0),
		__in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL)) PURE;

	// NAME: IXAudio2::StartEngine
	// DESCRIPTION: Creates and starts the audio processing thread.
	//
	STDMETHOD(StartEngine) (THIS)PURE;

	// NAME: IXAudio2::StopEngine
	// DESCRIPTION: Stops and destroys the audio processing thread.
	//
	STDMETHOD_(void, StopEngine) (THIS)PURE;

	// NAME: IXAudio2::CommitChanges
	// DESCRIPTION: Atomically applies a set of operations previously tagged
	//              with a given identifier.
	//
	// ARGUMENTS:
	//  OperationSet - Identifier of the set of operations to be applied.
	//
	STDMETHOD(CommitChanges) (THIS_ UINT32 OperationSet) PURE;

	// NAME: IXAudio2::GetPerformanceData
	// DESCRIPTION: Returns current resource usage details: memory, CPU, etc.
	//
	// ARGUMENTS:
	//  pPerfData - Returns the performance data structure.
	//
	STDMETHOD_(void, GetPerformanceData) (THIS_ __out XAUDIO2_PERFORMANCE_DATA* pPerfData) PURE;

	// NAME: IXAudio2::SetDebugConfiguration
	// DESCRIPTION: Configures XAudio2's debug output (in debug builds only).
	//
	// ARGUMENTS:
	//  pDebugConfiguration - Structure describing the debug output behavior.
	//  pReserved - Optional parameter; must be NULL.
	//
	STDMETHOD_(void, SetDebugConfiguration) (THIS_ __in_opt const XAUDIO2_DEBUG_CONFIGURATION* pDebugConfiguration,
		__in_opt __reserved void* pReserved X2DEFAULT(NULL)) PURE;
};

HRESULT XAudio2CreateProc27(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor)
{
	IXAudio2* pXAudio2;

	HRESULT hr = CoCreateInstance({ 0x5a508685, 0xa254, 0x4fba, 0x9b, 0x82, 0x9a, 0x24, 0xb0, 0x03, 0x06, 0xaf }, //__uuidof(XAudio2)
		NULL, CLSCTX_INPROC_SERVER,
		{ 0x8bcf1f58, 0x9fe7, 0x4583, 0x8a, 0xc6, 0xe2, 0xad, 0xc4, 0x65, 0xc8, 0xbb }, //__uuidof(IXAudio2)
		(void**)&pXAudio2);

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