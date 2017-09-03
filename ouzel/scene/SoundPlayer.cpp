// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundPlayer.hpp"
#include "core/Engine.hpp"
#include "core/Cache.hpp"

namespace ouzel
{
    namespace scene
    {
        SoundPlayer::SoundPlayer():
            Component(TYPE)
        {
        }

        SoundPlayer::SoundPlayer(const std::shared_ptr<audio::SoundData>& aSoundData):
            SoundPlayer()
        {
            init(aSoundData);
        }

        SoundPlayer::SoundPlayer(const std::string& filename):
            SoundPlayer()
        {
            init(filename);
        }

        bool SoundPlayer::init(const std::shared_ptr<audio::SoundData>& newSoundData)
        {
            soundData = newSoundData;

            sound = std::make_shared<audio::Sound>();
            if (!sound->init(soundData))
            {
                return false;
            }

            return true;
        }

        bool SoundPlayer::init(const std::string& filename)
        {
            soundData = sharedEngine->getCache()->getSoundData(filename);

            sound = std::make_shared<audio::Sound>();
            if (!sound->init(soundData))
            {
                return false;
            }

            return true;
        }

        void SoundPlayer::draw(const Matrix4& transformMatrix,
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

            if (sound) sound->setPosition(transformMatrix.getTranslation());
        }
    } // namespace scene
} // namespace ouzel
