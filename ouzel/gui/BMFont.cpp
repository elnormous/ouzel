// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include "BMFont.hpp"
#include "core/Engine.hpp"
#include "files/FileSystem.hpp"
#include "core/Cache.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    BMFont::BMFont()
    {
    }

    BMFont::BMFont(const std::string& filename)
    {
        if (!parseFont(filename))
        {
            Log(Log::Level::ERR) << "Failed to parse font " << filename;
        }

        kernCount = static_cast<uint16_t>(kern.size());
    }

    bool BMFont::parseFont(const std::string& filename)
    {
        std::vector<uint8_t> data;
        if (!sharedEngine->getFileSystem()->readFile(filename, data))
        {
            return false;
        }

        std::stringstream stream;
        std::copy(data.begin(), data.end(), std::ostream_iterator<uint8_t>(stream));

        std::string line;
        std::string read, key, value;
        std::size_t i;

        if (!stream)
        {
            Log(Log::Level::ERR) << "Failed to open font file " << filename;
            return false;
        }

        int16_t k;
        uint32_t first, second;
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

                        texture = value;
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
                int32_t charId = 0;

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

                chars.insert(std::unordered_map<int32_t, CharDescriptor>::value_type(charId, c));
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
                k = 0;
                first = second = 0;
                while (!lineStream.eof())
                {
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //assign the correct value
                    std::stringstream converter;
                    converter << value;
                    if (key == "first") converter >> first;
                    else if (key == "second") converter >> second;
                    else if (key == "amount") converter >> k;
                }
                kern[std::make_pair(first, second)] = k;
            }
        }

        return true;
    }
}
