// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>

namespace ouzel
{
    namespace assets
    {
        class Cache;

        class Loader
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
