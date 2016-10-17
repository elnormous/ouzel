// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "utils/Types.h"

#ifdef OPENAL
#undef OPENAL
#endif

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class Audio
        {
            friend Engine;
        public:
            enum class Driver
            {
                DEFAULT,
                EMPTY,
                OPENAL,
                XAUDIO2,
                OPENSL
            };

            virtual ~Audio();
            virtual void free();

            virtual bool init();

            virtual SoundDataPtr createSoundData() = 0;
            virtual SoundPtr createSound() = 0;

            uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            void setAPIVersion(uint16_t majorVersion, uint16_t minorVersion)
            {
                apiMajorVersion = majorVersion;
                apiMinorVersion = minorVersion;
            }

            bool isReady() const { return ready; }

        protected:
            Audio(Driver aDriver);

            Driver driver;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            bool ready = false;
        };
    } // namespace audio
} // namespace ouzel
