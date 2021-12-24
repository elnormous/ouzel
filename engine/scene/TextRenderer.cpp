// Ouzel by Elviss Strazdins

#include "TextRenderer.hpp"
#include "Camera.hpp"
#include "../core/Engine.hpp"
#include "../graphics/Graphics.hpp"
#include "../assets/Cache.hpp"
#include "../utils/Utils.hpp"

namespace ouzel::scene
{
    TextRenderer::TextRenderer(const std::string& fontFile,
                               float initFontSize,
                               const std::string& initText,
                               math::Color initColor,
                               const math::Vector<float, 2>& initTextAnchor):
        shader{engine->getCache().getShader(shaderTexture)},
        blendState{engine->getCache().getBlendState(blendAlpha)},
        indexBuffer{engine->getGraphics(),
                    graphics::BufferType::index,
                    graphics::Flags::dynamic},
        vertexBuffer{engine->getGraphics(),
                     graphics::BufferType::vertex,
                     graphics::Flags::dynamic},
        text{initText},
        fontSize{initFontSize},
        textAnchor{initTextAnchor},
        color{initColor}
    {
        whitePixelTexture = engine->getCache().getTexture(textureWhitePixel);

        font = engine->getCache().getFont(fontFile);

        updateText();
    }

    TextRenderer::TextRenderer(const gui::Font& initFont,
                               float initFontSize,
                               const std::string& initText,
                               math::Color initColor,
                               const math::Vector<float, 2>& initTextAnchor):
        shader{engine->getCache().getShader(shaderTexture)},
        blendState{engine->getCache().getBlendState(blendAlpha)},
        indexBuffer{engine->getGraphics(),
                    graphics::BufferType::index,
                    graphics::Flags::dynamic},
        vertexBuffer{engine->getGraphics(),
                     graphics::BufferType::vertex,
                     graphics::Flags::dynamic},
        font{&initFont},
        text{initText},
        fontSize{initFontSize},
        textAnchor{initTextAnchor},
        color{initColor}
    {
        whitePixelTexture = engine->getCache().getTexture(textureWhitePixel);

        updateText();
    }

    void TextRenderer::setFont(const std::string& fontFile)
    {
        font = engine->getCache().getFont(fontFile);

        updateText();
    }

    void TextRenderer::setTextAnchor(const math::Vector<float, 2>& newTextAnchor)
    {
        textAnchor = newTextAnchor;

        updateText();
    }

    void TextRenderer::setFontSize(float newFontSize)
    {
        fontSize = newFontSize;

        updateText();
    }

    void TextRenderer::draw(const math::Matrix<float, 4>& transformMatrix,
                            float opacity,
                            const math::Matrix<float, 4>& renderViewProjection,
                            bool wireframe)
    {
        Component::draw(transformMatrix,
                        opacity,
                        renderViewProjection,
                        wireframe);

        if (needsMeshUpdate)
        {
            indexBuffer.setData(indices.data(), static_cast<std::uint32_t>(getVectorSize(indices)));
            vertexBuffer.setData(vertices.data(), static_cast<std::uint32_t>(getVectorSize(vertices)));

            needsMeshUpdate = false;
        }

        const auto modelViewProj = renderViewProjection * transformMatrix;
        const auto colorVector = {
            color.normR(),
            color.normG(),
            color.normB(),
            color.normA() * opacity
        };

        std::vector<std::vector<float>> fragmentShaderConstants(1);
        fragmentShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

        std::vector<std::vector<float>> vertexShaderConstants(1);
        vertexShaderConstants[0] = {std::begin(modelViewProj.m.v), std::end(modelViewProj.m.v)};

        engine->getGraphics().setPipelineState(blendState->getResource(),
                                               shader->getResource(),
                                               graphics::CullMode::none,
                                               wireframe ? graphics::FillMode::wireframe : graphics::FillMode::solid);
        engine->getGraphics().setShaderConstants(fragmentShaderConstants,
                                                 vertexShaderConstants);
        engine->getGraphics().setTextures({wireframe ? whitePixelTexture->getResource() : texture ? texture->getResource() : 0U});
        engine->getGraphics().draw(indexBuffer.getResource(),
                                   static_cast<std::uint32_t>(indices.size()),
                                   sizeof(std::uint16_t),
                                   vertexBuffer.getResource(),
                                   graphics::DrawMode::triangleList,
                                   0);
    }

    void TextRenderer::setText(const std::string& newText)
    {
        text = newText;

        updateText();
    }

    void TextRenderer::updateText()
    {
        reset(boundingBox);

        if (font)
        {
            std::tie(indices, vertices, texture) = font->getRenderData(text, math::whiteColor, fontSize, textAnchor);
            needsMeshUpdate = true;

            for (const auto& vertex : vertices)
                insertPoint(boundingBox, vertex.position);
        }
        else
        {
            indices.clear();
            vertices.clear();
            texture.reset();
        }
    }
}
