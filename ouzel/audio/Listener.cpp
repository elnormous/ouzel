// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Listener.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "core/Engine.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        Listener::Listener():
            scene::Component(scene::Component::LISTENER)
        {
        }

        Listener::~Listener()
        {
            if (audio) audio->removeListener(this);
        }

        void Listener::draw(const Matrix4& transformMatrix,
                            float opacity,
                            const Matrix4& renderViewProjection,
                            const std::shared_ptr<graphics::Texture>& renderTarget,
                            const Rectangle& renderViewport,
                            bool depthWrite,
                            bool depthTest,
                            bool wireframe,
                            bool scissorTest,
                            const Rectangle& scissorRectangle)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            renderTarget,
                            renderViewport,
                            depthWrite,
                            depthTest,
                            wireframe,
                            scissorTest,
                            scissorRectangle);

            setPosition(transformMatrix.getTranslation());
            setRotation(transformMatrix.getRotation());
        }

        AudioDevice::RenderCommand Listener::getRenderCommand()
        {
            AudioDevice::RenderCommand renderCommand;

            for (SoundInput* input : inputs)
            {
                renderCommand.renderCommands.push_back(input->getRenderCommand());
            }

            renderCommand.attributeCallback = std::bind(&Listener::setAttributes,
                                                        std::placeholders::_1,
                                                        position,
                                                        rotation);

            return renderCommand;
        }

        bool Listener::setAttributes(AudioDevice::RenderCommand::ListenerAttributes& listenerAttributes,
                                     const Vector3& listenerPosition,
                                     const Quaternion& listenerRotation)
        {
            listenerAttributes.position = listenerPosition;
            listenerAttributes.rotation = listenerRotation;

            return true;
        }
    } // namespace audio
} // namespace ouzel
