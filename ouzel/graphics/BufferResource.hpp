// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include "graphics/Resource.hpp"
#include "graphics/Buffer.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BufferResource: public Resource
        {
            friend Renderer;
        public:
            virtual ~BufferResource();

            virtual bool init(Buffer::Usage newUsage, uint32_t newFlags = 0, uint32_t newSize = 0);
            virtual bool init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags = 0);

            virtual bool setData(const std::vector<uint8_t>& newData);

            uint32_t getFlags() const { return flags; }
            Buffer::Usage getUsage() const { return usage; }
            uint32_t getSize() const { return static_cast<uint32_t>(data.size()); }

        protected:
            BufferResource();

            std::vector<uint8_t> data;
            Buffer::Usage usage;
            uint32_t flags = 0;
        };
    } // namespace graphics
} // namespace ouzel
