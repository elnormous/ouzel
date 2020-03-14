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
            Loader(initCache, Loader::Font)
        {
        }

        bool TtfLoader::loadAsset(Bundle& bundle,
                                  const std::string& name,
                                  const std::vector<std::uint8_t>& data,
                                  bool mipmaps)
        {
            try
            {
                // TODO: move the loader here
                auto font = std::make_unique<gui::TTFont>(data, mipmaps);
                bundle.setFont(name, std::move(font));
            }
            catch (const std::exception&)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
