// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextRenderer.hpp"
#include "core/Engine.hpp"
#include "graphics/Renderer.hpp"
#include "scene/Camera.hpp"
#include "assets/Cache.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        TextRenderer::TextRenderer(const std::string& fontFile,
                                   bool initMipmaps,
                                   float initFontSize,
                                   const std::string& initText,
                                   Color initColor,
                                   const Vector2& initTextAnchor):
            Component(CLASS),
            text(initText),
            fontSize(initFontSize),
            textAnchor(initTextAnchor),
            color(initColor),
            mipmaps(initMipmaps)
        {
            shader = engine->getCache()->getShader(graphics::SHADER_TEXTURE);
            blendState = engine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            whitePixelTexture = engine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, graphics::Buffer::DYNAMIC);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, graphics::Buffer::DYNAMIC);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, vertexBuffer);

            font = engine->getCache()->getFont(fontFile, mipmaps);

            updateText();
        }

        void TextRenderer::setFont(const std::string& fontFile)
        {
            font = engine->getCache()->getFont(fontFile);

            updateText();
        }

        void TextRenderer::setTextAnchor(const Vector2& newTextAnchor)
        {
            textAnchor = newTextAnchor;

            updateText();
        }

        void TextRenderer::setFontSize(float newFontSize)
        {
            fontSize = newFontSize;

            updateText();
        }

        void TextRenderer::draw(const Matrix4& transformMatrix,
                                float opacity,
                                const Matrix4& renderViewProjection,
                                bool depthWrite,
                                bool depthTest,
                                bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            depthWrite,
                            depthTest,
                            wireframe);

            if (needsMeshUpdate)
            {
                indexBuffer->setData(indices.data(), static_cast<uint32_t>(getVectorSize(indices)));
                vertexBuffer->setData(vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)));

                needsMeshUpdate = false;
            }

            Matrix4 modelViewProj = renderViewProjection * transformMatrix;
            float colorVector[] = {color.normR(), color.normG(), color.normB(), color.normA() * opacity};

            std::vector<std::vector<float>> pixelShaderConstants(1);
            pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

            engine->getRenderer()->addSetCullModeCommad(graphics::Renderer::CullMode::NONE);

            engine->getRenderer()->addDrawCommand({wireframe ? whitePixelTexture : texture},
                                                  shader,
                                                  pixelShaderConstants,
                                                  vertexShaderConstants,
                                                  blendState,
                                                  meshBuffer,
                                                  static_cast<uint32_t>(indices.size()),
                                                  graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                  0,
                                                  depthWrite,
                                                  depthTest);
        }

        void TextRenderer::setText(const std::string& newText)
        {
            text = newText;

            updateText();
        }

        void TextRenderer::setColor(const Color& newColor)
        {
            color = newColor;
        }

        void TextRenderer::updateText()
        {
            boundingBox.reset();

            if (font)
            {
                font->getVertices(text, Color::WHITE, fontSize, textAnchor, indices, vertices, texture);
                needsMeshUpdate = true;

                for (const graphics::Vertex& vertex : vertices)
                {
                    boundingBox.insertPoint(Vector2(vertex.position.x, vertex.position.y));
                }
            }
            else
            {
                indices.clear();
                vertices.clear();
                texture.reset();
            }
        }
    } // namespace scene
} // namespace ouzel
