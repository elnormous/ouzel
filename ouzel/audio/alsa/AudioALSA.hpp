// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_ALSA

#include <thread>
#include <alsa/asoundlib.h>

#include "audio/Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioALSA: public Audio
        {
            friend Engine;
        public:
            virtual ~AudioALSA();

        protected:
            AudioALSA();
            virtual bool init(bool debugAudio) override;

            void run();

            snd_pcm_t* playbackHandle = nullptr;
            snd_pcm_hw_params_t* hwParams = nullptr;
            snd_pcm_sw_params_t* swParams = nullptr;

            bool running = true;
            std::thread audioThread;
        };
    } // namespace audio
} // namespace ouzel

#endif
