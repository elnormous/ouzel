// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderVorbis: public Loader
        {
        public:
            LoaderVorbis();
            virtual bool loadAsset(const std::string& filename) override;
        };
    } // namespace assets
} // namespace ouzel
