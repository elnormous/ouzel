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
            _updateCallback = std::make_shared<UpdateCallback>();
            _updateCallback->callback = std::bind(&Sprite::update, this, std::placeholders::_1);
        }

        Sprite::~Sprite()
        {
            sharedEngine->unscheduleUpdate(_updateCallback);
        }

        bool Sprite::initFromFile(const std::string& filename, bool mipmaps)
        {
            _frameCount = 0;
            _frameVertices.clear();
            _frameMeshBuffers.clear();

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
                _texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

                if (!_texture)
                {
                    return false;
                }

                _size = _texture->getSize();

                Rectangle rectangle(0, 0, _size.width, _size.height);

                addFrame(rectangle, _size, false, _size, Vector2(), Vector2(0.5f, 0.5f));
            }

            _blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);

            if (!_blendState)
            {
                return false;
            }

            _boundingBox.set(Vector2(-_size.width / 2.0f, -_size.height / 2.0f),
                             Vector2(_size.width / 2.0f, _size.height / 2.0f));

            _shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);

            if (!_shader)
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

            _texture = sharedEngine->getCache()->getTexture(metaObject["image"].GetString(), false, mipmaps);

            const rapidjson::Value& framesArray = document["frames"];

            _frameVertices.reserve(framesArray.Size());
            _frameMeshBuffers.reserve(framesArray.Size());

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

                if (sourceSize.width > _size.width) _size.width = sourceSize.width;
                if (sourceSize.height > _size.height) _size.height = sourceSize.height;

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

            std::vector<graphics::VertexPCT> vertices = {
                graphics::VertexPCT(Vector3(realOffset.x, realOffset.y, 0.0f), _color, textCoords[0]),
                graphics::VertexPCT(Vector3(realOffset.x + rectangle.width, realOffset.y, 0.0f), _color, textCoords[1]),
                graphics::VertexPCT(Vector3(realOffset.x, realOffset.y + rectangle.height, 0.0f),  _color, textCoords[2]),
                graphics::VertexPCT(Vector3(realOffset.x + rectangle.width, realOffset.y + rectangle.height, 0.0f),  _color, textCoords[3])
            };

            _frameVertices.push_back(vertices);

            _frameMeshBuffers.push_back(sharedEngine->getRenderer()->createMeshBufferFromData(indices.data(), sizeof(uint16_t),
                                                                                              static_cast<uint32_t>(indices.size()), false,
                                                                                              vertices.data(), graphics::VertexPCT::ATTRIBUTES,
                                                                                              static_cast<uint32_t>(vertices.size()), true));

            _frameCount++;
        }

        void Sprite::update(float delta)
        {
            if (_playing)
            {
                _timeSinceLastFrame += delta;

                while (_timeSinceLastFrame > _frameInterval)
                {
                    _timeSinceLastFrame -= _frameInterval;
                    _currentFrame++;

                    if (_repeat && _currentFrame >= _frameCount)
                    {
                        _currentFrame = 0;
                    }
                    else if (!_repeat && _currentFrame >= _frameCount - 1)
                    {
                        _currentFrame = _frameCount - 1;
                        _playing = false;
                        sharedEngine->unscheduleUpdate(_updateCallback);
                    }
                }
            }
        }

        void Sprite::draw()
        {
            Node::draw();

            LayerPtr layer = _layer.lock();

            if (_texture && layer)
            {
                sharedEngine->getRenderer()->activateBlendState(_blendState);
                sharedEngine->getRenderer()->activateTexture(_texture, 0);
                sharedEngine->getRenderer()->activateShader(_shader);

                Matrix4 modelViewProj = layer->getCamera()->getViewProjection() * _transform;

                _shader->setVertexShaderConstant(0, { modelViewProj });

                if (_currentFrame < _frameCount)
                {
                    graphics::MeshBufferPtr meshBuffer = _frameMeshBuffers[_currentFrame];

                    sharedEngine->getRenderer()->drawMeshBuffer(meshBuffer);
                }
            }
        }

        void Sprite::setOpacity(float opacity)
        {
            Node::setOpacity(opacity);

            updateVertexColor();
        }

        void Sprite::setTexture(const graphics::TexturePtr& texture)
        {
            _texture = texture;
        }

        void Sprite::setShader(const graphics::ShaderPtr& shader)
        {
            _shader = shader;
        }

        void Sprite::setColor(const graphics::Color& color)
        {
            _color = color;

            updateVertexColor();
        }

        void Sprite::updateVertexColor()
        {
            for (uint32_t i = 0; i < _frameMeshBuffers.size(); ++i)
            {
                for (graphics::VertexPCT& vertex : _frameVertices[i])
                {
                    vertex.color.r = _color.r;
                    vertex.color.g = _color.g;
                    vertex.color.b = _color.b;
                    vertex.color.a = static_cast<uint8_t>(_opacity * _color.a);
                }

                graphics::MeshBufferPtr meshBuffer = _frameMeshBuffers[i];
                meshBuffer->uploadVertices(_frameVertices[i].data(), static_cast<uint32_t>(_frameVertices[i].size()));
            }
        }

        void Sprite::play(bool repeat, float frameInterval)
        {
            _repeat = repeat;
            _frameInterval = frameInterval;

            if (!_playing && _frameCount > 1)
            {
                _playing = true;

                if (_currentFrame >= _frameCount - 1)
                {
                    _currentFrame = 0;
                    _timeSinceLastFrame = 0.0f;
                }

                sharedEngine->scheduleUpdate(_updateCallback);
            }
        }

        void Sprite::stop(bool resetAnimation)
        {
            if (_playing)
            {
                _playing = false;
                sharedEngine->unscheduleUpdate(_updateCallback);
            }

            if (resetAnimation)
            {
                reset();
            }
        }

        void Sprite::reset()
        {
            _playing = false;
            _currentFrame = 0;
            _timeSinceLastFrame = 0.0f;
        }
    } // namespace scene
} // namespace ouzel
