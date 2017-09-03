// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.hpp"
#include "audio/Sound.hpp"
#include "audio/SoundData.hpp"

namespace ouzel
{
    namespace scene
    {
        class SoundPlayer: public Component
        {
        public:
            static const uint32_t TYPE = Component::SOUND_PLAYER;

            SoundPlayer();
            SoundPlayer(const std::shared_ptr<audio::SoundData>& aSoundData);
            SoundPlayer(const std::string& filename);

            bool init(const std::shared_ptr<audio::SoundData>& newSoundData);
            bool init(const std::string& filename);

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

            const std::shared_ptr<audio::Sound>& getSound() const { return sound; }

        protected:
            std::shared_ptr<audio::Sound> sound;
            std::shared_ptr<audio::SoundData> soundData;
        };
    } // namespace scene
} // namespace ouzel
