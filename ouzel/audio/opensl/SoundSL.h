// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENSL

#include <vector>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include "audio/SoundResource.h"

namespace ouzel
{
    namespace audio
    {
        class SoundSL: public SoundResource
        {
        public:
            SoundSL();
            virtual ~SoundSL();

            SLPlayItf getPlayer() const { return player; }
            const std::vector<uint8_t>& getBuffer() const { return buffer; }

        protected:
            virtual bool update() override;

            uint32_t channels = 0;
            SLObjectItf playerObject = nullptr;
            SLPlayItf player = nullptr;
            SLAndroidSimpleBufferQueueItf bufferQueue = nullptr;
            SLVolumeItf playerVolume = nullptr;
            std::vector<uint8_t> buffer;
        };
    } // namespace audio
} // namespace ouzel

#endif
