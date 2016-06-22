// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioCA.h"

#if OUZEL_PLATFORM_MACOS
static const AudioObjectPropertyAddress devlistAddress = {
    kAudioHardwarePropertyDevices,
    kAudioObjectPropertyScopeGlobal,
    kAudioObjectPropertyElementMaster
};

static OSStatus deviceListChanged(AudioObjectID systemObj, UInt32 num_addr, const AudioObjectPropertyAddress *addrs, void *data)
{
    //reprocess_device_list(SDL_TRUE, &capture_devs);
    //reprocess_device_list(SDL_FALSE, &output_devs);
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
#endif
            return true;
        }
    } // namespace audio
} // namespace ouzel
