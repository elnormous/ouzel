// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Font.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    void Font::getVertices(const std::string& text,
                             const Color& color,
                             const Vector2& anchor,
                             const Vector2& scale,
                             std::vector<uint16_t>& indices,
                             std::vector<graphics::VertexPCT>& vertices)
    {
        Vector2 position;

        std::vector<uint32_t> utf32Text = utf8to32(text);

        indices.clear();
        vertices.clear();

        indices.reserve(utf32Text.size() * 6);
        vertices.reserve(utf32Text.size() * 4);

        Vector2 textCoords[4];

        size_t firstChar = 0;

        for (auto i = utf32Text.begin(); i != utf32Text.end(); ++i)
        {
            std::unordered_map<uint32_t, CharDescriptor>::iterator iter = chars.find(*i);

            if (iter != chars.end())
            {
                const CharDescriptor& f = iter->second;

                uint16_t startIndex = static_cast<uint16_t>(vertices.size());
                indices.push_back(startIndex + 0);
                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 2);

                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 3);
                indices.push_back(startIndex + 2);

                Vector2 leftTop(f.x / static_cast<float>(width),
                                f.y / static_cast<float>(height));

                Vector2 rightBottom((f.x + f.width) / static_cast<float>(width),
                                    (f.y + f.height) / static_cast<float>(height));

                textCoords[0] = Vector2(leftTop.v[0], rightBottom.v[1]);
                textCoords[1] = Vector2(rightBottom.v[0], rightBottom.v[1]);
                textCoords[2] = Vector2(leftTop.v[0], leftTop.v[1]);
                textCoords[3] = Vector2(rightBottom.v[0], leftTop.v[1]);

                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset, -position.v[1] - f.yOffset - f.height, 0.0f),
                                                       color, textCoords[0]));
                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset - f.height, 0.0f),
                                                       color, textCoords[1]));
                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset, -position.v[1] - f.yOffset, 0.0f),
                                                       color, textCoords[2]));
                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset, 0.0f),
                                                       color, textCoords[3]));

                if ((i + 1) != utf32Text.end())
                {
                    position.v[0] += static_cast<float>(getKerningPair(*i, *(i + 1)));
                }

                position.v[0] += f.xAdvance;
            }

            if (*i == static_cast<uint32_t>('\n') || // line feed
                (i + 1) == utf32Text.end()) // end of string
            {
                float lineWidth = position.v[0];
                position.v[0] = 0.0f;
                position.v[1] += lineHeight;

                for (size_t c = firstChar; c < vertices.size(); ++c)
                {
                    vertices[c].position.v[0] -= lineWidth * anchor.v[0];
                }

                firstChar = vertices.size();
            }
        }

        float textHeight = position.v[1];

        for (size_t c = 0; c < vertices.size(); ++c)
        {
            vertices[c].position.v[1] += textHeight * (1.0f - anchor.v[1]);
            
            vertices[c].position.v[0] *= scale.v[0];
            vertices[c].position.v[1] *= scale.v[1];
        }
    }

    int16_t Font::getKerningPair(uint32_t first, uint32_t second)
    {
        auto i = kern.find(std::make_pair(first, second));

        if (i != kern.end())
        {
            return i->second;
        }

        return 0;
    }

    float Font::getStringWidth(const std::string& text)
    {
        float total = 0.0f;

        std::vector<uint32_t> utf32Text = utf8to32(text);

        for (auto i = utf32Text.begin(); i != utf32Text.end(); ++i)
        {
            std::unordered_map<uint32_t, CharDescriptor>::iterator iter = chars.find(*i);

            if (iter != chars.end())
            {
                const CharDescriptor& f = iter->second;
                total += f.xAdvance;
            }
        }
        
        return total;
    }
}
