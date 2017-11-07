// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "XML.hpp"
#include "core/Engine.hpp"
#include "Log.hpp"
#include "Utils.hpp"

namespace ouzel
{
    namespace xml
    {
        Node::Node()
        {
        }

        Data::Data()
        {
        }

        Data::Data(const std::string& filename)
        {
            init(filename);
        }

        Data::Data(const std::vector<uint8_t>& data)
        {
            init(data);
        }

        bool Data::init(const std::string& filename)
        {
            std::vector<uint8_t> data;

            if (!engine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            return init(data);
        }

        bool Data::init(const std::vector<uint8_t>& data)
        {
            std::vector<uint32_t> utf32;

            // BOM
            if (data.size() >= 3 &&
                data[0] == 0xEF &&
                data[1] == 0xBB &&
                data[2] == 0xBF)
            {
                bom = true;
                std::vector<uint8_t> newData(data.begin() + 3, data.end());
                utf32 = utf8to32(newData);
            }
            else
            {
                bom = false;
                utf32 = utf8to32(data);
            }

            return true;
        }
    } // namespace xml
}
