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
                boundingBox.insertPoint(frame->rectangle.bottomLeft());
                boundingBox.insertPoint(frame->rectangle.topRight());

                if (frame->rectangle.width > size.width)
                {
                    size.width = frame->rectangle.width;
                }
                
                if (frame->rectangle.height > size.height)
                {
                    size.width = frame->rectangle.height;
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
                    currentFrame++;

                    if (repeat && currentFrame >= frames.size())
                    {
                        currentFrame = 0;
                    }
                    else if (!repeat && currentFrame >= frames.size() - 1)
                    {
                        currentFrame = static_cast<uint32_t>(frames.size() - 1);
                        playing = false;
                        sharedEngine->unscheduleUpdate(updateCallback);
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

                sharedEngine->getRenderer()->activateTexture(frames[currentFrame]->texture, 0);
                sharedEngine->getRenderer()->drawMeshBuffer(frames[currentFrame]->meshBuffer);
            }
        }

        /*void Sprite::setOpacity(float newOpacity)
        {
            opacity = newOpacity;

            updateVertexColor();
        }*/

        void Sprite::setShader(const graphics::ShaderPtr& newShader)
        {
            shader = newShader;
        }

        /*void Sprite::setColor(const graphics::Color& newColor)
        {
            color = newColor;

            updateVertexColor();
        }

        void Sprite::updateVertexColor()
        {
            for (uint32_t i = 0; i < frames.size(); ++i)
            {
                for (graphics::VertexPCT& vertex : frames[i].vertices)
                {
                    vertex.color.r = color.r;
                    vertex.color.g = color.g;
                    vertex.color.b = color.b;
                    vertex.color.a = static_cast<uint8_t>(opacity * color.a);
                }

                graphics::MeshBufferPtr meshBuffer = frames[i].meshBuffer;
                meshBuffer->uploadVertices(frames[i].vertices.data(), static_cast<uint32_t>(frames[i].vertices.size()));
            }
        }*/

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

        /*void Sprite::setOffset(const Vector2& newOffset)
        {
            offset = newOffset;
            boundingBox.reset();

            for (size_t i = 0; i < frames.size(); ++i)
            {
                Rectangle rectangle = frames[i].rectangle;

                frames[i].vertices[0].position.x = rectangle.x + offset.x;
                frames[i].vertices[0].position.y = rectangle.y + offset.y;

                frames[i].vertices[1].position.x = rectangle.x + offset.x + rectangle.width;
                frames[i].vertices[1].position.y = rectangle.y + offset.y;

                frames[i].vertices[2].position.x = rectangle.x + offset.x;
                frames[i].vertices[2].position.y = rectangle.y + offset.y + rectangle.height;

                frames[i].vertices[3].position.x = rectangle.x + offset.x + rectangle.width;
                frames[i].vertices[3].position.y = rectangle.y + offset.y + rectangle.height;

                frames[i].meshBuffer->uploadVertices(frames[i].vertices.data(), static_cast<uint32_t>(frames[i].vertices.size()));

                boundingBox.insertPoint(rectangle.bottomLeft() + offset);
                boundingBox.insertPoint(rectangle.topRight() + offset);
            }
        }*/
    } // namespace scene
} // namespace ouzel
