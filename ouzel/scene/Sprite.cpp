// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sprite.hpp"
#include "core/Setup.h"
#include "core/Engine.hpp"
#include "graphics/Renderer.hpp"
#include "Camera.hpp"
#include "SceneManager.hpp"
#include "files/FileSystem.hpp"
#include "Layer.hpp"
#include "core/Cache.hpp"

namespace ouzel
{
    namespace scene
    {
        Sprite::Sprite():
            Component(TYPE)
        {
            whitePixelTexture = sharedEngine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);

            updateCallback.callback = std::bind(&Sprite::update, this, std::placeholders::_1);
        }

        Sprite::Sprite(const SpriteDefinition& spriteDefinition):
            Sprite()
        {
            init(spriteDefinition);
        }

        Sprite::Sprite(const std::string& filename, bool mipmaps,
                       uint32_t spritesX, uint32_t spritesY,
                       const Vector2& pivot):
            Sprite()
        {
            init(filename, mipmaps, spritesX, spritesY, pivot);
        }

        Sprite::Sprite(std::shared_ptr<graphics::Texture> texture,
                       uint32_t spritesX, uint32_t spritesY,
                       const Vector2& pivot):
            Sprite()
        {
            init(texture, spritesX, spritesY, pivot);
        }

        bool Sprite::init(const SpriteDefinition& spriteDefinition)
        {
            material = std::make_shared<graphics::Material>();
            material->cullMode = graphics::Renderer::CullMode::NONE;
            material->shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            material->blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            material->textures[0] = spriteDefinition.texture;

            frames = spriteDefinition.frames;

            updateBoundingBox();

            return true;
        }

        bool Sprite::init(const std::string& filename, bool mipmaps,
                          uint32_t spritesX, uint32_t spritesY,
                          const Vector2& pivot)
        {
            material = std::make_shared<graphics::Material>();
            material->cullMode = graphics::Renderer::CullMode::NONE;
            material->shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            material->blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);

            SpriteDefinition spriteDefinition = sharedEngine->getCache()->getSpriteDefinition(filename, mipmaps, spritesX, spritesY, pivot);
            material->textures[0] = spriteDefinition.texture;
            frames = spriteDefinition.frames;

            updateBoundingBox();

            return true;
        }

        bool Sprite::init(std::shared_ptr<graphics::Texture> newTexture,
                          uint32_t spritesX, uint32_t spritesY,
                          const Vector2& pivot)
        {
            material = std::make_shared<graphics::Material>();
            material->cullMode = graphics::Renderer::CullMode::NONE;
            material->shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            material->blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            material->textures[0] = newTexture;
            frames.clear();

            Size2 spriteSize = Size2(material->textures[0]->getSize().width / spritesX,
                                     material->textures[0]->getSize().height / spritesY);

            for (uint32_t x = 0; x < spritesX; ++x)
            {
                for (uint32_t y = 0; y < spritesY; ++y)
                {
                    Rectangle rectangle(spriteSize.width * x,
                                        spriteSize.height * y,
                                        spriteSize.width,
                                        spriteSize.height);

                    scene::SpriteFrame frame = scene::SpriteFrame(newTexture->getFilename(), newTexture->getSize(), rectangle, false, spriteSize, Vector2(), pivot);
                    frames.push_back(frame);
                }
            }

            updateBoundingBox();

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

                                Event resetEvent;
                                resetEvent.type = Event::Type::ANIMATION_RESET;
                                resetEvent.animationEvent.component = this;
                                sharedEngine->getEventDispatcher()->postEvent(resetEvent);
                            }
                            else
                            {
                                currentFrame = static_cast<uint32_t>(frames.size() - 1);
                                playing = false;
                                updateCallback.remove();

                                Event finishEvent;
                                finishEvent.type = Event::Type::ANIMATION_FINISH;
                                finishEvent.animationEvent.component = this;
                                sharedEngine->getEventDispatcher()->postEvent(finishEvent);
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

                                Event resetEvent;
                                resetEvent.type = Event::Type::ANIMATION_RESET;
                                resetEvent.animationEvent.component = this;
                                sharedEngine->getEventDispatcher()->postEvent(resetEvent);
                            }
                            else
                            {
                                currentFrame = 0;
                                playing = false;
                                updateCallback.remove();

                                Event finishEvent;
                                finishEvent.type = Event::Type::ANIMATION_FINISH;
                                finishEvent.animationEvent.component = this;
                                sharedEngine->getEventDispatcher()->postEvent(finishEvent);
                            }
                        }
                    }
                }

                updateBoundingBox();
            }
        }

        void Sprite::draw(const Matrix4& transformMatrix,
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

            if (currentFrame < frames.size() && material)
            {
                Matrix4 modelViewProj = renderViewProjection * transformMatrix * offsetMatrix;
                float colorVector[] = {material->diffuseColor.normR(), material->diffuseColor.normG(), material->diffuseColor.normB(), material->diffuseColor.normA() * opacity};

                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

                std::vector<std::shared_ptr<graphics::Texture>> textures;
                if (wireframe) textures.push_back(whitePixelTexture);
                else textures.assign(std::begin(material->textures), std::end(material->textures));

                sharedEngine->getRenderer()->addDrawCommand(textures,
                                                            material->shader,
                                                            pixelShaderConstants,
                                                            vertexShaderConstants,
                                                            material->blendState,
                                                            frames[currentFrame].getMeshBuffer(),
                                                            0,
                                                            graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                            0,
                                                            renderTarget,
                                                            renderViewport,
                                                            depthWrite,
                                                            depthTest,
                                                            wireframe,
                                                            scissorTest,
                                                            scissorRectangle,
                                                            material->cullMode);
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

                if (currentFrame == 0)
                {
                    Event startEvent;
                    startEvent.type = Event::Type::ANIMATION_START;
                    startEvent.animationEvent.component = this;
                    sharedEngine->getEventDispatcher()->postEvent(startEvent);
                }
            }
        }

        void Sprite::stop(bool resetAnimation)
        {
            if (playing)
            {
                playing = false;
                updateCallback.remove();
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
                size.width = size.height = 0.0f;
                boundingBox.reset();
            }
        }
    } // namespace scene
} // namespace ouzel
