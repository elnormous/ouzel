// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "TtfLoader.hpp"
#include "Bundle.hpp"
#include "gui/TTFont.hpp"
#include "stb_truetype.h"

namespace ouzel
{
    namespace assets
    {
        TtfLoader::TtfLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool TtfLoader::loadAsset(Bundle& bundle,
                                  const std::string& name,
                                  const std::vector<uint8_t>& data,
                                  bool mipmaps)
        {
            try
            {
                // TODO: move the loader here
                auto font = std::make_shared<gui::TTFont>(data, mipmaps);
                bundle.setFont(name, font);
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
