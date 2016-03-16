// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Noncopyable.h"
#include "Size2.h"

namespace ouzel
{
    namespace video
    {
        class Image: public Noncopyable
        {
        public:
            Image();
            virtual ~Image();
            
            const Size2& getSize() const { return _size; }
            const void* getData() const { return _data; }
            
            virtual bool initFromFile(const std::string& filename);
            
        protected:
            std::string _filename;
            Size2 _size;
            
            void* _data = nullptr;
        };
    } // namespace video
} // namespace ouzel
