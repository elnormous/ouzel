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
        TextDrawable::TextDrawable(const std::string& fontFile, const std::string& pText, const Vector2& pTextAnchor):
            font(fontFile)
        {
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            whitePixelTexture = sharedEngine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);

            meshBuffer = sharedEngine->getRenderer()->createMeshBuffer();
            meshBuffer->init();
            meshBuffer->setIndexSize(sizeof(uint16_t));
            meshBuffer->setVertexAttributes(graphics::VertexPCT::ATTRIBUTES);

            textAnchor = pTextAnchor;

            texture = font.getTexture();

            setText(pText);
        }

        TextDrawable::~TextDrawable()
        {
        }

        void TextDrawable::setFont(const std::string& fontFile)
        {
            font = BMFont(fontFile);
            texture = font.getTexture();

            setText(text);
        }

        void TextDrawable::setTextAnchor(const Vector2& newTextAnchor)
        {
            textAnchor = newTextAnchor;

            setText(text);
        }

        void TextDrawable::draw(const Matrix4& projectionMatrix,
                                const Matrix4& transformMatrix,
                                const graphics::Color& drawColor,
                                const graphics::RenderTargetPtr& renderTarget)
        {
            Component::draw(projectionMatrix, transformMatrix, drawColor, renderTarget);

            if (needsMeshUpdate)
            {
                meshBuffer->setIndices(indices.data(), static_cast<uint32_t>(indices.size()));
                meshBuffer->setVertices(vertices.data(), static_cast<uint32_t>(vertices.size()));

                needsMeshUpdate = false;
            }

            Matrix4 modelViewProj = projectionMatrix * transformMatrix;
            float colorVector[] = { drawColor.getR(), drawColor.getG(), drawColor.getB(), drawColor.getA() };

            std::vector<std::vector<float>> pixelShaderConstants(1);
            pixelShaderConstants[0] = { std::begin(colorVector), std::end(colorVector) };

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = { std::begin(modelViewProj.m), std::end(modelViewProj.m) };

            sharedEngine->getRenderer()->addDrawCommand({ texture },
                                                        shader,
                                                        pixelShaderConstants,
                                                        vertexShaderConstants,
                                                        blendState,
                                                        meshBuffer,
                                                        static_cast<uint32_t>(indices.size()),
                                                        graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                        0,
                                                        renderTarget);
        }

        void TextDrawable::drawWireframe(const Matrix4& projectionMatrix,
                                         const Matrix4& transformMatrix,
                                         const graphics::Color& drawColor,
                                         const graphics::RenderTargetPtr& renderTarget)
        {
            Component::drawWireframe(projectionMatrix, transformMatrix, drawColor, renderTarget);

            Matrix4 modelViewProj = projectionMatrix * transformMatrix;
            float colorVector[] = { drawColor.getR(), drawColor.getG(), drawColor.getB(), drawColor.getA() };

            std::vector<std::vector<float>> pixelShaderConstants(1);
            pixelShaderConstants[0] = { std::begin(colorVector), std::end(colorVector) };

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = { std::begin(modelViewProj.m), std::end(modelViewProj.m) };

            sharedEngine->getRenderer()->addDrawCommand({ whitePixelTexture },
                                                        shader,
                                                        pixelShaderConstants,
                                                        vertexShaderConstants,
                                                        blendState,
                                                        meshBuffer,
                                                        static_cast<uint32_t>(indices.size()),
                                                        graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                        0,
                                                        renderTarget);
        }

        void TextDrawable::setText(const std::string& newText)
        {
            text = newText;

            font.getVertices(text, color, textAnchor, indices, vertices);
            needsMeshUpdate = true;

            updateBounds();
        }

        void TextDrawable::setColor(const graphics::Color& newColor)
        {
            color = newColor;

            font.getVertices(text, color, textAnchor, indices, vertices);
            needsMeshUpdate = true;

            updateBounds();
        }

        void TextDrawable::updateBounds()
        {
            boundingBox.reset();

            for (const graphics::VertexPCT& vertex : vertices)
            {
                boundingBox.insertPoint(Vector2(vertex.position.x, vertex.position.y));
            }

        }
    } // namespace scene
} // namespace ouzel
