// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioCA.h"

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

#endif

namespace ouzel
{
    namespace audio
    {
        AudioCA::AudioCA()
        {
        }

        AudioCA::~AudioCA()
        {
#if OUZEL_PLATFORM_MACOS
            AudioObjectAddPropertyListener(kAudioObjectSystemObject, &devlistAddress, deviceListChanged, this);
#endif
        }

        bool AudioCA::init()
        {
#if OUZEL_PLATFORM_MACOS
            AudioObjectAddPropertyListener(kAudioObjectSystemObject, &devlistAddress, deviceListChanged, this);
#else
            AudioSessionInitialize(NULL, NULL, NULL, nil);
            UInt32 category = kAudioSessionCategory_AmbientSound;
            AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(UInt32), &category);
#endif
            return true;
        }
    } // namespace audio
} // namespace ouzel
