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

            renderCommand.attributeCallback = std::bind(&Mixer::setAttributes,
                                                        std::placeholders::_1,
                                                        std::placeholders::_2,
                                                        std::placeholders::_3,
                                                        std::placeholders::_4,
                                                        std::placeholders::_5,
                                                        pitch,
                                                        gain,
                                                        rolloffScale);

            return renderCommand;
        }

        void Mixer::setAttributes(Vector3&,
                                  Quaternion&,
                                  float& pitch,
                                  float& gain,
                                  float& rolloffFactor,
                                  float pitchScale,
                                  float gainScale,
                                  float rolloffScale)
        {
            pitch *= pitchScale;
            gain *= gainScale;
            rolloffFactor *= rolloffScale;
        }
    } // namespace audio
} // namespace ouzel
