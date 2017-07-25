// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/Buffer.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BufferResource: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~BufferResource();

            virtual bool init(Buffer::Usage newUsage, bool newDynamic = true);
            virtual bool init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, bool newDynamic);

            virtual bool setData(const std::vector<uint8_t>& newData);

            bool isDynamic() const { return dynamic; }
            Buffer::Usage getUsage() const { return usage; }
            uint32_t getSize() const { return static_cast<uint32_t>(data.size()); }

        protected:
            BufferResource();

            std::vector<uint8_t> data;
            Buffer::Usage usage;
            bool dynamic = true;

            std::mutex uploadMutex;
        };
    } // namespace graphics
} // namespace ouzel
