// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Mixer.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"

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

        void Mixer::addRenderCommands(std::vector<AudioDevice::RenderCommand>& renderCommands)
        {
            AudioDevice::RenderCommand renderCommand;

            for (SoundInput* input : inputs)
                input->addRenderCommands(renderCommand.renderCommands);

            renderCommand.attributeCallback = std::bind(&Mixer::setAttributes,
                                                        std::placeholders::_1,
                                                        std::placeholders::_2,
                                                        std::placeholders::_3,
                                                        std::placeholders::_4,
                                                        std::placeholders::_5,
                                                        pitch,
                                                        gain,
                                                        rolloffScale);

            renderCommands.push_back(renderCommand);
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
