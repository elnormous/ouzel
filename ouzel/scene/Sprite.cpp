// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sprite.hpp"
#include "core/Setup.h"
#include "core/Engine.hpp"
#include "graphics/Renderer.hpp"
#include "Camera.hpp"
#include "SceneManager.hpp"
#include "files/FileSystem.hpp"
#include "Layer.hpp"
#include "assets/Cache.hpp"

namespace ouzel
{
    namespace scene
    {
        Sprite::Sprite():
            Component(TYPE)
        {
            whitePixelTexture = engine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);

            updateCallback.callback = std::bind(&Sprite::update, this, std::placeholders::_1);
        }

        Sprite::Sprite(const SpriteData& spriteData):
            Sprite()
        {
            init(spriteData);
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

        bool Sprite::init(const SpriteData& spriteData)
        {
            material = std::make_shared<graphics::Material>();
            material->cullMode = graphics::Renderer::CullMode::NONE;
            material->blendState = spriteData.blendState ? spriteData.blendState : engine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            material->shader = spriteData.shader ? spriteData.shader : engine->getCache()->getShader(graphics::SHADER_TEXTURE);
            material->textures[0] = spriteData.texture;

            animations = spriteData.animations;

            animationQueue.clear();
            animationQueue.push_back({&animations[""], false});
            currentAnimation = animationQueue.begin();

            updateBoundingBox();

            return true;
        }

        bool Sprite::init(const std::string& filename, bool mipmaps,
                          uint32_t spritesX, uint32_t spritesY,
                          const Vector2& pivot)
        {
            material = std::make_shared<graphics::Material>();
            material->cullMode = graphics::Renderer::CullMode::NONE;
            material->shader = engine->getCache()->getShader(graphics::SHADER_TEXTURE);
            material->blendState = engine->getCache()->getBlendState(graphics::BLEND_ALPHA);

            SpriteData spriteData = engine->getCache()->getSpriteData(filename, mipmaps, spritesX, spritesY, pivot);
            material->textures[0] = spriteData.texture;

            animations = spriteData.animations;

            animationQueue.clear();
            animationQueue.push_back({&animations[""], false});
            currentAnimation = animationQueue.begin();

            updateBoundingBox();

            return true;
        }

        bool Sprite::init(std::shared_ptr<graphics::Texture> newTexture,
                          uint32_t spritesX, uint32_t spritesY,
                          const Vector2& pivot)
        {
            material = std::make_shared<graphics::Material>();
            material->cullMode = graphics::Renderer::CullMode::NONE;
            material->shader = engine->getCache()->getShader(graphics::SHADER_TEXTURE);
            material->blendState = engine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            material->textures[0] = newTexture;
            animations.clear();

            Size2 spriteSize = Size2(material->textures[0]->getSize().width / spritesX,
                                     material->textures[0]->getSize().height / spritesY);

            SpriteData::Animation animation;
            animation.frames.reserve(spritesX * spritesY);

            for (uint32_t x = 0; x < spritesX; ++x)
            {
                for (uint32_t y = 0; y < spritesY; ++y)
                {
                    Rect rectangle(spriteSize.width * x,
                                   spriteSize.height * y,
                                   spriteSize.width,
                                   spriteSize.height);

                    SpriteData::Frame frame = SpriteData::Frame("", newTexture->getSize(), rectangle, false, spriteSize, Vector2(), pivot);
                    animation.frames.push_back(frame);
                }
            }

            animations[""] = std::move(animation);

            animationQueue.clear();
            animationQueue.push_back({&animations[""], false});
            currentAnimation = animationQueue.begin();

            updateBoundingBox();

            return true;
        }

        void Sprite::update(float delta)
        {
            if (currentAnimation->animation->frameInterval > 0.0f && playing)
            {
                timeSinceLastFrame += delta;

                while (timeSinceLastFrame > currentAnimation->animation->frameInterval)
                {
                    timeSinceLastFrame -= currentAnimation->animation->frameInterval;

                    ++currentFrame;

                    if (currentFrame >= currentAnimation->animation->frames.size())
                    {
                        if (currentAnimation->repeat)
                        {
                            currentFrame = 0;

                            Event resetEvent;
                            resetEvent.type = Event::Type::ANIMATION_RESET;
                            resetEvent.animationEvent.component = this;
                            resetEvent.animationEvent.name = currentAnimation->animation->name;
                            engine->getEventDispatcher()->postEvent(resetEvent);
                        }
                        else
                        {
                            Event finishEvent;
                            finishEvent.type = Event::Type::ANIMATION_FINISH;
                            finishEvent.animationEvent.component = this;
                            finishEvent.animationEvent.name = currentAnimation->animation->name;
                            engine->getEventDispatcher()->postEvent(finishEvent);

                            std::list<QueuedAnimation>::iterator nextAnimation = currentAnimation;

                            if (++nextAnimation != animationQueue.end())
                            {
                                currentAnimation = nextAnimation;

                                currentFrame = 0;

                                Event startEvent;
                                startEvent.type = Event::Type::ANIMATION_START;
                                startEvent.animationEvent.component = this;
                                startEvent.animationEvent.name = currentAnimation->animation->name;
                                engine->getEventDispatcher()->postEvent(startEvent);
                            }
                            else
                            {
                                currentFrame = static_cast<uint32_t>(currentAnimation->animation->frames.size() - 1);
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
                          const Rect& renderViewport,
                          bool depthWrite,
                          bool depthTest,
                          bool wireframe,
                          bool scissorTest,
                          const Rect& scissorRectangle)
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

            if (currentFrame < currentAnimation->animation->frames.size() && material)
            {
                Matrix4 modelViewProj = renderViewProjection * transformMatrix * offsetMatrix;
                float colorVector[] = {material->diffuseColor.normR(), material->diffuseColor.normG(), material->diffuseColor.normB(), material->diffuseColor.normA() * opacity * material->opacity};

                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

                std::vector<std::shared_ptr<graphics::Texture>> textures;
                if (wireframe) textures.push_back(whitePixelTexture);
                else textures.assign(std::begin(material->textures), std::end(material->textures));

                engine->getRenderer()->addDrawCommand(textures,
                                                            material->shader,
                                                            pixelShaderConstants,
                                                            vertexShaderConstants,
                                                            material->blendState,
                                                            currentAnimation->animation->frames[currentFrame].getMeshBuffer(),
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

        void Sprite::play()
        {
            if (!playing)
            {
                engine->scheduleUpdate(&updateCallback);
                playing = true;
            }

            if (currentAnimation->animation->frameInterval > 0.0f &&
                currentAnimation->animation->frames.size() > 1)
            {
                if (currentFrame >= currentAnimation->animation->frames.size() - 1)
                {
                    currentFrame = 0;
                    timeSinceLastFrame = 0.0f;
                }

                updateBoundingBox();

                if (currentFrame == 0)
                {
                    Event startEvent;
                    startEvent.type = Event::Type::ANIMATION_START;
                    startEvent.animationEvent.component = this;
                    startEvent.animationEvent.name = currentAnimation->animation->name;
                    engine->getEventDispatcher()->postEvent(startEvent);
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
            currentFrame = 0;
            timeSinceLastFrame = 0.0f;

            updateBoundingBox();
        }

        void Sprite::setCurrentFrame(uint32_t frame)
        {
            currentFrame = frame;

            if (currentFrame >= currentAnimation->animation->frames.size())
            {
                currentFrame = static_cast<uint32_t>(currentAnimation->animation->frames.size() - 1);
            }

            updateBoundingBox();
        }

        bool Sprite::hasAnimation(const std::string& animation) const
        {
            auto i = animations.find(animation);

            return i != animations.end();
        }

        void Sprite::setAnimation(const std::string& newAnimation, bool repeat)
        {
            animationQueue.clear();

            const SpriteData::Animation& animation = animations[newAnimation];
            animationQueue.push_back({&animation, repeat});
            currentAnimation = animationQueue.begin();
            currentFrame = 0;

            updateBoundingBox();
        }

        void Sprite::addAnimation(const std::string& newAnimation, bool repeat)
        {
            const SpriteData::Animation& animation = animations[newAnimation];
            animationQueue.push_back({&animation, repeat});
        }

        void Sprite::updateBoundingBox()
        {
            if (currentFrame < currentAnimation->animation->frames.size())
            {
                const SpriteData::Frame& frame = currentAnimation->animation->frames[currentFrame];

                boundingBox = frame.getBoundingBox();
                boundingBox += offset;
            }
            else
            {
                boundingBox.reset();
            }
        }
    } // namespace scene
} // namespace ouzel
