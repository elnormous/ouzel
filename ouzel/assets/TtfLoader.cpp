// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "TtfLoader.hpp"
#include "Bundle.hpp"
#include "gui/TTFont.hpp"

namespace ouzel
{
    namespace assets
    {
        TtfLoader::TtfLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool TtfLoader::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            try
            {
                // TODO: move the loader here
                std::unique_ptr<TTFont> font(new TTFont(data, mipmaps));
                bundle.setFont(filename, std::move(font));
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
