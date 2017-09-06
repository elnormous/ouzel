// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundListener.hpp"
#include "audio/Audio.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        SoundListener::SoundListener():
            Component(TYPE)
        {
        }

        void SoundListener::draw(const Matrix4& transformMatrix,
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

            //sharedEngine->getAudio()->setListenerPosition(transformMatrix.getTranslation());
            //sharedEngine->getAudio()->setListenerRotation(transformMatrix.getRotation());
        }
    } // namespace scene
} // namespace ouzel
