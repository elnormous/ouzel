#ifndef FLUXION_ASSETS_TTFLOADER_HPP
#define FLUXION_ASSETS_TTFLOADER_HPP

#include "Bundle.hpp"
#include "../gui/TTFont.hpp"
#include "stb_truetype.h"

namespace fluxion::assets
{
    inline bool loadTtf(Cache&,
                        Bundle& bundle,
                        const Uuid& uuid,
                        const std::string& name,
                        const std::vector<std::byte>& data,
                        const Asset::Options& options)
    {
        try
        {
            // TODO: move the loader here
            auto font = std::make_unique<gui::TTFont>(data, options.mipmaps);
            bundle.setFont(name, std::move(font), uuid);
        }
        catch (const std::exception&)
        {
            return false;
        }

        return true;
    }
}

#endif // FLUXION_ASSETS_TTFLOADER_HPP
