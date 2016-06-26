// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundCA.h"
#include "SoundDataCA.h"
#include "utils/Utils.h"

#if OUZEL_PLATFORM_MACOS
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

static OSStatus outputCallback(void* inRefCon,
                               AudioUnitRenderActionFlags* ioActionFlags,
                               const AudioTimeStamp* inTimeStamp,
                               UInt32 inBusNumber, UInt32 inNumberFrames,
                               AudioBufferList* ioData)
{
    AudioBuffer* abuf;
    uint8_t* ptr;

    ouzel::audio::SoundCA* sound = (ouzel::audio::SoundCA*)inRefCon;

    for (UInt32 i = 0; i < ioData->mNumberBuffers; i++) {
        abuf = &ioData->mBuffers[i];
        ptr = reinterpret_cast<uint8_t*>(abuf->mData);

        if (sound->offset > sound->getSoundData()->getData().size())
        {
            sound->offset = 0;
        }

        int64_t length = sound->getSoundData()->getData().size() - sound->offset;

        if (length >= abuf->mDataByteSize)
        {
            memcpy(ptr, sound->getSoundData()->getData().data() + sound->offset, abuf->mDataByteSize);

            sound->offset += abuf->mDataByteSize;
        }
        else
        {
            if (length > 0)
            {
                memcpy(ptr, sound->getSoundData()->getData().data() + sound->offset, length);
            }

            if (abuf->mDataByteSize - length > 0)
            {
                memset(ptr + abuf->mDataByteSize - length, 0, abuf->mDataByteSize - length);
            }

            sound->offset = 0;
            sound->stop();
        }
    }

    return noErr;
}

namespace ouzel
{
    namespace audio
    {
        SoundCA::SoundCA()
        {
        }

        SoundCA::~SoundCA()
        {
            if (audioUnitOpened)
            {
#if OUZEL_PLATFORM_MACOS
                AudioObjectRemovePropertyListener(deviceID, &aliveAddress, deviceUnplugged, this);
#endif

                AURenderCallbackStruct callback;
                const AudioUnitElement outputBus = 0;
                const AudioUnitElement bus = outputBus;
                const AudioUnitScope scope = kAudioUnitScope_Input;

                AudioOutputUnitStop(audioUnit);

                memset(&callback, 0, sizeof(AURenderCallbackStruct));
                AudioUnitSetProperty(audioUnit,
                                     kAudioUnitProperty_SetRenderCallback,
                                     scope, bus, &callback, sizeof(callback));

                AudioComponentInstanceDispose(audioUnit);
            }
        }

        void SoundCA::free()
        {
            Sound::free();

            if (audioUnitOpened)
            {
#if OUZEL_PLATFORM_MACOS
                AudioObjectRemovePropertyListener(deviceID, &aliveAddress, deviceUnplugged, this);
#endif

                AURenderCallbackStruct callback;
                const AudioUnitElement outputBus = 0;
                const AudioUnitElement bus = outputBus;
                const AudioUnitScope scope = kAudioUnitScope_Input;

                AudioOutputUnitStop(audioUnit);

                memset(&callback, 0, sizeof(AURenderCallbackStruct));
                AudioUnitSetProperty(audioUnit,
                                     kAudioUnitProperty_SetRenderCallback,
                                     scope, bus, &callback, sizeof(callback));

                AudioComponentInstanceDispose(audioUnit);

                audioUnitOpened = 0;
            }
        }

        bool SoundCA::init(const SoundDataPtr& newSoundData)
        {
            if (!Sound::init(newSoundData))
            {
                return false;
            }

            free();

            std::shared_ptr<SoundDataCA> soundDataCA = std::static_pointer_cast<SoundDataCA>(soundData);

            OSStatus result;

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
            result = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr,
                                                0, NULL, &size, &deviceID);

            if (result != noErr)
            {
                ouzel::log("AudioHardwareGetProperty (default device)");
                return false;
            }

            addr.mSelector = kAudioDevicePropertyDeviceIsAlive;
            addr.mScope = kAudioDevicePropertyScopeOutput;

            size = sizeof(alive);
            result = AudioObjectGetPropertyData(deviceID, &addr, 0, NULL, &size, &alive);

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
            result = AudioObjectGetPropertyData(deviceID, &addr, 0, NULL, &size, &pid);

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
            AudioComponent comp = AudioComponentFindNext(NULL, &desc);

            if (!comp)
            {
                log("Couldn't find requested CoreAudio component");
                return false;
            }

            result = AudioComponentInstanceNew(comp, &audioUnit);

            if (result != noErr)
            {
                log("AudioComponentInstanceNew");
                return false;
            }

            audioUnitOpened = 1;

#if OUZEL_PLATFORM_MACOS
            result = AudioUnitSetProperty(audioUnit,
                                          kAudioOutputUnitProperty_CurrentDevice,
                                          kAudioUnitScope_Global, 0,
                                          &deviceID,
                                          sizeof(AudioDeviceID));

            if (result != noErr)
            {
                log("AudioUnitSetProperty (kAudioOutputUnitProperty_CurrentDevice)");
                return false;
            }
#endif

            const AudioUnitElement outputBus = 0;
            const AudioUnitElement bus = outputBus;
            const AudioUnitScope scope = kAudioUnitScope_Input;

            result = AudioUnitSetProperty(audioUnit,
                                          kAudioUnitProperty_StreamFormat,
                                          scope, bus, &soundDataCA->getStreamDescription(), sizeof(soundDataCA->getStreamDescription()));

            if (result != noErr)
            {
                log("AudioUnitSetProperty (kAudioUnitProperty_StreamFormat)");
                return false;
            }

            AURenderCallbackStruct callback;
            memset(&callback, 0, sizeof(AURenderCallbackStruct));
            callback.inputProc = outputCallback;
            callback.inputProcRefCon = this;
            result = AudioUnitSetProperty(audioUnit,
                                          kAudioUnitProperty_SetRenderCallback,
                                          scope, bus, &callback, sizeof(callback));

            if (result != noErr)
            {
                log("AudioUnitSetProperty (kAudioUnitProperty_SetRenderCallback)");
                return false;
            }

            result = AudioUnitInitialize(audioUnit);

            if (result != noErr)
            {
                log("AudioUnitInitialize");
                return false;
            }

#if OUZEL_PLATFORM_MACOS
            AudioObjectAddPropertyListener(deviceID, &aliveAddress, deviceUnplugged, this);
#endif

            ready = true;

            return true;
        }

        bool SoundCA::play(bool repeatSound)
        {
            if (!Sound::play(repeatSound))
            {
                return false;
            }

            OSStatus result = AudioOutputUnitStart(audioUnit);

            if (result != noErr)
            {
                log("AudioOutputUnitStart");
                return false;
            }

            return true;
        }

        bool SoundCA::stop(bool resetSound)
        {
            if (!Sound::stop(resetSound))
            {
                return false;
            }

            OSStatus result = AudioOutputUnitStop(audioUnit);

            if (result != noErr)
            {
                log("AudioOutputUnitStart");
                return false;
            }

            return true;
        }

        bool SoundCA::reset()
        {
            if (!Sound::reset())
            {
                return false;
            }

            offset = 0;

            return true;
        }
    } // namespace audio
} // namespace ouzel
