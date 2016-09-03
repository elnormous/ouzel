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
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            whitePixelTexture = sharedEngine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);
        }

        TextDrawable::TextDrawable(const std::string& fontFile, const std::string& pText, const Vector2& pTextAnchor)
        {
            meshBuffer = sharedEngine->getRenderer()->createMeshBuffer();
            meshBuffer->init();
            meshBuffer->setIndexSize(sizeof(uint16_t));
            meshBuffer->setVertexAttributes(graphics::VertexPCT::ATTRIBUTES);

            init(fontFile, pText, pTextAnchor);
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
