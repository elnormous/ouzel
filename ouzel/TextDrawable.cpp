// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextDrawable.h"
#include "Engine.h"
#include "Renderer.h"
#include "Cache.h"
#include "Utils.h"

namespace ouzel
{
    namespace scene
    {
        std::shared_ptr<TextDrawable> TextDrawable::create(const std::string& font, const std::string& text, const Vector2& textAnchor)
        {
            std::shared_ptr<TextDrawable> result = std::make_shared<TextDrawable>();

            if (!result->init(font, text, textAnchor))
            {
                result.reset();
            }

            return result;
        }

        TextDrawable::TextDrawable()
        {
            _shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
        }

        TextDrawable::~TextDrawable()
        {

        }

        bool TextDrawable::init(const std::string& font, const std::string& text, const Vector2& textAnchor)
        {
            _font.loadFont(font);
            _textAnchor = textAnchor;

            _texture = _font.getTexture();

            if (!_texture)
            {
                return false;
            }

            setText(text);

            return true;
        }

        void TextDrawable::draw(const Matrix4& projection, const Matrix4& transform, const graphics::Color& color)
        {
            Drawable::draw(projection, transform, color);

            if (_shader && _texture && _meshBuffer)
            {
                sharedEngine->getRenderer()->activateTexture(_texture, 0);
                sharedEngine->getRenderer()->activateShader(_shader);

                Matrix4 modelViewProj = projection * transform;
                std::vector<float> colorVector = { color.getR(), color.getG(), color.getB(), color.getA() };

                _shader->setVertexShaderConstant(0, sizeof(Matrix4), 1, modelViewProj.m);
                _shader->setPixelShaderConstant(0, vectorDataSize(colorVector), 1, colorVector.data());

                sharedEngine->getRenderer()->drawMeshBuffer(_meshBuffer);
            }
        }

        void TextDrawable::setText(const std::string& text)
        {
            _text = text;

            if (_text.empty())
            {
                _meshBuffer.reset();
            }
            else
            {
                updateMesh();
            }
        }

        void TextDrawable::setColor(const graphics::Color& color)
        {
            _color = color;

            updateMesh();
        }

        void TextDrawable::updateMesh()
        {
            std::vector<uint16_t> indices;
            std::vector<graphics::VertexPCT> vertices;

            _font.getVertices(_text, _color, _textAnchor, indices, vertices);

            _meshBuffer = sharedEngine->getRenderer()->createMeshBufferFromData(indices.data(), sizeof(uint16_t), static_cast<uint32_t>(indices.size()), false,
                                                                                vertices.data(), graphics::VertexPCT::ATTRIBUTES, static_cast<uint32_t>(vertices.size()), false);
            
            _boundingBox.reset();
            
            for (const graphics::VertexPCT& vertex : vertices)
            {
                _boundingBox.insertPoint(Vector2(vertex.position.x, vertex.position.y));
            }
        }
    } // namespace scene
} // namespace ouzel
