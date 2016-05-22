// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "Noncopyable.h"
#include "Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Image: public Noncopyable
        {
        public:
            Image();
            virtual ~Image();

            const Size2& getSize() const { return size; }
            const void* getData() const { return data; }

            virtual bool initFromFile(const std::string& newFilename);
            virtual bool initFromData(const std::vector<uint8_t>& newData);

        protected:
            std::string filename;
            Size2 size;

            void* data = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
