// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_COREAUDIO

#include <system_error>
#if TARGET_OS_IOS || TARGET_OS_TV
#  import <AVFoundation/AVFoundation.h>
#endif

#include "CAAudioDevice.hpp"
#include "CAErrorCategory.hpp"
#include "../../core/Engine.hpp"
#include "../../utils/Log.hpp"

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

namespace ouzel::audio::coreaudio
{
    namespace
    {
        const ErrorCategory errorCategory{};

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
            auto audioDevice = static_cast<ouzel::audio::coreaudio::AudioDevice*>(inRefCon);

            try
            {
                audioDevice->outputCallback(ioData);
            }
            catch (const std::exception& e)
            {
                ouzel::logger.log(ouzel::Log::Level::error) << e.what();
                return -1;
            }

            return noErr;
        }
    }

    AudioDevice::AudioDevice(const Settings& settings,
                             const std::function<void(std::uint32_t frames,
                                                      std::uint32_t channels,
                                                      std::uint32_t sampleRate,
                                                      std::vector<float>& samples)>& initDataGetter):
        audio::AudioDevice(Driver::coreAudio, settings, initDataGetter)
    {
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
            channels = static_cast<std::uint32_t>(maxChannelCount);

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

        if (const auto result = AudioObjectAddPropertyListener(kAudioObjectSystemObject,
                                                               &deviceListAddress,
                                                               deviceListChanged,
                                                               this); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to add CoreAudio property listener");

        constexpr AudioObjectPropertyAddress defaultDeviceAddress = {
            kAudioHardwarePropertyDefaultOutputDevice,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
        };

        UInt32 deviceSize = sizeof(AudioDeviceID);
        if (const auto result = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                                           &defaultDeviceAddress,
                                                           0,
                                                           nullptr,
                                                           &deviceSize,
                                                           &deviceId); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to get CoreAudio output device");

        constexpr AudioObjectPropertyAddress aliveAddress = {
            kAudioDevicePropertyDeviceIsAlive,
            kAudioDevicePropertyScopeOutput,
            kAudioObjectPropertyElementMaster
        };

        UInt32 alive = 0;
        UInt32 aliveSize = sizeof(alive);
        if (const auto result = AudioObjectGetPropertyData(deviceId,
                                                           &aliveAddress,
                                                           0,
                                                           nullptr,
                                                           &aliveSize,
                                                           &alive); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to get CoreAudio device status");

        if (!alive)
            throw std::runtime_error("Requested CoreAudio device is not alive");

        constexpr AudioObjectPropertyAddress hogModeAddress = {
            kAudioDevicePropertyHogMode,
            kAudioDevicePropertyScopeOutput,
            kAudioObjectPropertyElementMaster
        };

        pid_t pid = 0;
        UInt32 pidSize = sizeof(pid);
        if (const auto result = AudioObjectGetPropertyData(deviceId,
                                                           &hogModeAddress,
                                                           0,
                                                           nullptr,
                                                           &pidSize,
                                                           &pid); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to check if CoreAudio device is in hog mode");

        if (pid != -1)
            throw std::runtime_error("Requested CoreAudio device is being hogged");

        constexpr AudioObjectPropertyAddress nameAddress = {
            kAudioObjectPropertyName,
            kAudioDevicePropertyScopeOutput,
            kAudioObjectPropertyElementMaster
        };

        CFStringRef tempStringRef = nullptr;
        UInt32 stringSize = sizeof(CFStringRef);

        if (const auto result = AudioObjectGetPropertyData(deviceId,
                                                           &nameAddress,
                                                           0,
                                                           nullptr,
                                                           &stringSize,
                                                           &tempStringRef); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to get CoreAudio device name");

        if (tempStringRef)
        {
            std::string name;
            if (const char* deviceName = CFStringGetCStringPtr(tempStringRef, kCFStringEncodingUTF8))
                name = deviceName;
            else
            {
                const CFIndex stringLength = CFStringGetLength(tempStringRef);
                std::vector<char> temp(static_cast<std::size_t>(CFStringGetMaximumSizeForEncoding(stringLength, kCFStringEncodingUTF8)) + 1);
                if (CFStringGetCString(tempStringRef, temp.data(), static_cast<CFIndex>(temp.size()), kCFStringEncodingUTF8))
                    name = temp.data();
            }
            CFRelease(tempStringRef);

            logger.log(Log::Level::info) << "Using " << name << " for audio";
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
        if (const auto result = AudioObjectAddPropertyListener(deviceId,
                                                               &aliveAddress,
                                                               deviceUnplugged,
                                                               this); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to add CoreAudio property listener");
#endif

        if (const auto result = AudioComponentInstanceNew(audioComponent,
                                                          &audioUnit); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to create CoreAudio component instance");

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
        if (const auto result = AudioUnitSetProperty(audioUnit,
                                                     kAudioOutputUnitProperty_CurrentDevice,
                                                     kAudioUnitScope_Global,
                                                     0,
                                                     &deviceId,
                                                     sizeof(AudioDeviceID)); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to set CoreAudio unit property");
#endif

        constexpr AudioUnitElement bus = 0U;

        AudioStreamBasicDescription streamDescription;
        streamDescription.mSampleRate = sampleRate;
        streamDescription.mFormatID = kAudioFormatLinearPCM;
        streamDescription.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
        streamDescription.mChannelsPerFrame = channels;
        streamDescription.mFramesPerPacket = 1;
        streamDescription.mBitsPerChannel = sizeof(float) * 8;
        streamDescription.mBytesPerFrame = streamDescription.mBitsPerChannel * streamDescription.mChannelsPerFrame / 8;
        streamDescription.mBytesPerPacket = streamDescription.mBytesPerFrame * streamDescription.mFramesPerPacket;
        streamDescription.mReserved = 0U;

        sampleFormat = SampleFormat::float32;
        sampleSize = sizeof(float);

        if (const auto result = AudioUnitSetProperty(audioUnit,
                                                     kAudioUnitProperty_StreamFormat,
                                                     kAudioUnitScope_Input,
                                                     bus,
                                                     &streamDescription,
                                                     sizeof(streamDescription)); result != noErr)
        {
            logger.log(Log::Level::warning) << "Failed to set CoreAudio unit stream format to float, error: " << result;

            streamDescription.mFormatFlags = kLinearPCMFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
            streamDescription.mBitsPerChannel = sizeof(std::int16_t) * 8;
            streamDescription.mBytesPerFrame = streamDescription.mBitsPerChannel * streamDescription.mChannelsPerFrame / 8;
            streamDescription.mBytesPerPacket = streamDescription.mBytesPerFrame * streamDescription.mFramesPerPacket;

            if (const auto setPropertyResult = AudioUnitSetProperty(audioUnit,
                                                                    kAudioUnitProperty_StreamFormat,
                                                                    kAudioUnitScope_Input,
                                                                    bus,
                                                                    &streamDescription,
                                                                    sizeof(streamDescription)); setPropertyResult != noErr)
                throw std::system_error(setPropertyResult, errorCategory, "Failed to set CoreAudio unit stream format");

            sampleFormat = SampleFormat::signedInt16;
            sampleSize = sizeof(std::int16_t);
        }

        AURenderCallbackStruct callback;
        callback.inputProc = coreaudio::outputCallback;
        callback.inputProcRefCon = this;
        if (const auto result = AudioUnitSetProperty(audioUnit,
                                                     kAudioUnitProperty_SetRenderCallback,
                                                     kAudioUnitScope_Input,
                                                     bus,
                                                     &callback,
                                                     sizeof(callback)); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to set CoreAudio unit output callback");

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
        const UInt32 inIOBufferFrameSize = 512;
        if (const auto result = AudioUnitSetProperty(audioUnit,
                                                     kAudioDevicePropertyBufferFrameSize,
                                                     kAudioUnitScope_Global,
                                                     0,
                                                     &inIOBufferFrameSize,
                                                     sizeof(UInt32)); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to set CoreAudio buffer size");
#endif

        if (const auto result = AudioUnitInitialize(audioUnit); result != noErr)
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

            constexpr AudioUnitElement bus = 0U;

            AudioUnitSetProperty(audioUnit,
                                 kAudioUnitProperty_SetRenderCallback,
                                 kAudioUnitScope_Input,
                                 bus,
                                 &callback,
                                 sizeof(callback));

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

        AudioObjectRemovePropertyListener(kAudioObjectSystemObject,
                                          &deviceListAddress,
                                          deviceListChanged,
                                          this);

        if (deviceId)
        {
            constexpr AudioObjectPropertyAddress aliveAddress = {
                kAudioDevicePropertyDeviceIsAlive,
                kAudioDevicePropertyScopeOutput,
                kAudioObjectPropertyElementMaster
            };

            AudioObjectRemovePropertyListener(deviceId,
                                              &aliveAddress,
                                              deviceUnplugged,
                                              this);
        }
#endif
    }

    void AudioDevice::start()
    {
        if (const auto result = AudioOutputUnitStart(audioUnit); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to start CoreAudio output unit");
    }

    void AudioDevice::stop()
    {
        if (const auto result = AudioOutputUnitStop(audioUnit); result != noErr)
            throw std::system_error(result, errorCategory, "Failed to stop CoreAudio output unit");
    }

    void AudioDevice::outputCallback(AudioBufferList* ioData)
    {
        for (UInt32 i = 0; i < ioData->mNumberBuffers; ++i)
        {
            AudioBuffer& buffer = ioData->mBuffers[i];
            getData(buffer.mDataByteSize / (sampleSize * channels), data);
            std::copy(data.begin(), data.end(), static_cast<std::uint8_t*>(buffer.mData));
        }
    }
}
#endif
