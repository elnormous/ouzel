// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace assets
    {
        class Cache;

        class Loader: public Noncopyable
        {
            friend Cache;
        public:
            virtual ~Loader();
            virtual bool loadAsset(const std::string& path) = 0;

        protected:
            Cache* cache = nullptr;
        };
    } // namespace assets
} // namespace ouzel
