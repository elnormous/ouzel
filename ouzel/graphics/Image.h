// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "utils/Noncopyable.h"
#include "math/Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Image: public Noncopyable
        {
        public:
            Image();

            const Size2& getSize() const { return size; }
            const std::vector<uint8_t>& getData() const { return data; }

            virtual bool initFromFile(const std::string& newFilename);
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData);

        protected:
            std::string filename;
            Size2 size;

            std::vector<uint8_t> data;
        };
    } // namespace graphics
} // namespace ouzel
