// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_COREAUDIO

#include <system_error>
#include "AudioDeviceCA.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

#if TARGET_OS_IOS || TARGET_OS_TV
#  include <objc/message.h>
extern "C" id const AVAudioSessionCategoryAmbient;
#elif TARGET_OS_MAC
static OSStatus deviceListChanged(AudioObjectID, UInt32, const AudioObjectPropertyAddress*, void*)
{
    // TODO: implement
    return 0;
}

static OSStatus deviceUnplugged(AudioObjectID, UInt32, const AudioObjectPropertyAddress*, void*)
{
    // TODO: implement
    return noErr;
}
#endif

static OSStatus outputCallback(void* inRefCon,
                               AudioUnitRenderActionFlags*,
                               const AudioTimeStamp*,
                               UInt32, UInt32,
                               AudioBufferList* ioData)
{
    ouzel::audio::AudioDeviceCA* audioDeviceCA = static_cast<ouzel::audio::AudioDeviceCA*>(inRefCon);

    try
    {
        audioDeviceCA->outputCallback(ioData);
    }
    catch (const std::exception& e)
    {
        ouzel::engine->log(ouzel::Log::Level::ERR) << e.what();
        return -1;
    }

    return noErr;
}

namespace ouzel
{
    namespace audio
    {
        class CoreAudioErrorCategory: public std::error_category
        {
        public:
            const char* name() const noexcept override
            {
                return "CoreAudio";
            }

            std::string message(int condition) const override
            {
                switch (condition)
                {
                    case kAudio_UnimplementedError: return "kAudio_UnimplementedError";
                    case kAudio_FileNotFoundError: return "kAudio_FileNotFoundError";
                    case kAudio_FilePermissionError: return "kAudio_FilePermissionError";
                    case kAudio_TooManyFilesOpenError: return "kAudio_TooManyFilesOpenError";
                    case kAudio_BadFilePathError: return "kAudio_BadFilePathError";
                    case kAudio_ParamError: return "kAudio_ParamError";
                    case kAudio_MemFullError: return "kAudio_MemFullError";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        const CoreAudioErrorCategory coreAudioErrorCategory {};

        AudioDeviceCA::AudioDeviceCA(mixer::Mixer& initMixer):
            AudioDevice(Driver::COREAUDIO, initMixer)
        {
            OSStatus result;

#if TARGET_OS_IOS || TARGET_OS_TV
            id audioSession = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("AVAudioSession"), sel_getUid("sharedInstance"));
            reinterpret_cast<BOOL (*)(id, SEL, id, id)>(&objc_msgSend)(audioSession, sel_getUid("setCategory:error:"), AVAudioSessionCategoryAmbient, nil);
#elif TARGET_OS_MAC
            static constexpr AudioObjectPropertyAddress deviceListAddress = {
                kAudioHardwarePropertyDevices,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };

            if ((result = AudioObjectAddPropertyListener(kAudioObjectSystemObject, &deviceListAddress, deviceListChanged, this)) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to add CoreAudio property listener");

            static constexpr AudioObjectPropertyAddress defaultDeviceAddress = {
                kAudioHardwarePropertyDefaultOutputDevice,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };

            UInt32 size = sizeof(AudioDeviceID);
            if ((result = AudioObjectGetPropertyData(kAudioObjectSystemObject, &defaultDeviceAddress,
                                                     0, nullptr, &size, &deviceId)) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to get CoreAudio output device");

            static constexpr AudioObjectPropertyAddress aliveAddress = {
                kAudioDevicePropertyDeviceIsAlive,
                kAudioDevicePropertyScopeOutput,
                kAudioObjectPropertyElementMaster
            };

            UInt32 alive = 0;
            size = sizeof(alive);
            if ((result = AudioObjectGetPropertyData(deviceId, &aliveAddress, 0, nullptr, &size, &alive)) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to get CoreAudio device status");

            if (!alive)
                throw std::system_error(result, coreAudioErrorCategory, "Requested CoreAudio device is not alive");

            static constexpr AudioObjectPropertyAddress hogModeAddress = {
                kAudioDevicePropertyHogMode,
                kAudioDevicePropertyScopeOutput,
                kAudioObjectPropertyElementMaster
            };

            pid_t pid = 0;
            size = sizeof(pid);
            if ((result = AudioObjectGetPropertyData(deviceId, &hogModeAddress, 0, nullptr, &size, &pid)) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to check if CoreAudio device is in hog mode");

            if (pid != -1)
                throw std::runtime_error("Requested CoreAudio device is being hogged");

            static constexpr AudioObjectPropertyAddress nameAddress = {
                kAudioObjectPropertyName,
                kAudioDevicePropertyScopeOutput,
                kAudioObjectPropertyElementMaster
            };

            CFStringRef tempStringRef = nullptr;
            size = sizeof(CFStringRef);

            if ((result = AudioObjectGetPropertyData(deviceId, &nameAddress,
                                                     0, nullptr, &size, &tempStringRef)) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to get CoreAudio device name");

            if (tempStringRef)
            {
                std::string name;
                if (const char* deviceName = CFStringGetCStringPtr(tempStringRef, kCFStringEncodingUTF8))
                    name = deviceName;
                else
                {
                    CFIndex stringLength = CFStringGetLength(tempStringRef);
                    std::vector<char> temp(static_cast<size_t>(CFStringGetMaximumSizeForEncoding(stringLength, kCFStringEncodingUTF8)) + 1);
                    CFStringGetCString(tempStringRef, temp.data(), static_cast<CFIndex>(temp.size()), kCFStringEncodingUTF8);
                    name = temp.data();
                }

                engine->log(Log::Level::INFO) << "Using " << name << " for audio";

                CFRelease(tempStringRef);
            }
#endif

            AudioComponentDescription desc;
            desc.componentType = kAudioUnitType_Output;

#if TARGET_OS_IOS || TARGET_OS_TV
            desc.componentSubType = kAudioUnitSubType_RemoteIO;
#elif TARGET_OS_MAC
            desc.componentSubType = kAudioUnitSubType_DefaultOutput;
#endif

            desc.componentManufacturer = kAudioUnitManufacturer_Apple;
            desc.componentFlags = 0;
            desc.componentFlagsMask = 0;

            audioComponent = AudioComponentFindNext(nullptr, &desc);

            if (!audioComponent)
                throw std::runtime_error("Failed to find requested CoreAudio component");

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
            if ((result = AudioObjectAddPropertyListener(deviceId, &aliveAddress, deviceUnplugged, this)) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to add CoreAudio property listener");
#endif

            if ((result = AudioComponentInstanceNew(audioComponent, &audioUnit)) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to create CoreAudio component instance");

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
            if ((result = AudioUnitSetProperty(audioUnit,
                                               kAudioOutputUnitProperty_CurrentDevice,
                                               kAudioUnitScope_Global, 0,
                                               &deviceId,
                                               sizeof(AudioDeviceID))) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to set CoreAudio unit property");
#endif

            constexpr AudioUnitElement bus = 0;

            AudioStreamBasicDescription streamDescription;
            streamDescription.mSampleRate = sampleRate;
            streamDescription.mFormatID = kAudioFormatLinearPCM;
            streamDescription.mFormatFlags = kLinearPCMFormatFlagIsFloat;
            streamDescription.mChannelsPerFrame = channels;
            streamDescription.mFramesPerPacket = 1;
            streamDescription.mBitsPerChannel = 32;
            streamDescription.mBytesPerFrame = streamDescription.mBitsPerChannel * streamDescription.mChannelsPerFrame / 8;
            streamDescription.mBytesPerPacket = streamDescription.mBytesPerFrame * streamDescription.mFramesPerPacket;
            streamDescription.mReserved = 0;

            if ((result = AudioUnitSetProperty(audioUnit,
                                               kAudioUnitProperty_StreamFormat,
                                               kAudioUnitScope_Input, bus, &streamDescription, sizeof(streamDescription))) != noErr)
            {
                engine->log(Log::Level::WARN) << "Failed to set CoreAudio unit stream format to float, error: " << result;

                streamDescription.mFormatFlags = kLinearPCMFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
                streamDescription.mBitsPerChannel = 16;

                if ((result = AudioUnitSetProperty(audioUnit,
                                                   kAudioUnitProperty_StreamFormat,
                                                   kAudioUnitScope_Input, bus, &streamDescription, sizeof(streamDescription))) != noErr)
                    throw std::system_error(result, coreAudioErrorCategory, "Failed to set CoreAudio unit stream format");

                sampleFormat = SampleFormat::SINT16;
                sampleSize = sizeof(int16_t);
            }
            else
            {
                sampleFormat = SampleFormat::FLOAT32;
                sampleSize = sizeof(float);
            }

            AURenderCallbackStruct callback;
            callback.inputProc = ::outputCallback;
            callback.inputProcRefCon = this;
            if ((result = AudioUnitSetProperty(audioUnit,
                                               kAudioUnitProperty_SetRenderCallback,
                                               kAudioUnitScope_Input, bus, &callback, sizeof(callback))) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to set CoreAudio unit output callback");

            if ((result = AudioUnitInitialize(audioUnit)) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to initialize CoreAudio unit");

            if ((result = AudioOutputUnitStart(audioUnit)) != noErr)
                throw std::system_error(result, coreAudioErrorCategory, "Failed to start CoreAudio output unit");
        }

        AudioDeviceCA::~AudioDeviceCA()
        {
            if (audioUnit)
            {
                AudioOutputUnitStop(audioUnit);

                AURenderCallbackStruct callback;
                callback.inputProc = nullptr;
                callback.inputProcRefCon = nullptr;

                constexpr AudioUnitElement bus = 0;

                AudioUnitSetProperty(audioUnit,
                                     kAudioUnitProperty_SetRenderCallback,
                                     kAudioUnitScope_Input, bus, &callback, sizeof(callback));

                AudioComponentInstanceDispose(audioUnit);
            }

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
            static constexpr AudioObjectPropertyAddress deviceListAddress = {
                kAudioHardwarePropertyDevices,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };

            AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &deviceListAddress, deviceListChanged, this);

            if (deviceId)
            {
                static constexpr AudioObjectPropertyAddress aliveAddress = {
                    kAudioDevicePropertyDeviceIsAlive,
                    kAudioDevicePropertyScopeOutput,
                    kAudioObjectPropertyElementMaster
                };

                AudioObjectRemovePropertyListener(deviceId, &aliveAddress, deviceUnplugged, this);
            }
#endif
        }

        void AudioDeviceCA::outputCallback(AudioBufferList* ioData)
        {
            process();

            for (UInt32 i = 0; i < ioData->mNumberBuffers; ++i)
            {
                AudioBuffer* buffer = &ioData->mBuffers[i];

                getData(buffer->mDataByteSize / (sampleSize * channels), data);

                std::copy(data.begin(), data.end(), static_cast<uint8_t*>(buffer->mData));
            }
        }
    } // namespace audio
} // namespace ouzel

#endif
