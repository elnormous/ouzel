// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Listener.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "SoundInput.hpp"
#include "scene/Actor.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        Listener::Listener(Audio& initAudio):
            scene::Component(scene::Component::LISTENER),
            audio(initAudio)
        {
        }

        Listener::~Listener()
        {
        }

        void Listener::updateTransform()
        {
            transformDirty = true;
        }

        AudioDevice::RenderCommand Listener::getRenderCommand()
        {
            AudioDevice::RenderCommand renderCommand;

            for (SoundInput* input : inputs)
                input->addRenderCommands(renderCommand.renderCommands);

            if (transformDirty && actor)
            {
                position = actor->getWorldPosition();
                const Matrix4& transform = actor->getTransform();
                rotation = transform.getRotation();

                transformDirty = false;
            }

            renderCommand.attributeCallback = std::bind(&Listener::setAttributes,
                                                        std::placeholders::_1,
                                                        std::placeholders::_2,
                                                        std::placeholders::_3,
                                                        std::placeholders::_4,
                                                        std::placeholders::_5,
                                                        position,
                                                        rotation);

            return renderCommand;
        }

        void Listener::setAttributes(Vector3& listenerPosition,
                                     Quaternion& listenerRotation,
                                     float&,
                                     float&,
                                     float&,
                                     const Vector3& position,
                                     const Quaternion& rotation)
        {
            listenerPosition = position;
            listenerRotation = rotation;
        }
    } // namespace audio
} // namespace ouzel
