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
            friend SoundOutput;
        public:
            virtual ~SoundInput();

            SoundOutput* getOutput() const { return output; }
            void setOutput(SoundOutput* newOutput);

            virtual AudioDevice::RenderCommand getRenderCommand() = 0;

        protected:
            SoundOutput* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel
