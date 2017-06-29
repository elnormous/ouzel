// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextRenderer.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "scene/Camera.h"
#include "core/Cache.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace scene
    {
        TextRenderer::TextRenderer(const std::string& fontFile,
                                   bool aMipmaps,
                                   const std::string& aText,
                                   Color aColor,
                                   const Vector2& aTextAnchor):
            Component(Component::TEXT_RENDERER),
            text(aText),
            textAnchor(aTextAnchor),
            color(aColor),
            mipmaps(aMipmaps)
        {
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            whitePixelTexture = sharedEngine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, graphics::VertexPCT::ATTRIBUTES, vertexBuffer);

            font = sharedEngine->getCache()->getBMFont(fontFile);
            texture = sharedEngine->getCache()->getTexture(font.getTexture(), false, mipmaps);

            updateText();
        }

        void TextRenderer::setFont(const std::string& fontFile)
        {
            font = sharedEngine->getCache()->getBMFont(fontFile);
            texture = sharedEngine->getCache()->getTexture(font.getTexture(), false, mipmaps);

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
                                                        scissorRectangle);
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
            font.getVertices(text, color, textAnchor, textScale, indices, vertices);
            needsMeshUpdate = true;

            boundingBox.reset();

            for (const graphics::VertexPCT& vertex : vertices)
            {
                boundingBox.insertPoint(Vector2(vertex.position.v[0], vertex.position.v[1]));
            }

        }
    } // namespace scene
} // namespace ouzel
