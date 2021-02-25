// Ouzel by Elviss Strazdins

#include "SpriteRenderer.hpp"
#include "Camera.hpp"
#include "SceneManager.hpp"
#include "Layer.hpp"
#include "../assets/Cache.hpp"
#include "../core/Engine.hpp"
#include "../graphics/Graphics.hpp"
#include "../storage/FileSystem.hpp"
#include "../utils/Utils.hpp"

namespace ouzel::scene
{
    SpriteData::Frame::Frame(const std::string& frameName,
                             const Size2F& textureSize,
                             const RectF& frameRectangle,
                             bool rotated,
                             const Size2F& sourceSize,
                             const Vector2F& sourceOffset,
                             const Vector2F& pivot):
        name(frameName)
    {
        const std::vector<std::uint16_t> indices = {0, 1, 2, 1, 3, 2};
        indexCount = static_cast<std::uint32_t>(indices.size());

        Vector2F textCoords[4];
        const Vector2F finalOffset{
            -sourceSize.v[0] * pivot.v[0] + sourceOffset.v[0],
            -sourceSize.v[1] * pivot.v[1] + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.v[1])
        };

        if (!rotated)
        {
            const Vector2F leftTop{
                frameRectangle.position.v[0] / textureSize.v[0],
                frameRectangle.position.v[1] / textureSize.v[1]
            };

            const Vector2F rightBottom{
                (frameRectangle.position.v[0] + frameRectangle.size.v[0]) / textureSize.v[0],
                (frameRectangle.position.v[1] + frameRectangle.size.v[1]) / textureSize.v[1]
            };

            textCoords[0] = Vector2F(leftTop.v[0], rightBottom.v[1]);
            textCoords[1] = Vector2F(rightBottom.v[0], rightBottom.v[1]);
            textCoords[2] = Vector2F(leftTop.v[0], leftTop.v[1]);
            textCoords[3] = Vector2F(rightBottom.v[0], leftTop.v[1]);
        }
        else
        {
            const auto leftTop = Vector2F{
                frameRectangle.position.v[0] / textureSize.v[0],
                frameRectangle.position.v[1] / textureSize.v[1]
            };

            const auto rightBottom = Vector2F{
                (frameRectangle.position.v[0] + frameRectangle.size.v[1]) / textureSize.v[0],
                (frameRectangle.position.v[1] + frameRectangle.size.v[0]) / textureSize.v[1]
            };

            textCoords[0] = Vector2F(leftTop.v[0], leftTop.v[1]);
            textCoords[1] = Vector2F(leftTop.v[0], rightBottom.v[1]);
            textCoords[2] = Vector2F(rightBottom.v[0], leftTop.v[1]);
            textCoords[3] = Vector2F(rightBottom.v[0], rightBottom.v[1]);
        }

        const std::vector<graphics::Vertex> vertices{
            graphics::Vertex(Vector3F{finalOffset.v[0], finalOffset.v[1], 0.0F}, Color::white(),
                             textCoords[0], Vector3F{0.0F, 0.0F, -1.0F}),
            graphics::Vertex(Vector3F{finalOffset.v[0] + frameRectangle.size.v[0], finalOffset.v[1], 0.0F}, Color::white(),
                             textCoords[1], Vector3F{0.0F, 0.0F, -1.0F}),
            graphics::Vertex(Vector3F{finalOffset.v[0], finalOffset.v[1] + frameRectangle.size.v[1], 0.0F}, Color::white(),
                             textCoords[2], Vector3F{0.0F, 0.0F, -1.0F}),
            graphics::Vertex(Vector3F{finalOffset.v[0] + frameRectangle.size.v[0], finalOffset.v[1] + frameRectangle.size.v[1], 0.0F}, Color::white(),
                             textCoords[3], Vector3F{0.0F, 0.0F, -1.0F})
        };

        boundingBox.min = finalOffset;
        boundingBox.max = finalOffset + Vector2F(frameRectangle.size.v[0], frameRectangle.size.v[1]);

        indexBuffer = std::make_unique<graphics::Buffer>(*engine->getGraphics(),
                                                         graphics::BufferType::index,
                                                         graphics::Flags::none,
                                                         indices.data(),
                                                         static_cast<std::uint32_t>(getVectorSize(indices)));

        vertexBuffer = std::make_unique<graphics::Buffer>(*engine->getGraphics(),
                                                          graphics::BufferType::vertex,
                                                          graphics::Flags::none,
                                                          vertices.data(),
                                                          static_cast<std::uint32_t>(getVectorSize(vertices)));
    }

    SpriteData::Frame::Frame(const std::string& frameName,
                             const std::vector<std::uint16_t>& indices,
                             const std::vector<graphics::Vertex>& vertices):
        name(frameName)
    {
        indexCount = static_cast<std::uint32_t>(indices.size());

        for (const graphics::Vertex& vertex : vertices)
            boundingBox.insertPoint(Vector2F(vertex.position));

        indexBuffer = std::make_unique<graphics::Buffer>(*engine->getGraphics(),
                                                         graphics::BufferType::index,
                                                         graphics::Flags::none,
                                                         indices.data(),
                                                         static_cast<std::uint32_t>(getVectorSize(indices)));

        vertexBuffer = std::make_unique<graphics::Buffer>(*engine->getGraphics(),
                                                          graphics::BufferType::vertex,
                                                          graphics::Flags::none,
                                                          vertices.data(),
                                                          static_cast<std::uint32_t>(getVectorSize(vertices)));
    }

    SpriteData::Frame::Frame(const std::string& frameName,
                             const std::vector<std::uint16_t>& indices,
                             const std::vector<graphics::Vertex>& vertices,
                             const RectF& frameRectangle,
                             const Size2F& sourceSize,
                             const Vector2F& sourceOffset,
                             const Vector2F& pivot):
        name(frameName)
    {
        indexCount = static_cast<std::uint32_t>(indices.size());

        for (const graphics::Vertex& vertex : vertices)
            boundingBox.insertPoint(Vector2F(vertex.position));

        // TODO: fix
        const Vector2F finalOffset{
            -sourceSize.v[0] * pivot.v[0] + sourceOffset.v[0],
            -sourceSize.v[1] * pivot.v[1] + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.v[1])
        };

        indexBuffer = std::make_shared<graphics::Buffer>(*engine->getGraphics(),
                                                         graphics::BufferType::index,
                                                         graphics::Flags::none,
                                                         indices.data(),
                                                         static_cast<std::uint32_t>(getVectorSize(indices)));

        vertexBuffer = std::make_shared<graphics::Buffer>(*engine->getGraphics(),
                                                          graphics::BufferType::vertex,
                                                          graphics::Flags::none,
                                                          vertices.data(),
                                                          static_cast<std::uint32_t>(getVectorSize(vertices)));
    }

    SpriteRenderer::SpriteRenderer()
    {
        updateHandler.updateHandler = [this](const UpdateEvent& event) {
            update(event.delta);
            return false;
        };

        currentAnimation = animationQueue.end();
    }

    SpriteRenderer::SpriteRenderer(const SpriteData& spriteData):
        SpriteRenderer()
    {
        init(spriteData);
    }

    SpriteRenderer::SpriteRenderer(const std::string& filename):
        SpriteRenderer()
    {
        init(filename);
    }

    SpriteRenderer::SpriteRenderer(std::shared_ptr<graphics::Texture> texture,
                                   std::uint32_t spritesX, std::uint32_t spritesY,
                                   const Vector2F& pivot):
        SpriteRenderer()
    {
        init(texture, spritesX, spritesY, pivot);
    }

    void SpriteRenderer::init(const SpriteData& spriteData)
    {
        material = std::make_shared<graphics::Material>();
        material->cullMode = graphics::CullMode::none;
        material->blendState = spriteData.blendState ? spriteData.blendState : engine->getCache().getBlendState(blendAlpha);
        material->shader = spriteData.shader ? spriteData.shader : engine->getCache().getShader(shaderTexture);
        material->textures[0] = spriteData.texture;

        animations = spriteData.animations;

        animationQueue.clear();
        animationQueue.push_back({&animations[""], false});
        currentAnimation = animationQueue.begin();

        updateBoundingBox();
    }

    void SpriteRenderer::init(const std::string& filename)
    {
        material = std::make_shared<graphics::Material>();
        material->cullMode = graphics::CullMode::none;
        material->shader = engine->getCache().getShader(shaderTexture);
        material->blendState = engine->getCache().getBlendState(blendAlpha);

        if (const auto spriteData = engine->getCache().getSpriteData(filename))
        {
            material->textures[0] = spriteData->texture;

            animations = spriteData->animations;
        }
        else if (auto texture = engine->getCache().getTexture(filename))
        {
            material->textures[0] = texture;

            SpriteData::Animation animation;

            const Size2F size(static_cast<float>(texture->getSize().v[0]),
                              static_cast<float>(texture->getSize().v[1]));

            const RectF rectangle(0.0F, 0.0F, size.v[0], size.v[1]);
            animation.frames.emplace_back("", size, rectangle, false, size, Vector2F(), Vector2F{0.5F, 0.5F});

            animations[""] = std::move(animation);
        }

        animationQueue.clear();
        animationQueue.push_back({&animations[""], false});
        currentAnimation = animationQueue.begin();

        updateBoundingBox();
    }

    void SpriteRenderer::init(std::shared_ptr<graphics::Texture> newTexture,
                              std::uint32_t spritesX, std::uint32_t spritesY,
                              const Vector2F& pivot)
    {
        material = std::make_shared<graphics::Material>();
        material->cullMode = graphics::CullMode::none;
        material->shader = engine->getCache().getShader(shaderTexture);
        material->blendState = engine->getCache().getBlendState(blendAlpha);
        material->textures[0] = newTexture;
        animations.clear();

        const Size2F size(static_cast<float>(newTexture->getSize().v[0]),
                          static_cast<float>(newTexture->getSize().v[1]));

        auto spriteSize = Size2F(size.v[0] / spritesX,
                                 size.v[1] / spritesY);

        SpriteData::Animation animation;
        animation.frames.reserve(spritesX * spritesY);

        for (std::uint32_t x = 0; x < spritesX; ++x)
            for (std::uint32_t y = 0; y < spritesY; ++y)
            {
                const RectF rectangle(spriteSize.v[0] * x,
                                      spriteSize.v[1] * y,
                                      spriteSize.v[0],
                                      spriteSize.v[1]);
                animation.frames.emplace_back("", size, rectangle, false, spriteSize, Vector2F(), pivot);
            }

        animations[""] = std::move(animation);

        animationQueue.clear();
        animationQueue.push_back({&animations[""], false});
        currentAnimation = animationQueue.begin();

        updateBoundingBox();
    }

    void SpriteRenderer::update(float delta)
    {
        if (playing)
        {
            currentTime += delta;

            for (; currentAnimation != animationQueue.end(); ++currentAnimation)
            {
                const float length = currentAnimation->animation->frames.size() * currentAnimation->animation->frameInterval;

                if (length > 0.0F)
                {
                    if (length > currentTime)
                        break;
                    else
                    {
                        if (currentAnimation->repeat)
                        {
                            currentTime = std::fmod(currentTime, length);

                            auto resetEvent = std::make_unique<AnimationEvent>();
                            resetEvent->type = Event::Type::animationReset;
                            resetEvent->component = this;
                            resetEvent->name = currentAnimation->animation->name;
                            engine->getEventDispatcher().dispatchEvent(std::move(resetEvent));
                            break;
                        }
                        else
                        {
                            if (running)
                            {
                                auto finishEvent = std::make_unique<AnimationEvent>();
                                finishEvent->type = Event::Type::animationFinish;
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

                                auto startEvent = std::make_unique<AnimationEvent>();
                                startEvent->type = Event::Type::animationStart;
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

    void SpriteRenderer::draw(const Matrix4F& transformMatrix,
                              float opacity,
                              const Matrix4F& renderViewProjection,
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
            auto currentFrame = static_cast<std::size_t>(currentTime / currentAnimation->animation->frameInterval);
            if (currentFrame >= currentAnimation->animation->frames.size())
                currentFrame = currentAnimation->animation->frames.size() - 1;

            const auto modelViewProj = renderViewProjection * transformMatrix * offsetMatrix;
            const float colorVector[] = {
                material->diffuseColor.normR(),
                material->diffuseColor.normG(),
                material->diffuseColor.normB(),
                material->diffuseColor.normA() * opacity * material->opacity
            };

            std::vector<std::vector<float>> fragmentShaderConstants(1);
            fragmentShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

            std::vector<std::size_t> textures;
            textures.reserve(graphics::Material::textureLayers);
            for (const std::shared_ptr<graphics::Texture>& texture : material->textures)
                textures.push_back(texture ? texture->getResource() : 0);

            engine->getGraphics()->setPipelineState(material->blendState->getResource(),
                                                    material->shader->getResource(),
                                                    graphics::CullMode::none,
                                                    wireframe ? graphics::FillMode::wireframe : graphics::FillMode::solid);
            engine->getGraphics()->setShaderConstants(fragmentShaderConstants,
                                                      vertexShaderConstants);
            engine->getGraphics()->setTextures(textures);

            const auto& frame = currentAnimation->animation->frames[currentFrame];

            engine->getGraphics()->draw(frame.getIndexBuffer()->getResource(),
                                        frame.getIndexCount(),
                                        sizeof(std::uint16_t),
                                        frame.getVertexBuffer()->getResource(),
                                        graphics::DrawMode::triangleList,
                                        0);
        }
    }

    void SpriteRenderer::setOffset(const Vector2F& newOffset)
    {
        offset = newOffset;
        offsetMatrix.setTranslation(Vector3F(offset));
        updateBoundingBox();
    }

    void SpriteRenderer::play()
    {
        if (!playing)
        {
            engine->getEventDispatcher().addEventHandler(updateHandler);
            playing = true;
            running = true;
        }
    }

    void SpriteRenderer::stop(bool resetAnimation)
    {
        if (playing)
        {
            playing = false;
            running = false;
            updateHandler.remove();
        }

        if (resetAnimation) reset();
    }

    void SpriteRenderer::reset()
    {
        currentTime = 0.0F;
        running = true;

        updateBoundingBox();
    }

    bool SpriteRenderer::hasAnimation(const std::string& animation) const
    {
        const auto i = animations.find(animation);

        return i != animations.end();
    }

    void SpriteRenderer::setAnimation(const std::string& newAnimation, bool repeat)
    {
        animationQueue.clear();

        const auto& animation = animations[newAnimation];
        animationQueue.push_back({&animation, repeat});
        currentAnimation = animationQueue.begin();
        running = true;

        updateBoundingBox();
    }

    void SpriteRenderer::addAnimation(const std::string& newAnimation, bool repeat)
    {
        const auto& animation = animations[newAnimation];
        animationQueue.push_back({&animation, repeat});
        running = true;
    }

    void SpriteRenderer::setAnimationProgress(float progress)
    {
        float totalTime = 0.0F;

        for (const auto& queuedAnimation : animationQueue)
        {
            totalTime += queuedAnimation.animation->frames.size() * queuedAnimation.animation->frameInterval;

            if (queuedAnimation.repeat) break;
        }

        setAnimationTime(totalTime * progress);
    }

    void SpriteRenderer::setAnimationTime(float time)
    {
        currentTime = time;

        for (currentAnimation = animationQueue.begin(); currentAnimation != animationQueue.end(); ++currentAnimation)
        {
            const float length = currentAnimation->animation->frames.size() * currentAnimation->animation->frameInterval;

            if (length > 0.0F)
            {
                if (length > currentTime)
                    break;
                else
                {
                    if (currentAnimation->repeat)
                    {
                        currentTime = std::fmod(currentTime, length);
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

    void SpriteRenderer::updateBoundingBox()
    {
        if (currentAnimation != animationQueue.end() &&
            !currentAnimation->animation->frames.empty())
        {
            std::size_t currentFrame = 0;

            if (currentAnimation->animation->frameInterval >= 0.0F)
                currentFrame = static_cast<std::size_t>(currentTime / currentAnimation->animation->frameInterval);

            if (currentFrame >= currentAnimation->animation->frames.size()) currentFrame = currentAnimation->animation->frames.size() - 1;

            const auto& frame = currentAnimation->animation->frames[currentFrame];

            boundingBox = Box3F(frame.getBoundingBox());
            boundingBox.min.v[0] += offset.v[0];
            boundingBox.min.v[1] += offset.v[1];

            boundingBox.max.v[0] += offset.v[0];
            boundingBox.max.v[1] += offset.v[1];
        }
        else
            boundingBox.reset();
    }
}
