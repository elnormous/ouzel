// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
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
            Loader(const std::vector<std::string>& aExtensions);
            virtual ~Loader();
            virtual bool loadAsset(const std::string& filename) = 0;

        protected:
            Cache* cache = nullptr;
            std::vector<std::string> extensions;
        };
    } // namespace assets
} // namespace ouzel
