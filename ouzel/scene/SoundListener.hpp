// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.hpp"

namespace ouzel
{
    namespace scene
    {
        class SoundListener: public Component
        {
        public:
            static const uint32_t TYPE = Component::SOUND_LISTENER;

            SoundListener();

            virtual void draw(const Matrix4& transformMatrix,
                              float opacity,
                              const Matrix4& renderViewProjection,
                              const std::shared_ptr<graphics::Texture>& renderTarget,
                              const Rectangle& renderViewport,
                              bool depthWrite,
                              bool depthTest,
                              bool wireframe,
                              bool scissorTest,
                              const Rectangle& scissorRectangle) override;
        };
    } // namespace scene
} // namespace ouzel
