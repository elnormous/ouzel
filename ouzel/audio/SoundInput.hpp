// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class SoundOutput;

        class SoundInput
        {
        public:
            virtual ~SoundInput();

            void setOutput(SoundOutput* newOutput);

            virtual AudioDevice::RenderCommand getRenderCommand() = 0;

        protected:
            SoundOutput* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel
