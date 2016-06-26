// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#import <AVFoundation/AVFoundation.h>
#endif

#include "AudioCA.h"
#include "SoundDataCA.h"
#include "SoundCA.h"
#include "utils/Utils.h"

#if OUZEL_PLATFORM_MACOS
static const AudioObjectPropertyAddress devlistAddress = {
    kAudioHardwarePropertyDevices,
    kAudioObjectPropertyScopeGlobal,
    kAudioObjectPropertyElementMaster
};

static OSStatus deviceListChanged(AudioObjectID systemObj, UInt32 numAddr, const AudioObjectPropertyAddress* addrs, void* data)
{
    return 0;
}

static const AudioObjectPropertyAddress aliveAddress =
{
    kAudioDevicePropertyDeviceIsAlive,
    kAudioObjectPropertyScopeGlobal,
    kAudioObjectPropertyElementMaster
};

static OSStatus deviceUnplugged(AudioObjectID devid, UInt32 num_addr, const AudioObjectPropertyAddress *addrs, void *data)
{
    return noErr;
}
#endif

namespace ouzel
{
    namespace audio
    {
        AudioCA::AudioCA():
            Audio(Driver::COREAUDIO)
        {
        }

        AudioCA::~AudioCA()
        {
#if OUZEL_PLATFORM_MACOS
            AudioObjectAddPropertyListener(kAudioObjectSystemObject, &devlistAddress, deviceListChanged, this);

            if (deviceId)
            {
                AudioObjectRemovePropertyListener(deviceId, &aliveAddress, deviceUnplugged, this);
            }
#endif

        }

        void AudioCA::free()
        {
            Audio::free();

#if OUZEL_PLATFORM_MACOS
            AudioObjectAddPropertyListener(kAudioObjectSystemObject, &devlistAddress, deviceListChanged, this);

            if (deviceId)
            {
                AudioObjectRemovePropertyListener(deviceId, &aliveAddress, deviceUnplugged, this);
                deviceId = 0;
            }
#endif
        }

        bool AudioCA::init()
        {
            if (!Audio::init())
            {
                return false;
            }

            free();

#if OUZEL_PLATFORM_MACOS
            AudioObjectAddPropertyListener(kAudioObjectSystemObject, &devlistAddress, deviceListChanged, this);
#else
            [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:Nil];
#endif

#if OUZEL_PLATFORM_MACOS
            UInt32 size = 0;
            UInt32 alive = 0;
            pid_t pid = 0;

            AudioObjectPropertyAddress addr = {
                0,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };

            size = sizeof(AudioDeviceID);
            addr.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
            OSStatus result = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr,
                                                         0, NULL, &size, &deviceId);

            if (result != noErr)
            {
                ouzel::log("AudioHardwareGetProperty (default device)");
                return false;
            }

            addr.mSelector = kAudioDevicePropertyDeviceIsAlive;
            addr.mScope = kAudioDevicePropertyScopeOutput;

            size = sizeof(alive);
            result = AudioObjectGetPropertyData(deviceId, &addr, 0, NULL, &size, &alive);

            if (result != noErr)
            {
                ouzel::log("AudioDeviceGetProperty (kAudioDevicePropertyDeviceIsAlive)");
                return false;
            }

            if (!alive)
            {
                ouzel::log("CoreAudio: requested device exists, but isn't alive.");
                return false;
            }

            addr.mSelector = kAudioDevicePropertyHogMode;
            size = sizeof(pid);
            result = AudioObjectGetPropertyData(deviceId, &addr, 0, NULL, &size, &pid);

            if ((result == noErr) && (pid != -1))
            {
                ouzel::log("CoreAudio: requested device is being hogged.");
                return false;
            }
#endif

            AudioComponentDescription desc;

            memset(&desc, 0, sizeof(desc));
            desc.componentType = kAudioUnitType_Output;
            desc.componentManufacturer = kAudioUnitManufacturer_Apple;

#if OUZEL_PLATFORM_MACOS
            desc.componentSubType = kAudioUnitSubType_DefaultOutput;
#else
            desc.componentSubType = kAudioUnitSubType_RemoteIO;
#endif
            audioComponent = AudioComponentFindNext(NULL, &desc);

            if (!audioComponent)
            {
                log("Couldn't find requested CoreAudio component");
                return false;
            }

#if OUZEL_PLATFORM_MACOS
            AudioObjectAddPropertyListener(deviceId, &aliveAddress, deviceUnplugged, this);
#endif

            ready = true;
            
            return true;
        }

        SoundDataPtr AudioCA::createSoundData()
        {
            SoundDataPtr soundData(new SoundDataCA());
            return soundData;
        }

        SoundPtr AudioCA::createSound()
        {
            SoundPtr sound(new SoundCA());
            return sound;
        }
    } // namespace audio
} // namespace ouzel
