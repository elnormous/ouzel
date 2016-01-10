// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "BMFont.h"
#include "Engine.h"
#include "FileSystem.h"
#include "Vertex.h"
#include "Utils.h"

namespace ouzel
{
    BMFont::BMFont()
    {
        
    }
    
    BMFont::~BMFont()
    {
        
    }
    
    bool BMFont::parseFont(std::string const& filename)
    {
        std::ifstream stream(Engine::getInstance()->getFileSystem()->getPath(filename));
        std::string line;
        std::string read, key, value;
        std::size_t i;
        
        if (!stream)
        {
            log("Failed to open font file");
            return false;
        }

        KerningInfo k;
        CharDescriptor c;

        while (!stream.eof())
        {
            std::getline(stream, line);
            
            std::stringstream lineStream;
            lineStream << line;
            
            lineStream >> read;
            
            if (read == "page")
            {
                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);
                    
                    //assign the correct value
                    converter << value;
                    if (key == "file")
                    {
                        // trim quotes
                        if (value.length() && value[0] == '"' && value[value.length() - 1] == '"')
                        {
                            value = value.substr(1, value.length() - 2);
                        }
                        
                        _texture = Engine::getInstance()->getRenderer()->getTexture(value);
                    }
                }
            }
            else if (read == "common")
            {
                //this holds common data
                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //assign the correct value
                    converter << value;
                    if (key == "lineHeight") converter >> lineHeight;
                    else if (key == "base") converter >> base;
                    else if (key == "scaleW") converter >> width;
                    else if (key == "scaleH") converter >> height;
                    else if (key == "pages") converter >> pages;
                    else if (key == "outline") converter >> outline;
                }
            }
            else if (read == "char")
            {
                //This is data for each specific character.
                int charId = 0;
        
                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //Assign the correct value
                    converter << value;
                    if (key == "id") converter >> charId;
                    else if (key == "x") converter >> c.x;
                    else if (key == "y") converter >> c.y;
                    else if (key == "width") converter >> c.width;
                    else if (key == "height") converter >> c.height;
                    else if (key == "xoffset") converter >> c.xOffset;
                    else if (key == "yoffset") converter >> c.yOffset;
                    else if (key == "xadvance") converter >> c.xAdvance;
                    else if (key == "page") converter >> c.page;
                }
                
                chars.insert(std::map<int,CharDescriptor>::value_type(charId, c));
            }
            else if (read == "kernings")
            {
                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //assign the correct value
                    converter << value;
                    if (key == "count") converter >> kernCount;
                }
            }
            else if (read == "kerning")
            {
                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //assign the correct value
                    converter << value;
                    if (key == "first") converter >> k.first;
                    else if (key == "second") converter >> k.second;
                    else if (key == "amount") converter >> k.amount;
                }
                kern.push_back(k);
            }
        }

        stream.close();
        
        return true;
    }

    int32_t BMFont::getKerningPair(int32_t first, int32_t second)
    {
        // TODO: keep kerning information in hashmap
        
        //Kearning is checked for every character processed. This is expensive in terms of processing time.
        for (int j = 0; j < kernCount;  j++)
        {
            if (kern[j].first == first && kern[j].second == second)
            {
                return kern[j].amount;
            }
        }

        return 0;
    }

    float BMFont::getStringWidth(std::string const& text)
    {
        float total = 0;
        CharDescriptor* f;

        for (int i = 0; i != text.length(); i++)
        {
            std::map<int32_t, CharDescriptor>::iterator iter = chars.find(text[i]);
            
            if (iter == chars.end())
            {
                continue;
            }
            
            f = &iter->second;
            total += f->xAdvance;
        }

        return total;
    }

    bool BMFont::loadFont(std::string const& filename)
    {
        if (!parseFont(filename))
        {
            log("Failed to parse font %s",filename.c_str());
            return false;
        }
        
        if (!_texture)
        {
            return false;
        }
        
        kernCount = static_cast<uint16_t>(kern.size());

        return true;
    }

    std::shared_ptr<MeshBuffer> BMFont::createMeshBuffer(std::string const& text)
    {
        uint32_t flen;
        
        CharDescriptor  *f;
        
        //Set type of blending to use with this font.
        //setBlendMode(fblend);
        
        float x = 0.0f;
        float y = lineHeight / 2.0f;
        
        flen = static_cast<uint32_t>(text.length());
        
        std::vector<VertexPCT> vertices;
        std::vector<uint16_t> indices;
        
        Vector2 textCoords[4];
        
        for (uint32_t i = 0; i != flen; ++i)
        {
            std::map<int32_t, CharDescriptor>::iterator iter = chars.find(text[i]);
            
            if (iter == chars.end())
            {
                continue;
            }
            else
            {
                f = &iter->second;
            }
            
            uint16_t startIndex = vertices.size();
            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 2);
            
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 3);
            indices.push_back(startIndex + 2);
            
            Vector2 leftTop(f->x / static_cast<float>(width),
                            f->y / static_cast<float>(height));
            
            Vector2 rightBottom((f->x + f->width) / static_cast<float>(width),
                                (f->y + f->height) / static_cast<float>(height));

            textCoords[0] = Vector2(leftTop.x, leftTop.y);
            textCoords[1] = Vector2(rightBottom.x, leftTop.y);
            textCoords[2] = Vector2(leftTop.x, rightBottom.y);
            textCoords[3] = Vector2(rightBottom.x, rightBottom.y);
            
            vertices.push_back(VertexPCT(Vector3(x + f->xOffset, y - f->yOffset, 0.0f),
                                         _color,
                                         textCoords[0]));
            
            vertices.push_back(VertexPCT(Vector3(x + f->xOffset + f->width, y - f->yOffset, 0.0f),
                                         _color,
                                         textCoords[1]));
            
            vertices.push_back(VertexPCT(Vector3(x + f->xOffset, y - f->yOffset - f->height, 0.0f),
                                         _color,
                                         textCoords[2]));
            
            vertices.push_back(VertexPCT(Vector3(x + f->xOffset + f->width, y - f->yOffset - f->height, 0.0f),
                                         _color,
                                         textCoords[3]));

            // Only check kerning if there is greater then 1 character and
            // if the check character is 1 less then the end of the string.
            if (flen > 1 && i < flen)
            {
                x += getKerningPair(text[i], text[i+1]);
            }
              
            x +=  f->xAdvance;
        }
        
        float width = x;
        
        for (VertexPCT& vertex : vertices)
        {
            vertex.position.x -= width / 2.0f;
        }
        
        std::shared_ptr<MeshBuffer> meshBuffer =
            Engine::getInstance()->getRenderer()->createMeshBuffer(indices.data(), sizeof(uint16_t), static_cast<uint32_t>(indices.size()), false,
                                                                   vertices.data(), sizeof(VertexPCT), static_cast<uint32_t>(vertices.size()), false,
                                                                   VertexPCT::ATTRIBUTES);
        
        return meshBuffer;
    }

    void BMFont::printCenter(float y, std::string const& text)
    {
        /*int x = 0;
        CharDescriptor  *f;		 
        
        int window_width = MyWindow.width;

        int len = strlen(string);

        for (int i = 0; i != len; ++i)
        {
            f = &chars[string[i]];

            if (len > 1 && i < len)
            {
                x += getKerningPair(string[i], string[i+1]);
            }
            x +=  f->xAdvance;
        }

        print((MyWindow.width/2.0f) - (x/2.0f), y, string);*/
    }
}
