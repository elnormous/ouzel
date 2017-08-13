// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextRenderer.hpp"
#include "core/Engine.hpp"
#include "graphics/Renderer.hpp"
#include "scene/Camera.hpp"
#include "core/Cache.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        TextRenderer::TextRenderer(const std::string& fontFile,
                                   bool aMipmaps,
                                   uint16_t pt,
                                   const std::string& aText,
                                   Color aColor,
                                   const Vector2& aTextAnchor):
            Component(TYPE),
            text(aText),
            textAnchor(aTextAnchor),
            color(aColor),
            mipmaps(aMipmaps)
        {
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            whitePixelTexture = sharedEngine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, graphics::Buffer::DYNAMIC);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, graphics::Buffer::DYNAMIC);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, graphics::VertexPCT::ATTRIBUTES, vertexBuffer);

            if (pt == 0)
            {
                font = sharedEngine->getCache()->getFont(fontFile, 0, mipmaps);
            }
            else
            {
                font = sharedEngine->getCache()->getFont(fontFile, pt, mipmaps);
            }

            updateText();
        }

        void TextRenderer::setFont(const std::string& fontFile, uint16_t pt)
        {
            if (pt == 0)
            {
                font = sharedEngine->getCache()->getFont(fontFile);
            }
            else
            {
                font = sharedEngine->getCache()->getFont(fontFile, pt);
            }

            updateText();
        }

        void TextRenderer::setTextAnchor(const Vector2& newTextAnchor)
        {
            textAnchor = newTextAnchor;

            updateText();
        }

        void TextRenderer::setTextScale(const Vector2& newTextScale)
        {
            textScale = newTextScale;

            updateText();
        }

        void TextRenderer::draw(const Matrix4& transformMatrix,
                                const Color& drawColor,
                                const Matrix4& renderViewProjection,
                                const std::shared_ptr<graphics::Texture>& renderTarget,
                                const Rectangle& renderViewport,
                                bool depthWrite,
                                bool depthTest,
                                bool wireframe,
                                bool scissorTest,
                                const Rectangle& scissorRectangle)
        {
            Component::draw(transformMatrix,
                            drawColor,
                            renderViewProjection,
                            renderTarget,
                            renderViewport,
                            depthWrite,
                            depthTest,
                            wireframe,
                            scissorTest,
                            scissorRectangle);

            if (needsMeshUpdate)
            {
                indexBuffer->setData(indices.data(), static_cast<uint32_t>(getVectorSize(indices)));
                vertexBuffer->setData(vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)));

                needsMeshUpdate = false;
            }

            Matrix4 modelViewProj = renderViewProjection * transformMatrix;
            float colorVector[] = {drawColor.normR(), drawColor.normG(), drawColor.normB(), drawColor.normA()};

            std::vector<std::vector<float>> pixelShaderConstants(1);
            pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

            sharedEngine->getRenderer()->addDrawCommand({wireframe ? whitePixelTexture : texture},
                                                        shader,
                                                        pixelShaderConstants,
                                                        vertexShaderConstants,
                                                        blendState,
                                                        meshBuffer,
                                                        static_cast<uint32_t>(indices.size()),
                                                        graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                        0,
                                                        renderTarget,
                                                        renderViewport,
                                                        depthWrite,
                                                        depthTest,
                                                        wireframe,
                                                        scissorTest,
                                                        scissorRectangle,
                                                        graphics::Renderer::CullMode::NONE);
        }

        void TextRenderer::setText(const std::string& newText)
        {
            text = newText;

            updateText();
        }

        void TextRenderer::setColor(const Color& newColor)
        {
            color = newColor;

            updateText();
        }

        void TextRenderer::updateText()
        {
            font->getVertices(text, color, textAnchor, textScale, indices, vertices, texture);
            needsMeshUpdate = true;

            boundingBox.reset();

            for (const graphics::VertexPCT& vertex : vertices)
            {
                boundingBox.insertPoint(Vector2(vertex.position.v[0], vertex.position.v[1]));
            }

        }
    } // namespace scene
} // namespace ouzel
