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
                               Color initColor,
                               const Vector<float, 2>& initTextAnchor):
        shader{engine->getCache().getShader(shaderTexture)},
        blendState{engine->getCache().getBlendState(blendAlpha)},
        indexBuffer{*engine->getGraphics(),
                    graphics::BufferType::index,
                    graphics::Flags::dynamic},
        vertexBuffer{*engine->getGraphics(),
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
                               Color initColor,
                               const Vector<float, 2>& initTextAnchor):
        shader{engine->getCache().getShader(shaderTexture)},
        blendState{engine->getCache().getBlendState(blendAlpha)},
        indexBuffer{*engine->getGraphics(),
                    graphics::BufferType::index,
                    graphics::Flags::dynamic},
        vertexBuffer{*engine->getGraphics(),
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

    void TextRenderer::setTextAnchor(const Vector<float, 2>& newTextAnchor)
    {
        textAnchor = newTextAnchor;

        updateText();
    }

    void TextRenderer::setFontSize(float newFontSize)
    {
        fontSize = newFontSize;

        updateText();
    }

    void TextRenderer::draw(const Matrix<float, 4>& transformMatrix,
                            float opacity,
                            const Matrix<float, 4>& renderViewProjection,
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
        vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

        engine->getGraphics()->setPipelineState(blendState->getResource(),
                                                shader->getResource(),
                                                graphics::CullMode::none,
                                                wireframe ? graphics::FillMode::wireframe : graphics::FillMode::solid);
        engine->getGraphics()->setShaderConstants(fragmentShaderConstants,
                                                  vertexShaderConstants);
        engine->getGraphics()->setTextures({wireframe ? whitePixelTexture->getResource() : texture ? texture->getResource() : 0U});
        engine->getGraphics()->draw(indexBuffer.getResource(),
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

    void TextRenderer::setColor(Color newColor)
    {
        color = newColor;
    }

    void TextRenderer::updateText()
    {
        boundingBox.reset();

        if (font)
        {
            std::tie(indices, vertices, texture) = font->getRenderData(text, Color::white(), fontSize, textAnchor);
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
}
