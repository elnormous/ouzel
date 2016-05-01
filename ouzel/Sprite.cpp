// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "Sprite.h"
#include "CompileConfig.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "Utils.h"
#include "Camera.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "File.h"
#include "Layer.h"
#include "Cache.h"

namespace ouzel
{
    namespace scene
    {
        std::shared_ptr<Sprite> Sprite::createFromFile(const std::string& filename, bool mipmaps)
        {
            std::shared_ptr<Sprite> result = std::make_shared<Sprite>();

            if (!result->initFromFile(filename, mipmaps))
            {
                result.reset();
            }

            return result;
        }

        Sprite::Sprite()
        {
            updateCallback = std::make_shared<UpdateCallback>();
            updateCallback->callback = std::bind(&Sprite::update, this, std::placeholders::_1);
        }

        Sprite::~Sprite()
        {
            sharedEngine->unscheduleUpdate(updateCallback);
        }

        bool Sprite::initFromFile(const std::string& filename, bool mipmaps)
        {
            frames.clear();

            boundingBox.reset();

            frames = sharedEngine->getCache()->getSpriteFrames(filename, mipmaps);

            for (const SpriteFramePtr& frame : frames)
            {
                boundingBox.insertPoint(frame->getRectangle().bottomLeft());
                boundingBox.insertPoint(frame->getRectangle().topRight());

                if (frame->getRectangle().width > size.width)
                {
                    size.width = frame->getRectangle().width;
                }
                
                if (frame->getRectangle().height > size.height)
                {
                    size.height = frame->getRectangle().height;
                }
            }

            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);

            if (!blendState)
            {
                return false;
            }

            shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);

            if (!shader)
            {
                return false;
            }

            return true;
        }

        void Sprite::update(float delta)
        {
            if (playing)
            {
                timeSinceLastFrame += delta;

                while (timeSinceLastFrame > frameInterval)
                {
                    timeSinceLastFrame -= frameInterval;
                    ++currentFrame;

                    if (currentFrame >= frames.size())
                    {
                        if (repeat)
                        {
                            currentFrame = 0;
                        }
                        else
                        {
                            currentFrame = static_cast<uint32_t>(frames.size() - 1);
                            playing = false;
                            sharedEngine->unscheduleUpdate(updateCallback);
                        }
                    }
                }
            }
        }

        void Sprite::draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& drawColor)
        {
            Drawable::draw(projectionMatrix, transformMatrix, drawColor);

            if (currentFrame < frames.size())
            {
                sharedEngine->getRenderer()->activateBlendState(blendState);
                sharedEngine->getRenderer()->activateShader(shader);

                Matrix4 modelViewProj = projectionMatrix * transformMatrix;
                float colorVector[] = { drawColor.getR(), drawColor.getG(), drawColor.getB(), drawColor.getA() };

                shader->setVertexShaderConstant(0, sizeof(Matrix4), 1, modelViewProj.m);
                shader->setPixelShaderConstant(0, sizeof(colorVector), 1, colorVector);

                sharedEngine->getRenderer()->activateTexture(frames[currentFrame]->getRexture(), 0);
                sharedEngine->getRenderer()->drawMeshBuffer(frames[currentFrame]->getMeshBuffer());
            }
        }

        void Sprite::setShader(const graphics::ShaderPtr& newShader)
        {
            shader = newShader;
        }

        void Sprite::play(bool pRepeat, float newFrameInterval)
        {
            if (newFrameInterval == 0.0f)
            {
                playing = false;
                return;
            }

            repeat = pRepeat;
            frameInterval = newFrameInterval;

            if (!playing && frames.size() > 1)
            {
                playing = true;

                if (currentFrame >= frames.size() - 1)
                {
                    currentFrame = 0;
                    timeSinceLastFrame = 0.0f;
                }

                sharedEngine->scheduleUpdate(updateCallback);
            }
        }

        void Sprite::stop(bool resetAnimation)
        {
            if (playing)
            {
                playing = false;
                sharedEngine->unscheduleUpdate(updateCallback);
            }

            if (resetAnimation)
            {
                reset();
            }
        }

        void Sprite::reset()
        {
            playing = false;
            currentFrame = 0;
            timeSinceLastFrame = 0.0f;
        }
    } // namespace scene
} // namespace ouzel
