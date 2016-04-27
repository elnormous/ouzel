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
            frameCount = 0;
            frameVertices.clear();
            frameMeshBuffers.clear();

            boundingBox.reset();

            std::string extension = sharedEngine->getFileSystem()->getExtension(filename);

            if (extension == "json")
            {
                if (!loadSpriteSheet(filename, mipmaps))
                {
                    return false;
                }
            }
            else
            {
                texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

                if (!texture)
                {
                    return false;
                }

                size = texture->getSize();

                Rectangle rectangle(0, 0, size.width, size.height);

                addFrame(rectangle, size, false, size, Vector2(), Vector2(0.5f, 0.5f));
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

        bool Sprite::loadSpriteSheet(const std::string& filename, bool mipmaps)
        {
            File file(filename, File::Mode::READ, false);

            if (!file)
            {
                log("Failed to open %s", filename.c_str());
                return false;
            }

            rapidjson::FileReadStream is(file.getFile().get(), TEMP_BUFFER, sizeof(TEMP_BUFFER));

            rapidjson::Document document;
            document.ParseStream<0>(is);

            if (document.HasParseError())
            {
                log("Failed to parse %s", filename.c_str());
                return false;
            }

            const rapidjson::Value& metaObject = document["meta"];
            const rapidjson::Value& sizeObject = metaObject["size"];

            Size2 textureSize(static_cast<float>(sizeObject["w"].GetInt()),
                              static_cast<float>(sizeObject["h"].GetInt()));

            texture = sharedEngine->getCache()->getTexture(metaObject["image"].GetString(), false, mipmaps);

            const rapidjson::Value& framesArray = document["frames"];

            frameVertices.reserve(framesArray.Size());
            frameMeshBuffers.reserve(framesArray.Size());

            for (uint32_t index = 0; index < static_cast<uint32_t>(framesArray.Size()); ++index)
            {
                const rapidjson::Value& frameObject = framesArray[index];

                const rapidjson::Value& rectangleObject = frameObject["frame"];

                Rectangle rectangle(static_cast<float>(rectangleObject["x"].GetInt()),
                                    static_cast<float>(rectangleObject["y"].GetInt()),
                                    static_cast<float>(rectangleObject["w"].GetInt()),
                                    static_cast<float>(rectangleObject["h"].GetInt()));

                bool rotated = frameObject["rotated"].GetBool();

                const rapidjson::Value& sourceSizeObject = frameObject["sourceSize"];

                Size2 sourceSize(static_cast<float>(sourceSizeObject["w"].GetInt()),
                                 static_cast<float>(sourceSizeObject["h"].GetInt()));

                if (sourceSize.width > size.width) size.width = sourceSize.width;
                if (sourceSize.height > size.height) size.height = sourceSize.height;

                const rapidjson::Value& spriteSourceSizeObject = frameObject["spriteSourceSize"];

                Vector2 offset(static_cast<float>(spriteSourceSizeObject["x"].GetInt()),
                               static_cast<float>(spriteSourceSizeObject["y"].GetInt()));

                const rapidjson::Value& pivotObject = frameObject["pivot"];

                Vector2 pivot(static_cast<float>(pivotObject["x"].GetDouble()),
                              static_cast<float>(pivotObject["y"].GetDouble()));

                addFrame(rectangle, textureSize, rotated, sourceSize, offset, pivot);
            }

            return true;
        }

        void Sprite::addFrame(const Rectangle& rectangle, const Size2& textureSize, bool rotated, const Size2& sourceSize, const Vector2& offset, const Vector2& pivot)
        {
            std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};

            Vector2 textCoords[4];
            Vector2 realOffset(-sourceSize.width * pivot.x + offset.x,
                               -sourceSize.height * pivot.y + (sourceSize.height - rectangle.height - offset.y));

            realOffset += offset;

            if (!rotated)
            {
                Vector2 leftTop(rectangle.x / textureSize.width,
                                rectangle.y / textureSize.height);

                Vector2 rightBottom((rectangle.x + rectangle.width) / textureSize.width,
                                    (rectangle.y + rectangle.height) / textureSize.height);

                textCoords[0] = Vector2(leftTop.x, rightBottom.y);
                textCoords[1] = Vector2(rightBottom.x, rightBottom.y);
                textCoords[2] = Vector2(leftTop.x, leftTop.y);
                textCoords[3] = Vector2(rightBottom.x, leftTop.y);
            }
            else
            {
                Vector2 leftTop = Vector2(rectangle.x / textureSize.width,
                                          rectangle.y / textureSize.height);

                Vector2 rightBottom = Vector2((rectangle.x + rectangle.height) / textureSize.width,
                                              (rectangle.y + rectangle.width) / textureSize.height);

                textCoords[0] = Vector2(leftTop.x, leftTop.y);
                textCoords[1] = Vector2(leftTop.x, rightBottom.y);
                textCoords[2] = Vector2(rightBottom.x, leftTop.y);
                textCoords[3] = Vector2(rightBottom.x, rightBottom.y);
            }

            frameRectangles.push_back(Rectangle(realOffset.x, realOffset.y,
                                                 rectangle.width, rectangle.height));

            std::vector<graphics::VertexPCT> vertices = {
                graphics::VertexPCT(Vector3(realOffset.x, realOffset.y, 0.0f), color, textCoords[0]),
                graphics::VertexPCT(Vector3(realOffset.x + rectangle.width, realOffset.y, 0.0f), color, textCoords[1]),
                graphics::VertexPCT(Vector3(realOffset.x, realOffset.y + rectangle.height, 0.0f),  color, textCoords[2]),
                graphics::VertexPCT(Vector3(realOffset.x + rectangle.width, realOffset.y + rectangle.height, 0.0f),  color, textCoords[3])
            };

            frameVertices.push_back(vertices);

            frameMeshBuffers.push_back(sharedEngine->getRenderer()->createMeshBufferFromData(indices.data(), sizeof(uint16_t),
                                                                                              static_cast<uint32_t>(indices.size()), false,
                                                                                              vertices.data(), graphics::VertexPCT::ATTRIBUTES,
                                                                                              static_cast<uint32_t>(vertices.size()), true));

            frameCount++;

            boundingBox.insertPoint(realOffset);
            boundingBox.insertPoint(realOffset + Vector2(rectangle.width, rectangle.height));
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

                    if (repeat && currentFrame >= frameCount)
                    {
                        currentFrame = 0;
                    }
                    else if (!repeat && currentFrame >= frameCount - 1)
                    {
                        currentFrame = frameCount - 1;
                        playing = false;
                        sharedEngine->unscheduleUpdate(updateCallback);
                    }
                }
            }
        }

        void Sprite::draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& color)
        {
            Drawable::draw(projectionMatrix, transformMatrix, color);

            if (texture && currentFrame < frameCount)
            {
                sharedEngine->getRenderer()->activateBlendState(blendState);
                sharedEngine->getRenderer()->activateTexture(texture, 0);
                sharedEngine->getRenderer()->activateShader(shader);

                Matrix4 modelViewProj = projectionMatrix * transformMatrix;
                float colorVector[] = { color.getR(), color.getG(), color.getB(), color.getA() };

                shader->setVertexShaderConstant(0, sizeof(Matrix4), 1, modelViewProj.m);
                shader->setPixelShaderConstant(0, sizeof(colorVector), 1, colorVector);

                graphics::MeshBufferPtr frameMeshBuffer = frameMeshBuffers[currentFrame];
                sharedEngine->getRenderer()->drawMeshBuffer(frameMeshBuffer);
            }
        }

        void Sprite::setOpacity(float opacity)
        {
            opacity = opacity;

            updateVertexColor();
        }

        void Sprite::setTexture(const graphics::TexturePtr& newTexture)
        {
            texture = newTexture;
        }

        void Sprite::setShader(const graphics::ShaderPtr& newShader)
        {
            shader = newShader;
        }

        void Sprite::setColor(const graphics::Color& newColor)
        {
            color = newColor;

            updateVertexColor();
        }

        void Sprite::updateVertexColor()
        {
            for (uint32_t i = 0; i < frameMeshBuffers.size(); ++i)
            {
                for (graphics::VertexPCT& vertex : frameVertices[i])
                {
                    vertex.color.r = color.r;
                    vertex.color.g = color.g;
                    vertex.color.b = color.b;
                    vertex.color.a = static_cast<uint8_t>(opacity * color.a);
                }

                graphics::MeshBufferPtr meshBuffer = frameMeshBuffers[i];
                meshBuffer->uploadVertices(frameVertices[i].data(), static_cast<uint32_t>(frameVertices[i].size()));
            }
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

            if (!playing && frameCount > 1)
            {
                playing = true;

                if (currentFrame >= frameCount - 1)
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

        void Sprite::setOffset(const Vector2& newOffset)
        {
            offset = newOffset;
            boundingBox.reset();

            for (size_t i = 0; i < frameVertices.size(); ++i)
            {
                Rectangle rectangle = frameRectangles[i];

                frameVertices[i][0].position.x = rectangle.x + offset.x;
                frameVertices[i][0].position.y = rectangle.y + offset.y;

                frameVertices[i][1].position.x = rectangle.x + offset.x + rectangle.width;
                frameVertices[i][1].position.y = rectangle.y + offset.y;

                frameVertices[i][2].position.x = rectangle.x + offset.x;
                frameVertices[i][2].position.y = rectangle.y + offset.y + rectangle.height;

                frameVertices[i][3].position.x = rectangle.x + offset.x + rectangle.width;
                frameVertices[i][3].position.y = rectangle.y + offset.y + rectangle.height;

                frameMeshBuffers[i]->uploadVertices(frameVertices[i].data(), static_cast<uint32_t>(frameVertices[i].size()));

                boundingBox.insertPoint(rectangle.bottomLeft() + offset);
                boundingBox.insertPoint(rectangle.topRight() + offset);
            }
        }
    } // namespace scene
} // namespace ouzel
