// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
                                   float initFontSize,
                                   const std::string& initText,
                                   Color initColor,
                                   const Vector2F& initTextAnchor):
            indexBuffer(*engine->getRenderer(),
                        graphics::BufferType::Index,
                        graphics::Flags::Dynamic),
            vertexBuffer(*engine->getRenderer(),
                         graphics::BufferType::Vertex,
                         graphics::Flags::Dynamic),
            text(initText),
            fontSize(initFontSize),
            textAnchor(initTextAnchor),
            color(initColor)
        {
            shader = engine->getCache().getShader(SHADER_TEXTURE);
            blendState = engine->getCache().getBlendState(BLEND_ALPHA);
            whitePixelTexture = engine->getCache().getTexture(TEXTURE_WHITE_PIXEL);

            font = engine->getCache().getFont(fontFile);

            updateText();
        }

        void TextRenderer::setFont(const std::string& fontFile)
        {
            font = engine->getCache().getFont(fontFile);

            updateText();
        }

        void TextRenderer::setTextAnchor(const Vector2F& newTextAnchor)
        {
            textAnchor = newTextAnchor;

            updateText();
        }

        void TextRenderer::setFontSize(float newFontSize)
        {
            fontSize = newFontSize;

            updateText();
        }

        void TextRenderer::draw(const Matrix4F& transformMatrix,
                                float opacity,
                                const Matrix4F& renderViewProjection,
                                bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            wireframe);

            if (needsMeshUpdate)
            {
                indexBuffer.setData(indices.data(), static_cast<uint32_t>(getVectorSize(indices)));
                vertexBuffer.setData(vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)));

                needsMeshUpdate = false;
            }

            const Matrix4F modelViewProj = renderViewProjection * transformMatrix;
            const float colorVector[] = {color.normR(), color.normG(), color.normB(), color.normA() * opacity};

            std::vector<std::vector<float>> fragmentShaderConstants(1);
            fragmentShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

            engine->getRenderer()->setPipelineState(blendState->getResource(),
                                                    shader->getResource(),
                                                    graphics::CullMode::NoCull,
                                                    wireframe ? graphics::FillMode::Wireframe : graphics::FillMode::Solid);
            engine->getRenderer()->setShaderConstants(fragmentShaderConstants,
                                                      vertexShaderConstants);
            engine->getRenderer()->setTextures({wireframe ? whitePixelTexture->getResource() : texture->getResource()});
            engine->getRenderer()->draw(indexBuffer.getResource(),
                                        static_cast<uint32_t>(indices.size()),
                                        sizeof(uint16_t),
                                        vertexBuffer.getResource(),
                                        graphics::DrawMode::TriangleList,
                                        0);
        }

        void TextRenderer::setText(const std::string& newText)
        {
            text = newText;

            updateText();
        }

        void TextRenderer::setColor(Color newColor)
        {
            color = newColor;
        }

        void TextRenderer::updateText()
        {
            boundingBox.reset();

            if (font)
            {
                font->getVertices(text, Color::white(), fontSize, textAnchor, indices, vertices, texture);
                needsMeshUpdate = true;

                for (const graphics::Vertex& vertex : vertices)
                    boundingBox.insertPoint(vertex.position);
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
