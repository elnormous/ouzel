// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"
#include "Label.h"
#include "Engine.h"
#include "Renderer.h"
#include "Layer.h"
#include "Camera.h"
#include "BMFont.h"
#include "Cache.h"

namespace ouzel
{
    namespace gui
    {
        std::shared_ptr<Label> Label::create(const std::string& font, const std::string& text, const Vector2& textAnchor)
        {
            std::shared_ptr<Label> result = std::make_shared<Label>();

            if (!result->init(font, text, textAnchor))
            {
                result.reset();
            }

            return result;
        }

        Label::Label()
        {
            _shader = Engine::getInstance()->getCache()->getShader(video::SHADER_TEXTURE);

    #ifdef OUZEL_PLATFORM_WINDOWS
            _uniModelViewProj = 0;
    #else
            _uniModelViewProj = _shader->getVertexShaderConstantId("modelViewProj");
    #endif
        }

        Label::~Label()
        {

        }

        bool Label::init(const std::string& font, const std::string& text, const Vector2& textAnchor)
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

        void Label::draw()
        {
            Widget::draw();

            scene::LayerPtr layer = _layer.lock();

            if (_shader && _texture && layer && _meshBuffer)
            {
                Engine::getInstance()->getRenderer()->activateTexture(_texture, 0);
                Engine::getInstance()->getRenderer()->activateShader(_shader);

                Matrix4 modelViewProj = layer->getCamera()->getViewProjection() * _transform;

                _shader->setVertexShaderConstant(_uniModelViewProj, { modelViewProj });

                Engine::getInstance()->getRenderer()->drawMeshBuffer(_meshBuffer);
            }
        }

        void Label::setText(const std::string& text)
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

        void Label::setColor(const video::Color& color)
        {
            _color = color;

            updateMesh();
        }

        void Label::updateMesh()
        {
            std::vector<uint16_t> indices;
            std::vector<video::VertexPCT> vertices;

            _font.getVertices(_text, _color, _textAnchor, indices, vertices);

            _meshBuffer = Engine::getInstance()->getRenderer()->createMeshBuffer(indices.data(), sizeof(uint16_t), static_cast<uint32_t>(indices.size()), false,
                                                                                 vertices.data(), sizeof(video::VertexPCT), static_cast<uint32_t>(vertices.size()), false,
                                                                                 video::VertexPCT::ATTRIBUTES);

            _boundingBox.reset();

            for (const video::VertexPCT& vertex : vertices)
            {
                _boundingBox.insertPoint(Vector2(vertex.position.x, vertex.position.y));
            }
        }
    } // namespace gui
} // namespace ouzel
