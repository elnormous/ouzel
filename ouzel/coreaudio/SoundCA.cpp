// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundCA.h"
#include "AudioCA.h"
#include "SoundDataCA.h"
#include "core/Engine.h"
#include "utils/Utils.h"

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
            if (audioUnit)
            {
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

            if (audioUnit)
            {
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

                audioUnit = nullptr;
            }
        }

        bool SoundCA::init(const SoundDataPtr& newSoundData)
        {
            if (!Sound::init(newSoundData))
            {
                return false;
            }

            free();

            std::shared_ptr<AudioCA> audioCA = std::static_pointer_cast<AudioCA>(sharedEngine->getAudio());
            std::shared_ptr<SoundDataCA> soundDataCA = std::static_pointer_cast<SoundDataCA>(soundData);

            OSStatus result;

            result = AudioComponentInstanceNew(audioCA->getAudioComponent(), &audioUnit);

            if (result != noErr)
            {
                log("AudioComponentInstanceNew");
                return false;
            }

#if OUZEL_PLATFORM_MACOS
            AudioDeviceID deviceId = audioCA->getAudioDeviceId();

            result = AudioUnitSetProperty(audioUnit,
                                          kAudioOutputUnitProperty_CurrentDevice,
                                          kAudioUnitScope_Global, 0,
                                          &deviceId,
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
