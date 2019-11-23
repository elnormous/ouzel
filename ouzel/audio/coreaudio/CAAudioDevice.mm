// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_COREAUDIO

#include <system_error>
#if TARGET_OS_IOS || TARGET_OS_TV
#  import <AVFoundation/AVFoundation.h>
#endif

#include "CAAudioDevice.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

#if TARGET_OS_IOS || TARGET_OS_TV
@interface RouteChangeDelegate: NSObject

@end

@implementation RouteChangeDelegate
{
    ouzel::audio::coreaudio::AudioDevice* audioDevice;
}

- (id)initWithAudioDevice:(ouzel::audio::coreaudio::AudioDevice*)initAudioDevice
{
    if (self = [super init])
        audioDevice = initAudioDevice;

    return self;
}

- (void)handleRouteChanged:(NSNotification*)notification
{
    switch ([[[notification userInfo] objectForKey:AVAudioSessionRouteChangeReasonKey] unsignedIntegerValue])
    {
        case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
            // TODO: implement
            break;

        case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
            // TODO: implement
            break;
    }
}

@end
#endif

namespace ouzel
{
    namespace audio
    {
        namespace coreaudio
        {
            namespace
            {
                class ErrorCategory final: public std::error_category
                {
                public:
                    const char* name() const noexcept final
                    {
                        return "CoreAudio";
                    }

                    std::string message(int condition) const final
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

                const ErrorCategory errorCategory {};

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
                OSStatus deviceListChanged(AudioObjectID, UInt32, const AudioObjectPropertyAddress*, void*)
                {
                    // TODO: implement
                    return 0;
                }

                OSStatus deviceUnplugged(AudioObjectID, UInt32, const AudioObjectPropertyAddress*, void*)
                {
                    // TODO: implement
                    return noErr;
                }
#endif

                OSStatus outputCallback(void* inRefCon,
                                        AudioUnitRenderActionFlags*,
                                        const AudioTimeStamp*,
                                        UInt32, UInt32,
                                        AudioBufferList* ioData)
                {
                    ouzel::audio::coreaudio::AudioDevice* audioDevice = static_cast<ouzel::audio::coreaudio::AudioDevice*>(inRefCon);

                    try
                    {
                        audioDevice->outputCallback(ioData);
                    }
                    catch (const std::exception& e)
                    {
                        ouzel::engine->log(ouzel::Log::Level::Error) << e.what();
                        return -1;
                    }

                    return noErr;
                }
            }

            AudioDevice::AudioDevice(uint32_t initBufferSize,
                                     uint32_t initSampleRate,
                                     uint32_t initChannels,
                                     const std::function<void(uint32_t frames,
                                                              uint32_t channels,
                                                              uint32_t sampleRate,
                                                              std::vector<float>& samples)>& initDataGetter):
                audio::AudioDevice(Driver::CoreAudio, initBufferSize, initSampleRate, initChannels, initDataGetter)
            {
                OSStatus result;

#if TARGET_OS_IOS || TARGET_OS_TV
                AVAudioSession* audioSession = [AVAudioSession sharedInstance];
                if (![audioSession setCategory:AVAudioSessionCategoryAmbient error:nil])
                    throw std::runtime_error("Failed to set audio session category");

                AVAudioSessionRouteDescription* currentRoute = [audioSession currentRoute];
                NSArray<AVAudioSessionPortDescription*>* outputs = [currentRoute outputs];
                const NSUInteger count = [outputs count];

                NSUInteger maxChannelCount = 0;
                for (NSUInteger outputIndex = 0; outputIndex < count; ++outputIndex)
                {
                    AVAudioSessionPortDescription* output = [outputs objectAtIndex:outputIndex];
                    NSArray<AVAudioSessionChannelDescription*>* channels = [output channels];
                    const NSUInteger channelCount = [channels count];
                    if (channelCount > maxChannelCount)
                        maxChannelCount = channelCount;
                }

                if (channels > maxChannelCount)
                    channels = static_cast<uint32_t>(maxChannelCount);

                routeChangeDelegate = [[RouteChangeDelegate alloc] initWithAudioDevice:this];

                [[NSNotificationCenter defaultCenter] addObserver:routeChangeDelegate
                                                         selector:@selector(handleRouteChanged:)
                                                             name:AVAudioSessionRouteChangeNotification
                                                           object:nil];
#elif TARGET_OS_MAC
                constexpr AudioObjectPropertyAddress deviceListAddress = {
                    kAudioHardwarePropertyDevices,
                    kAudioObjectPropertyScopeGlobal,
                    kAudioObjectPropertyElementMaster
                };

                if ((result = AudioObjectAddPropertyListener(kAudioObjectSystemObject,
                                                             &deviceListAddress,
                                                             deviceListChanged,
                                                             this)) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to add CoreAudio property listener");

                constexpr AudioObjectPropertyAddress defaultDeviceAddress = {
                    kAudioHardwarePropertyDefaultOutputDevice,
                    kAudioObjectPropertyScopeGlobal,
                    kAudioObjectPropertyElementMaster
                };

                UInt32 size = sizeof(AudioDeviceID);
                if ((result = AudioObjectGetPropertyData(kAudioObjectSystemObject, &defaultDeviceAddress,
                                                         0, nullptr, &size, &deviceId)) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to get CoreAudio output device");

                constexpr AudioObjectPropertyAddress aliveAddress = {
                    kAudioDevicePropertyDeviceIsAlive,
                    kAudioDevicePropertyScopeOutput,
                    kAudioObjectPropertyElementMaster
                };

                UInt32 alive = 0;
                size = sizeof(alive);
                if ((result = AudioObjectGetPropertyData(deviceId, &aliveAddress, 0, nullptr, &size, &alive)) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to get CoreAudio device status");

                if (!alive)
                    throw std::system_error(result, errorCategory, "Requested CoreAudio device is not alive");

                constexpr AudioObjectPropertyAddress hogModeAddress = {
                    kAudioDevicePropertyHogMode,
                    kAudioDevicePropertyScopeOutput,
                    kAudioObjectPropertyElementMaster
                };

                pid_t pid = 0;
                size = sizeof(pid);
                if ((result = AudioObjectGetPropertyData(deviceId, &hogModeAddress, 0, nullptr, &size, &pid)) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to check if CoreAudio device is in hog mode");

                if (pid != -1)
                    throw std::runtime_error("Requested CoreAudio device is being hogged");

                constexpr AudioObjectPropertyAddress nameAddress = {
                    kAudioObjectPropertyName,
                    kAudioDevicePropertyScopeOutput,
                    kAudioObjectPropertyElementMaster
                };

                CFStringRef tempStringRef = nullptr;
                size = sizeof(CFStringRef);

                if ((result = AudioObjectGetPropertyData(deviceId, &nameAddress,
                                                         0, nullptr, &size, &tempStringRef)) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to get CoreAudio device name");

                if (tempStringRef)
                {
                    std::string name;
                    if (const char* deviceName = CFStringGetCStringPtr(tempStringRef, kCFStringEncodingUTF8))
                        name = deviceName;
                    else
                    {
                        const CFIndex stringLength = CFStringGetLength(tempStringRef);
                        std::vector<char> temp(static_cast<size_t>(CFStringGetMaximumSizeForEncoding(stringLength, kCFStringEncodingUTF8)) + 1);
                        if (CFStringGetCString(tempStringRef, temp.data(), static_cast<CFIndex>(temp.size()), kCFStringEncodingUTF8))
                            name = temp.data();
                    }
                    CFRelease(tempStringRef);

                    engine->log(Log::Level::Info) << "Using " << name << " for audio";
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
                    throw std::system_error(result, errorCategory, "Failed to add CoreAudio property listener");
#endif

                if ((result = AudioComponentInstanceNew(audioComponent, &audioUnit)) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to create CoreAudio component instance");

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
                if ((result = AudioUnitSetProperty(audioUnit,
                                                   kAudioOutputUnitProperty_CurrentDevice,
                                                   kAudioUnitScope_Global, 0,
                                                   &deviceId,
                                                   sizeof(AudioDeviceID))) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to set CoreAudio unit property");
#endif

                constexpr AudioUnitElement bus = 0;

                AudioStreamBasicDescription streamDescription;
                streamDescription.mSampleRate = sampleRate;
                streamDescription.mFormatID = kAudioFormatLinearPCM;
                streamDescription.mFormatFlags = kLinearPCMFormatFlagIsFloat;
                streamDescription.mChannelsPerFrame = channels;
                streamDescription.mFramesPerPacket = 1;
                streamDescription.mBitsPerChannel = sizeof(float) * 8;
                streamDescription.mBytesPerFrame = streamDescription.mBitsPerChannel * streamDescription.mChannelsPerFrame / 8;
                streamDescription.mBytesPerPacket = streamDescription.mBytesPerFrame * streamDescription.mFramesPerPacket;
                streamDescription.mReserved = 0;

                sampleFormat = SampleFormat::Float32;
                sampleSize = sizeof(float);

                if ((result = AudioUnitSetProperty(audioUnit,
                                                   kAudioUnitProperty_StreamFormat,
                                                   kAudioUnitScope_Input, bus, &streamDescription, sizeof(streamDescription))) != noErr)
                {
                    engine->log(Log::Level::Warning) << "Failed to set CoreAudio unit stream format to float, error: " << result;

                    streamDescription.mFormatFlags = kLinearPCMFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
                    streamDescription.mBitsPerChannel = sizeof(int16_t) * 8;
                    streamDescription.mBytesPerFrame = streamDescription.mBitsPerChannel * streamDescription.mChannelsPerFrame / 8;
                    streamDescription.mBytesPerPacket = streamDescription.mBytesPerFrame * streamDescription.mFramesPerPacket;

                    if ((result = AudioUnitSetProperty(audioUnit,
                                                       kAudioUnitProperty_StreamFormat,
                                                       kAudioUnitScope_Input, bus, &streamDescription, sizeof(streamDescription))) != noErr)
                        throw std::system_error(result, errorCategory, "Failed to set CoreAudio unit stream format");

                    sampleFormat = SampleFormat::SignedInt16;
                    sampleSize = sizeof(int16_t);
                }

                AURenderCallbackStruct callback;
                callback.inputProc = coreaudio::outputCallback;
                callback.inputProcRefCon = this;
                if ((result = AudioUnitSetProperty(audioUnit,
                                                   kAudioUnitProperty_SetRenderCallback,
                                                   kAudioUnitScope_Input, bus, &callback, sizeof(callback))) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to set CoreAudio unit output callback");

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
                const UInt32 inIOBufferFrameSize = 512;
                if ((result = AudioUnitSetProperty(audioUnit,
                                                   kAudioDevicePropertyBufferFrameSize,
                                                   kAudioUnitScope_Global,
                                                   0,
                                                   &inIOBufferFrameSize, sizeof(UInt32))) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to set CoreAudio buffer size");
#endif

                if ((result = AudioUnitInitialize(audioUnit)) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to initialize CoreAudio unit");
            }

            AudioDevice::~AudioDevice()
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

#if TARGET_OS_IOS || TARGET_OS_TV
                if (routeChangeDelegate) [routeChangeDelegate release];
#elif TARGET_OS_MAC
                constexpr AudioObjectPropertyAddress deviceListAddress = {
                    kAudioHardwarePropertyDevices,
                    kAudioObjectPropertyScopeGlobal,
                    kAudioObjectPropertyElementMaster
                };

                AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &deviceListAddress, deviceListChanged, this);

                if (deviceId)
                {
                    constexpr AudioObjectPropertyAddress aliveAddress = {
                        kAudioDevicePropertyDeviceIsAlive,
                        kAudioDevicePropertyScopeOutput,
                        kAudioObjectPropertyElementMaster
                    };

                    AudioObjectRemovePropertyListener(deviceId, &aliveAddress, deviceUnplugged, this);
                }
#endif
            }

            void AudioDevice::start()
            {
                OSStatus result;
                if ((result = AudioOutputUnitStart(audioUnit)) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to start CoreAudio output unit");
            }

            void AudioDevice::stop()
            {
                OSStatus result;
                if ((result = AudioOutputUnitStop(audioUnit)) != noErr)
                    throw std::system_error(result, errorCategory, "Failed to stop CoreAudio output unit");
            }

            void AudioDevice::outputCallback(AudioBufferList* ioData)
            {
                for (UInt32 i = 0; i < ioData->mNumberBuffers; ++i)
                {
                    AudioBuffer& buffer = ioData->mBuffers[i];
                    getData(buffer.mDataByteSize / (sampleSize * channels), data);
                    std::copy(data.begin(), data.end(), static_cast<uint8_t*>(buffer.mData));
                }
            }
        } // namespace coreaudio
    } // namespace audio
} // namespace ouzel

#endif
