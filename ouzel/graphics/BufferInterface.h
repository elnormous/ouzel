// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include "graphics/ResourceInterface.h"
#include "graphics/Buffer.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BufferInterface: public ResourceInterface
        {
            friend Renderer;
        public:
            virtual ~BufferInterface();

            virtual bool init(Buffer::Usage newUsage, uint32_t newFlags = 0);
            virtual bool init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags = 0);

            virtual bool setData(const std::vector<uint8_t>& newData);

            uint32_t getFlags() const { return flags; }
            Buffer::Usage getUsage() const { return usage; }
            uint32_t getSize() const { return static_cast<uint32_t>(data.size()); }

        protected:
            BufferInterface();

            std::vector<uint8_t> data;
            Buffer::Usage usage;
            uint32_t flags = 0;
        };
    } // namespace graphics
} // namespace ouzel
