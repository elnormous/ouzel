// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_COREAUDIO

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <objc/message.h>
extern "C" id const AVAudioSessionCategoryAmbient;
#endif

#include "AudioDeviceCA.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

#if OUZEL_PLATFORM_MACOS

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
        AudioDeviceCA::AudioDeviceCA():
            AudioDevice(Driver::COREAUDIO)
        {
            OSStatus result;

#if OUZEL_PLATFORM_MACOS
            static const AudioObjectPropertyAddress deviceListAddress = {
                kAudioHardwarePropertyDevices,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };

            if ((result = AudioObjectAddPropertyListener(kAudioObjectSystemObject, &deviceListAddress, deviceListChanged, this)) != noErr)
                throw SystemError("Failed to add CoreAudio property listener, error: " + std::to_string(result));

            static const AudioObjectPropertyAddress defaultDeviceAddress = {
                kAudioHardwarePropertyDefaultOutputDevice,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };

            UInt32 size = sizeof(AudioDeviceID);
            if ((result = AudioObjectGetPropertyData(kAudioObjectSystemObject, &defaultDeviceAddress,
                                                     0, nullptr, &size, &deviceId)) != noErr)
                throw SystemError("Failed to get CoreAudio output device, error: " + std::to_string(result));

            static const AudioObjectPropertyAddress aliveAddress = {
                kAudioDevicePropertyDeviceIsAlive,
                kAudioDevicePropertyScopeOutput,
                kAudioObjectPropertyElementMaster
            };

            UInt32 alive = 0;
            size = sizeof(alive);
            if ((result = AudioObjectGetPropertyData(deviceId, &aliveAddress, 0, nullptr, &size, &alive)) != noErr)
                throw SystemError("Failed to get CoreAudio device status, error: " + std::to_string(result));

            if (!alive)
                throw SystemError("Requested CoreAudio device is not alive, error: " + std::to_string(result));

            static const AudioObjectPropertyAddress hogModeAddress = {
                kAudioDevicePropertyHogMode,
                kAudioDevicePropertyScopeOutput,
                kAudioObjectPropertyElementMaster
            };

            pid_t pid = 0;
            size = sizeof(pid);
            if ((result = AudioObjectGetPropertyData(deviceId, &hogModeAddress, 0, nullptr, &size, &pid)) != noErr)
                throw SystemError("Failed to check if CoreAudio device is in hog mode, error: " + std::to_string(result));

            if (pid != -1)
                throw SystemError("Requested CoreAudio device is being hogged");

            static const AudioObjectPropertyAddress nameAddress = {
                kAudioObjectPropertyName,
                kAudioDevicePropertyScopeOutput,
                kAudioObjectPropertyElementMaster
            };

            CFStringRef tempStringRef = nullptr;
            size = sizeof(CFStringRef);

            if ((result = AudioObjectGetPropertyData(deviceId, &nameAddress,
                                                     0, nullptr, &size, &tempStringRef)) != noErr)
                throw SystemError("Failed to get CoreAudio device name, error: " + std::to_string(result));

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
#endif // OUZEL_PLATFORM_MACOS

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
            id audioSession = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("AVAudioSession"), sel_getUid("sharedInstance"));
            reinterpret_cast<BOOL (*)(id, SEL, id, id)>(&objc_msgSend)(audioSession, sel_getUid("setCategory:error:"), AVAudioSessionCategoryAmbient, nil);
#endif

            AudioComponentDescription desc;
            desc.componentType = kAudioUnitType_Output;

#if OUZEL_PLATFORM_MACOS
            desc.componentSubType = kAudioUnitSubType_DefaultOutput;
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
            desc.componentSubType = kAudioUnitSubType_RemoteIO;
#endif

            desc.componentManufacturer = kAudioUnitManufacturer_Apple;
            desc.componentFlags = 0;
            desc.componentFlagsMask = 0;

            audioComponent = AudioComponentFindNext(nullptr, &desc);

            if (!audioComponent)
                throw SystemError("Failed to find requested CoreAudio component");

#if OUZEL_PLATFORM_MACOS
            if ((result = AudioObjectAddPropertyListener(deviceId, &aliveAddress, deviceUnplugged, this)) != noErr)
                throw SystemError("Failed to add CoreAudio property listener, error: " + std::to_string(result));
#endif

            if ((result = AudioComponentInstanceNew(audioComponent, &audioUnit)) != noErr)
                throw SystemError("Failed to create CoreAudio component instance, error: " + std::to_string(result));

#if OUZEL_PLATFORM_MACOS
            if ((result = AudioUnitSetProperty(audioUnit,
                                               kAudioOutputUnitProperty_CurrentDevice,
                                               kAudioUnitScope_Global, 0,
                                               &deviceId,
                                               sizeof(AudioDeviceID))) != noErr)
                throw SystemError("Failed to set CoreAudio unit property, error: " + std::to_string(result));
#endif

            const AudioUnitElement bus = 0;

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
                    throw SystemError("Failed to set CoreAudio unit stream format, error: " + std::to_string(result));

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
                throw SystemError("Failed to set CoreAudio unit output callback, error: " + std::to_string(result));

            if ((result = AudioUnitInitialize(audioUnit)) != noErr)
                throw SystemError("Failed to initialize CoreAudio unit, error: " + std::to_string(result));

            if ((result = AudioOutputUnitStart(audioUnit)) != noErr)
                throw SystemError("Failed to start CoreAudio output unit, error: " + std::to_string(result));
        }

        AudioDeviceCA::~AudioDeviceCA()
        {
            if (audioUnit)
            {
                AudioOutputUnitStop(audioUnit);

                AURenderCallbackStruct callback;
                callback.inputProc = nullptr;
                callback.inputProcRefCon = nullptr;

                const AudioUnitElement bus = 0;

                AudioUnitSetProperty(audioUnit,
                                     kAudioUnitProperty_SetRenderCallback,
                                     kAudioUnitScope_Input, bus, &callback, sizeof(callback));

                AudioComponentInstanceDispose(audioUnit);
            }

#if OUZEL_PLATFORM_MACOS
            static const AudioObjectPropertyAddress deviceListAddress = {
                kAudioHardwarePropertyDevices,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };

            AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &deviceListAddress, deviceListChanged, this);

            if (deviceId)
            {
                static const AudioObjectPropertyAddress aliveAddress = {
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
