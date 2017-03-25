// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "Sprite.h"
#include "core/CompileConfig.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "graphics/TextureResource.h"
#include "graphics/ShaderResource.h"
#include "utils/Utils.h"
#include "Camera.h"
#include "SceneManager.h"
#include "files/FileSystem.h"
#include "Layer.h"
#include "core/Cache.h"

namespace ouzel
{
    namespace scene
    {
        Sprite::Sprite()
        {
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            whitePixelTexture = sharedEngine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);

            updateCallback.callback = std::bind(&Sprite::update, this, std::placeholders::_1);
        }

        Sprite::Sprite(const std::vector<SpriteFrame>& spriteFrames):
            Sprite()
        {
            initFromSpriteFrames(spriteFrames);
        }

        Sprite::Sprite(const std::string& filename, bool mipmaps):
            Sprite()
        {
            initFromFile(filename, mipmaps);
        }

        bool Sprite::initFromSpriteFrames(const std::vector<SpriteFrame>& spriteFrames)
        {
            frames = spriteFrames;

            updateBoundingBox();

            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);

            if (!blendState)
            {
                return false;
            }

            return true;
        }

        bool Sprite::initFromFile(const std::string& filename, bool mipmaps, uint32_t spritesX, uint32_t spritesY)
        {
            frames = sharedEngine->getCache()->getSpriteFrames(filename, mipmaps, spritesX, spritesY);

            updateBoundingBox();

            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);

            if (!blendState)
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

                while (timeSinceLastFrame > fabsf(frameInterval))
                {
                    timeSinceLastFrame -= fabsf(frameInterval);

                    if (frameInterval > 0.0f)
                    {
                        ++currentFrame;

                        if (currentFrame >= frames.size())
                        {
                            if (repeating)
                            {
                                currentFrame = 0;
                            }
                            else
                            {
                                currentFrame = static_cast<uint32_t>(frames.size() - 1);
                                playing = false;
                                sharedEngine->unscheduleUpdate(&updateCallback);
                            }
                        }
                    }
                    else if (frameInterval < 0.0f)
                    {
                        --currentFrame;

                        if (currentFrame >= frames.size()) // wrap around happened
                        {
                            if (repeating)
                            {
                                currentFrame = static_cast<uint32_t>(frames.size() - 1);
                            }
                            else
                            {
                                currentFrame = 0;
                                playing = false;
                                sharedEngine->unscheduleUpdate(&updateCallback);
                            }
                        }
                    }
                }

                updateBoundingBox();
            }
        }

        void Sprite::draw(const Matrix4& transformMatrix,
                          const Color& drawColor,
                          Camera* camera)
        {
            Component::draw(transformMatrix, drawColor, camera);

            if (currentFrame < frames.size())
            {
                Matrix4 modelViewProj = camera->getRenderViewProjection() * transformMatrix * offsetMatrix;
                float colorVector[] = {drawColor.normR(), drawColor.normG(), drawColor.normB(), drawColor.normA()};

                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

                sharedEngine->getRenderer()->addDrawCommand({frames[currentFrame].getTexture()},
                                                            shader,
                                                            pixelShaderConstants,
                                                            vertexShaderConstants,
                                                            blendState,
                                                            frames[currentFrame].getMeshBuffer(),
                                                            0,
                                                            graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                            0,
                                                            camera->getRenderTarget(),
                                                            camera->getRenderViewport(),
                                                            camera->getDepthWrite(),
                                                            camera->getDepthTest());
            }
        }

        void Sprite::drawWireframe(const Matrix4& transformMatrix,
                                   const Color& drawColor,
                                   Camera* camera)
        {
            Component::drawWireframe(transformMatrix, drawColor, camera);

            if (currentFrame < frames.size())
            {
                Matrix4 modelViewProj = camera->getRenderViewProjection() * transformMatrix * offsetMatrix;
                float colorVector[] = {drawColor.normR(), drawColor.normG(), drawColor.normB(), drawColor.normA()};

                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

                sharedEngine->getRenderer()->addDrawCommand({whitePixelTexture},
                                                            shader,
                                                            pixelShaderConstants,
                                                            vertexShaderConstants,
                                                            blendState,
                                                            frames[currentFrame].getMeshBuffer(),
                                                            0,
                                                            graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                            0,
                                                            camera->getRenderTarget(),
                                                            camera->getRenderViewport(),
                                                            camera->getDepthWrite(),
                                                            camera->getDepthTest(),
                                                            true);
            }
        }

        void Sprite::setOffset(const Vector2& newOffset)
        {
            offset = newOffset;
            Matrix4::createTranslation(offset, offsetMatrix);
            updateBoundingBox();
        }

        void Sprite::play(bool repeat, float newFrameInterval)
        {
            if (newFrameInterval == 0.0f)
            {
                playing = false;
                return;
            }

            repeating = repeat;
            frameInterval = newFrameInterval;

            if (!playing && frames.size() > 1)
            {
                playing = true;

                if (frameInterval > 0.0f)
                {
                    if (currentFrame >= frames.size() - 1)
                    {
                        currentFrame = 0;
                        timeSinceLastFrame = 0.0f;
                    }
                }
                else if (frameInterval < 0.0f)
                {
                    if (currentFrame == 0)
                    {
                        currentFrame = static_cast<uint32_t>(frames.size() - 1);
                        timeSinceLastFrame = 0.0f;
                    }
                }

                updateBoundingBox();

                sharedEngine->scheduleUpdate(&updateCallback);
            }
        }

        void Sprite::stop(bool resetAnimation)
        {
            if (playing)
            {
                playing = false;
                sharedEngine->unscheduleUpdate(&updateCallback);
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

            updateBoundingBox();
        }

        void Sprite::setCurrentFrame(uint32_t frame)
        {
            currentFrame = frame;

            if (currentFrame >= frames.size())
            {
                currentFrame = static_cast<uint32_t>(frames.size() - 1);
            }

            updateBoundingBox();
        }

        void Sprite::updateBoundingBox()
        {
            if (currentFrame < frames.size())
            {
                const SpriteFrame& frame = frames[currentFrame];

                size = frame.getRectangle().size;

                boundingBox = frame.getBoundingBox();
                boundingBox += offset;
            }
            else
            {
                size.v[0] = size.v[1] = 0.0f;
                boundingBox.reset();
            }
        }
    } // namespace scene
} // namespace ouzel
