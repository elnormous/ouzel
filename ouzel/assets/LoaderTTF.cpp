// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderTTF.hpp"
#include "Cache.hpp"
#include "gui/TTFont.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderTTF::LoaderTTF():
            Loader({"ttf"})
        {
        }

        bool LoaderTTF::loadAsset(const std::string& filename)
        {
            std::shared_ptr<TTFont> font = std::make_shared<TTFont>();
            if (!font->init(filename))
            {
                return false;
            }

            cache->setFont(filename, font);

            return true;
        }
    } // namespace assets
} // namespace ouzel
