// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderImage: public Loader
        {
        public:
            virtual bool loadAsset(const std::string& path) override;
        };
    } // namespace assets
} // namespace ouzel
