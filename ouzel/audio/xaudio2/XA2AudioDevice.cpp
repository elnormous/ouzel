// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include <system_error>
#include "XA2AudioDevice.hpp"
#include "XAudio27.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

#ifndef XAUDIO2_DEBUG_ENGINE
#  define XAUDIO2_DEBUG_ENGINE 0x0001 // Used in XAudio2Create on Windows only
#endif

static constexpr char* XAUDIO2_DLL_28 = "xaudio2_8.dll";
static constexpr char* XAUDIO2_DLL_27 = "xaudio2_7.dll";

typedef HRESULT(__stdcall *XAudio2CreateProc)(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);

namespace ouzel
{
    namespace audio
    {
        class XAudio2ErrorCategory final: public std::error_category
        {
        public:
            const char* name() const noexcept override
            {
                return "XAudio2";
            }

            std::string message(int condition) const override
            {
                switch (condition)
                {
                    case XAUDIO2_E_INVALID_CALL: return "XAUDIO2_E_INVALID_CALL";
                    case XAUDIO2_E_XMA_DECODER_ERROR: return "XAUDIO2_E_XMA_DECODER_ERROR";
                    case XAUDIO2_E_XAPO_CREATION_FAILED: return "XAUDIO2_E_XAPO_CREATION_FAILED";
                    case XAUDIO2_E_DEVICE_INVALIDATED: return "XAUDIO2_E_DEVICE_INVALIDATED";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        const XAudio2ErrorCategory xAudio2ErrorCategory {};

        XA2AudioDevice::XA2AudioDevice(uint32_t initBufferSize,
                                       uint32_t initSampleRate,
                                       uint16_t initChannels,
                                       const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter,
                                       bool debugAudio):
            AudioDevice(Driver::XAUDIO2, initBufferSize, initSampleRate, initChannels, initDataGetter)
        {
            xAudio2Library = LoadLibraryA(XAUDIO2_DLL_28);

            if (xAudio2Library)
            {
                apiMajorVersion = 2;
                apiMinorVersion = 8;

                XAudio2CreateProc xAudio2CreateProc = reinterpret_cast<XAudio2CreateProc>(GetProcAddress(xAudio2Library, "XAudio2Create"));

                if (!xAudio2CreateProc)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get address of XAudio2Create");

                HRESULT hr;
                if (FAILED(hr = xAudio2CreateProc(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR)))
                    throw std::system_error(hr, xAudio2ErrorCategory, "Failed to initialize XAudio2");

                if (debugAudio)
                {
                    XAUDIO2_DEBUG_CONFIGURATION debugConfiguration;
                    debugConfiguration.TraceMask = XAUDIO2_LOG_WARNINGS | XAUDIO2_LOG_DETAIL;
                    debugConfiguration.BreakMask = XAUDIO2_LOG_ERRORS;
                    debugConfiguration.LogThreadID = FALSE;
                    debugConfiguration.LogFileline = FALSE;
                    debugConfiguration.LogFunctionName = FALSE;
                    debugConfiguration.LogTiming = FALSE;

                    xAudio->SetDebugConfiguration(&debugConfiguration);
                }
            }
            else
            {
                engine->log(Log::Level::INFO) << "Failed to load " << XAUDIO2_DLL_28;

                xAudio2Library = LoadLibraryA(XAUDIO2_DLL_27);

                if (xAudio2Library)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 7;
                }
                else
                    throw std::runtime_error("Failed to load " + std::string(XAUDIO2_DLL_27));

                UINT32 flags = 0;
                if (debugAudio) flags |= XAUDIO2_DEBUG_ENGINE;

                HRESULT hr;
                if (FAILED(hr = XAudio27CreateProc(&xAudio, flags, XAUDIO2_DEFAULT_PROCESSOR)))
                    throw std::system_error(hr, xAudio2ErrorCategory, "Failed to initialize XAudio2");
            }

            WAVEFORMATEX waveFormat;
            waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
            waveFormat.nChannels = channels;
            waveFormat.nSamplesPerSec = sampleRate;
            waveFormat.wBitsPerSample = sizeof(float) * 8;
            waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = 0;

            if (apiMajorVersion == 2 && apiMinorVersion == 7)
            {
                HRESULT hr;
                if (FAILED(hr = IXAudio2CreateMasteringVoice(xAudio, &masteringVoice)))
                    throw std::system_error(hr, xAudio2ErrorCategory, "Failed to create XAudio2 mastering voice");

                if (FAILED(hr = IXAudio2CreateSourceVoice(xAudio, &sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this)))
                    throw std::system_error(hr, xAudio2ErrorCategory, "Failed to create source voice");
            }
            else
            {
                HRESULT hr;
                if (FAILED(hr = xAudio->CreateMasteringVoice(&masteringVoice)))
                    throw std::system_error(hr, xAudio2ErrorCategory, "Failed to create XAudio2 mastering voice");

                if (FAILED(hr = xAudio->CreateSourceVoice(&sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this)))
                    throw std::system_error(hr, xAudio2ErrorCategory, "Failed to create source voice");
            }

            sampleFormat = SampleFormat::FLOAT32;

            getData(bufferSize / (channels * sizeof(float)), data[0]);

            XAUDIO2_BUFFER bufferData;
            bufferData.Flags = 0;
            bufferData.AudioBytes = static_cast<UINT32>(data[0].size());
            bufferData.pAudioData = data[0].data();
            bufferData.PlayBegin = 0;
            bufferData.PlayLength = 0;
            bufferData.LoopBegin = 0;
            bufferData.LoopLength = 0;
            bufferData.LoopCount = 0;
            bufferData.pContext = nullptr;

            HRESULT hr;
            if (FAILED(hr = sourceVoice->SubmitSourceBuffer(&bufferData)))
                throw std::system_error(hr, xAudio2ErrorCategory, "Failed to upload sound data");

            getData(bufferSize / (channels * sizeof(float)), data[1]);
            bufferData.AudioBytes = static_cast<UINT32>(data[1].size());
            bufferData.pAudioData = data[1].data();

            if (FAILED(hr = sourceVoice->SubmitSourceBuffer(&bufferData)))
                throw std::system_error(hr, xAudio2ErrorCategory, "Failed to upload sound data");

            nextBuffer = 0;

            if (FAILED(hr = sourceVoice->Start()))
                throw std::system_error(hr, xAudio2ErrorCategory, "Failed to start consuming sound data");

            running = true;
            audioThread = std::thread(&XA2AudioDevice::run, this);
        }

        XA2AudioDevice::~XA2AudioDevice()
        {
            running = false;

            if (audioThread.joinable())
            {
                std::unique_lock<std::mutex> lock(fillDataMutex);
                fillDataCondition.notify_all();
                lock.unlock();
                audioThread.join();
            }

            if (sourceVoice) sourceVoice->DestroyVoice();
            if (masteringVoice) masteringVoice->DestroyVoice();
            if (xAudio)
            {
                if (apiMajorVersion == 2 && apiMinorVersion == 7) IXAudio2Release(xAudio);
                else xAudio->Release();
            }
            if (xAudio2Library) FreeModule(xAudio2Library);
        }

        void XA2AudioDevice::run()
        {
            setCurrentThreadName("Audio");

            for (;;)
            {
                try
                {
                    std::unique_lock<std::mutex> lock(fillDataMutex);
                    while (!fillData && running) fillDataCondition.wait(lock);

                    if (!running) break;

                    getData(bufferSize / (channels * sizeof(float)), data[nextBuffer]);

                    XAUDIO2_BUFFER bufferData;
                    bufferData.Flags = 0;
                    bufferData.AudioBytes = static_cast<UINT32>(data[nextBuffer].size());
                    bufferData.pAudioData = data[nextBuffer].data();
                    bufferData.PlayBegin = 0;
                    bufferData.PlayLength = 0;
                    bufferData.LoopBegin = 0;
                    bufferData.LoopLength = 0;
                    bufferData.LoopCount = 0;
                    bufferData.pContext = nullptr;

                    HRESULT hr;
                    if (FAILED(hr = sourceVoice->SubmitSourceBuffer(&bufferData)))
                        throw std::system_error(hr, xAudio2ErrorCategory, "Failed to upload sound data");

                    nextBuffer = (nextBuffer == 0) ? 1 : 0;

                    fillData = false;
                }
                catch (const std::exception& e)
                {
                    engine->log(Log::Level::ERR) << e.what();
                }
            }
        }

        void XA2AudioDevice::OnVoiceProcessingPassStart(UINT32)
        {
        }

        void XA2AudioDevice::OnVoiceProcessingPassEnd()
        {
        }

        void XA2AudioDevice::OnStreamEnd()
        {
        }

        void XA2AudioDevice::OnBufferStart(void*)
        {
        }

        void XA2AudioDevice::OnBufferEnd(void*)
        {
            std::unique_lock<std::mutex> lock(fillDataMutex);
            fillData = true;
            lock.unlock();
            fillDataCondition.notify_all();
        }

        void XA2AudioDevice::OnLoopEnd(void*)
        {
        }

        void XA2AudioDevice::OnVoiceError(void*, HRESULT error)
        {
            throw std::runtime_error("Xaudio2 voice error: " + std::to_string(error));
        }
    } // namespace audio
} // namespace ouzel

#endif
