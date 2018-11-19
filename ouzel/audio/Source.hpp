// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SOURCE_HPP
#define OUZEL_AUDIO_SOURCE_HPP

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class Destination;

        class Source
        {
            friend Destination;
        public:
            virtual ~Source();

            inline Destination* getOutput() const { return output; }
            void setOutput(Destination* newOutput);

            virtual void addRenderCommands(std::vector<AudioDevice::RenderCommand>& renderCommands) {}

        protected:
            Destination* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SOURCE_HPP
