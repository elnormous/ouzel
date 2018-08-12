// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderTTF.hpp"
#include "Bundle.hpp"
#include "gui/TTFont.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderTTF::LoaderTTF(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool LoaderTTF::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            try
            {
                std::shared_ptr<TTFont> font = std::make_shared<TTFont>(data, mipmaps);
                bundle.setFont(filename, font);
            }
            catch (...)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
