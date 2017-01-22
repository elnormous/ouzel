// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class IndexBufferResource: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~IndexBufferResource();
            virtual void free() override;

            virtual bool init(bool newDynamic = true);
            virtual bool initFromBuffer(const void* newIndices, uint32_t newSize, bool newDynamic);

            uint32_t getSize() const { return size; }

            virtual bool setData(const void* newIndices, uint32_t newSize);

        protected:
            IndexBufferResource();
            virtual void update() override;
            virtual bool upload() override;

            struct Data
            {
                std::vector<uint8_t> data;
                bool dynamic = true;
            };

            Data uploadData;
            std::mutex uploadMutex;

        private:
            uint32_t size = 0;
            std::vector<uint8_t> data;

            bool dynamic = true;

            Data currentData;
        };
    } // namespace graphics
} // namespace ouzel
