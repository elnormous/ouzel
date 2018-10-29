// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
            Component(CLASS)
        {
            whitePixelTexture = engine->getCache().getTexture(TEXTURE_WHITE_PIXEL);

            updateHandler.updateHandler = std::bind(&Sprite::handleUpdate, this, std::placeholders::_1);

            currentAnimation = animationQueue.end();
        }

        Sprite::Sprite(const SpriteData& spriteData):
            Sprite()
        {
            init(spriteData);
        }

        Sprite::Sprite(const std::string& filename):
            Sprite()
        {
            init(filename);
        }

        Sprite::Sprite(std::shared_ptr<graphics::Texture> texture,
                       uint32_t spritesX, uint32_t spritesY,
                       const Vector2& pivot):
            Sprite()
        {
            init(texture, spritesX, spritesY, pivot);
        }

        void Sprite::init(const SpriteData& spriteData)
        {
            material = std::make_shared<graphics::Material>();
            material->cullMode = graphics::Renderer::CullMode::NONE;
            material->blendState = spriteData.blendState ? spriteData.blendState : engine->getCache().getBlendState(BLEND_ALPHA);
            material->shader = spriteData.shader ? spriteData.shader : engine->getCache().getShader(SHADER_TEXTURE);
            material->textures[0] = spriteData.texture;

            animations = spriteData.animations;

            animationQueue.clear();
            animationQueue.push_back({&animations[""], false});
            currentAnimation = animationQueue.begin();

            updateBoundingBox();
        }

        void Sprite::init(const std::string& filename)
        {
            material = std::make_shared<graphics::Material>();
            material->cullMode = graphics::Renderer::CullMode::NONE;
            material->shader = engine->getCache().getShader(SHADER_TEXTURE);
            material->blendState = engine->getCache().getBlendState(BLEND_ALPHA);

            if (const SpriteData* spriteData = engine->getCache().getSpriteData(filename))
            {
                material->textures[0] = spriteData->texture;

                animations = spriteData->animations;
            }
            else if (std::shared_ptr<graphics::Texture> texture = engine->getCache().getTexture(filename))
            {
                material->textures[0] = texture;

                SpriteData::Animation animation;

                Rect rectangle(0.0f,
                               0.0f,
                               texture->getSize().width,
                               texture->getSize().height);

                SpriteData::Frame frame = SpriteData::Frame("", texture->getSize(), rectangle, false, texture->getSize(), Vector2(), Vector2(0.5f, 0.5f));
                animation.frames.push_back(frame);

                animations[""] = std::move(animation);
            }

            animationQueue.clear();
            animationQueue.push_back({&animations[""], false});
            currentAnimation = animationQueue.begin();

            updateBoundingBox();
        }

        void Sprite::init(std::shared_ptr<graphics::Texture> newTexture,
                          uint32_t spritesX, uint32_t spritesY,
                          const Vector2& pivot)
        {
            material = std::make_shared<graphics::Material>();
            material->cullMode = graphics::Renderer::CullMode::NONE;
            material->shader = engine->getCache().getShader(SHADER_TEXTURE);
            material->blendState = engine->getCache().getBlendState(BLEND_ALPHA);
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
        }

        void Sprite::update(float delta)
        {
            if (playing)
            {
                currentTime += delta;

                for (; currentAnimation != animationQueue.end(); ++currentAnimation)
                {
                    float length = currentAnimation->animation->frames.size() * currentAnimation->animation->frameInterval;

                    if (length > 0.0F)
                    {
                        if (length > currentTime)
                            break;
                        else
                        {
                            if (currentAnimation->repeat)
                            {
                                currentTime = fmodf(currentTime, length);

                                std::unique_ptr<AnimationEvent> resetEvent(new AnimationEvent());
                                resetEvent->type = Event::Type::ANIMATION_RESET;
                                resetEvent->component = this;
                                resetEvent->name = currentAnimation->animation->name;
                                engine->getEventDispatcher().dispatchEvent(std::move(resetEvent));
                                break;
                            }
                            else
                            {
                                if (running)
                                {
                                    std::unique_ptr<AnimationEvent> finishEvent(new AnimationEvent());
                                    finishEvent->type = Event::Type::ANIMATION_FINISH;
                                    finishEvent->component = this;
                                    finishEvent->name = currentAnimation->animation->name;
                                    engine->getEventDispatcher().dispatchEvent(std::move(finishEvent));
                                }

                                auto nextAnimation = std::next(currentAnimation);
                                if (nextAnimation == animationQueue.end())
                                {
                                    currentTime = length;
                                    running = false;
                                    break;
                                }
                                else
                                {
                                    currentTime -= length;

                                    std::unique_ptr<AnimationEvent> startEvent(new AnimationEvent());
                                    startEvent->type = Event::Type::ANIMATION_START;
                                    startEvent->component = this;
                                    startEvent->name = nextAnimation->animation->name;
                                    engine->getEventDispatcher().dispatchEvent(std::move(startEvent));
                                }
                            }
                        }
                    }
                }

                updateBoundingBox();
            }
        }

        bool Sprite::handleUpdate(const UpdateEvent& event)
        {
            update(event.delta);
            return false;
        }

        void Sprite::draw(const Matrix4& transformMatrix,
                          float opacity,
                          const Matrix4& renderViewProjection,
                          bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            wireframe);

            if (currentAnimation != animationQueue.end() &&
                currentAnimation->animation->frameInterval > 0.0F &&
                !currentAnimation->animation->frames.empty() &&
                material)
            {
                size_t currentFrame = static_cast<size_t>(currentTime / currentAnimation->animation->frameInterval);
                if (currentFrame >= currentAnimation->animation->frames.size()) currentFrame = currentAnimation->animation->frames.size() - 1;

                Matrix4 modelViewProj = renderViewProjection * transformMatrix * offsetMatrix;
                float colorVector[] = {material->diffuseColor.normR(), material->diffuseColor.normG(), material->diffuseColor.normB(), material->diffuseColor.normA() * opacity * material->opacity};

                std::vector<std::vector<float>> fragmentShaderConstants(1);
                fragmentShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

                std::vector<graphics::RenderResource*> textures;
                if (wireframe) textures.push_back(whitePixelTexture->getResource());
                else
                {
                    for (const std::shared_ptr<graphics::Texture>& texture : material->textures)
                        textures.push_back(texture ? texture->getResource() : nullptr);
                }

                engine->getRenderer()->setCullMode(material->cullMode);
                engine->getRenderer()->setPipelineState(material->blendState->getResource(),
                                                        material->shader->getResource());
                engine->getRenderer()->setShaderConstants(fragmentShaderConstants,
                                                          vertexShaderConstants);
                engine->getRenderer()->setTextures(textures);

                const SpriteData::Frame& frame = currentAnimation->animation->frames[currentFrame];

                engine->getRenderer()->draw(frame.getIndexBuffer()->getResource(),
                                            frame.getIndexCount(),
                                            sizeof(uint16_t),
                                            frame.getVertexBuffer()->getResource(),
                                            graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                            0);
            }
        }

        void Sprite::setOffset(const Vector2& newOffset)
        {
            offset = newOffset;
            Matrix4::createTranslation(Vector3(offset), offsetMatrix);
            updateBoundingBox();
        }

        void Sprite::play()
        {
            if (!playing)
            {
                engine->getEventDispatcher().addEventHandler(&updateHandler);
                playing = true;
                running = true;
            }
        }

        void Sprite::stop(bool resetAnimation)
        {
            if (playing)
            {
                playing = false;
                running = false;
                updateHandler.remove();
            }

            if (resetAnimation) reset();
        }

        void Sprite::reset()
        {
            currentTime = 0.0F;
            running = true;

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
            running = true;

            updateBoundingBox();
        }

        void Sprite::addAnimation(const std::string& newAnimation, bool repeat)
        {
            const SpriteData::Animation& animation = animations[newAnimation];
            animationQueue.push_back({&animation, repeat});
            running = true;
        }

        void Sprite::setAnimationProgress(float progress)
        {
            float totalTime = 0.0F;

            for (auto i = animationQueue.begin(); i != animationQueue.end(); ++i)
            {
                totalTime += i->animation->frames.size() * i->animation->frameInterval;

                if (i->repeat) break;
            }

            setAnimationTime(totalTime * progress);
        }

        void Sprite::setAnimationTime(float time)
        {
            currentTime = time;

            for (currentAnimation = animationQueue.begin(); currentAnimation != animationQueue.end(); ++currentAnimation)
            {
                float length = currentAnimation->animation->frames.size() * currentAnimation->animation->frameInterval;

                if (length > 0.0F)
                {
                    if (length > currentTime)
                        break;
                    else
                    {
                        if (currentAnimation->repeat)
                        {
                            currentTime = fmodf(currentTime, length);
                            break;
                        }
                        else if (std::next(currentAnimation) == animationQueue.end())
                        {
                            currentTime = length;
                            break;
                        }
                        else
                            currentTime -= length;
                    }
                }
            }

            running = true;
        }

        void Sprite::updateBoundingBox()
        {
            if (currentAnimation != animationQueue.end() &&
                currentAnimation->animation->frameInterval > 0.0F &&
                !currentAnimation->animation->frames.empty())
            {
                size_t currentFrame = static_cast<size_t>(currentTime / currentAnimation->animation->frameInterval);
                if (currentFrame >= currentAnimation->animation->frames.size()) currentFrame = currentAnimation->animation->frames.size() - 1;

                const SpriteData::Frame& frame = currentAnimation->animation->frames[currentFrame];

                boundingBox = frame.getBoundingBox();
                boundingBox += Vector3(offset);
            }
            else
                boundingBox.reset();
        }
    } // namespace scene
} // namespace ouzel
