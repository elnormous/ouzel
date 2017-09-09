// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Mixer.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        Mixer::Mixer()
        {
        }

        Mixer::~Mixer()
        {
        }

        AudioDevice::RenderCommand Mixer::getRenderCommand()
        {
            AudioDevice::RenderCommand renderCommand;

            for (SoundInput* input : inputs)
            {
                renderCommand.renderCommands.push_back(input->getRenderCommand());
            }

            renderCommand.callback = std::bind(&Mixer::render,
                                               std::placeholders::_1,
                                               std::placeholders::_2,
                                               std::placeholders::_3,
                                               std::placeholders::_4,
                                               std::placeholders::_5,
                                               gain);

            return renderCommand;
        }

        bool Mixer::render(uint32_t,
                           uint16_t,
                           uint32_t,
                           Vector3&,
                           std::vector<float>& result,
                           float gain)
        {
            if (gain != 1.0f)
            {
                for (float& sample : result)
                {
                    sample *= gain;
                }
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
