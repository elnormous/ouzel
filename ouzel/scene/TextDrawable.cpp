// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextDrawable.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "graphics/MeshBuffer.h"
#include "core/Cache.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace scene
    {
        TextDrawable::TextDrawable()
        {
        }
        
        TextDrawable::TextDrawable(const std::string& font, const std::string& newText, const Vector2& textAnchor)
        {
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            init(font, newText, textAnchor);
        }

        TextDrawable::~TextDrawable()
        {

        }

        bool TextDrawable::init(const std::string& fontFile, const std::string& newText, const Vector2& newTextAnchor)
        {
            font.loadFont(fontFile);
            textAnchor = newTextAnchor;

            texture = font.getTexture();

            if (!texture)
            {
                return false;
            }

            setText(newText);

            return true;
        }

        void TextDrawable::draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& drawColor, const NodePtr& currentNode)
        {
            Drawable::draw(projectionMatrix, transformMatrix, drawColor, currentNode);

            if (shader && texture && meshBuffer)
            {
                sharedEngine->getRenderer()->activateTexture(texture, 0);
                sharedEngine->getRenderer()->activateShader(shader);

                Matrix4 modelViewProj = projectionMatrix * transformMatrix;
                float colorVector[] = { drawColor.getR(), drawColor.getG(), drawColor.getB(), drawColor.getA() };

                shader->setVertexShaderConstant(0, sizeof(Matrix4), 1, modelViewProj.m);
                shader->setPixelShaderConstant(0, sizeof(colorVector), 1, colorVector);

                sharedEngine->getRenderer()->drawMeshBuffer(meshBuffer);
            }
        }

        void TextDrawable::setText(const std::string& newText)
        {
            text = newText;

            if (text.empty())
            {
                meshBuffer.reset();
            }
            else
            {
                updateMesh();
            }
        }

        void TextDrawable::setColor(const graphics::Color& newColor)
        {
            color = newColor;

            updateMesh();
        }

        void TextDrawable::updateMesh()
        {
            std::vector<uint16_t> indices;
            std::vector<graphics::VertexPCT> vertices;

            font.getVertices(text, color, textAnchor, indices, vertices);

            meshBuffer = sharedEngine->getRenderer()->createMeshBuffer();

            meshBuffer->initFromData(indices.data(), sizeof(uint16_t), static_cast<uint32_t>(indices.size()), false,
                                     vertices.data(), graphics::VertexPCT::ATTRIBUTES, static_cast<uint32_t>(vertices.size()), false);

            boundingBox.reset();

            for (const graphics::VertexPCT& vertex : vertices)
            {
                boundingBox.insertPoint(Vector2(vertex.position.x, vertex.position.y));
            }
        }
    } // namespace scene
} // namespace ouzel
