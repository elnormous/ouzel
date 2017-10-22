// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderOBJ: public Loader
        {
        public:
            LoaderOBJ();
            virtual bool loadAsset(const std::string& filename) override;
        };
    } // namespace assets
} // namespace ouzel
