#ifndef FLUXION_ASSETS_BMFLOADER_HPP
#define FLUXION_ASSETS_BMFLOADER_HPP

#include "Bundle.hpp"
#include "../gui/BMFont.hpp"

namespace fluxion::assets
{
    inline bool loadBmf(Cache&,
                        Bundle& bundle,
                        const Uuid& uuid,
                        const std::string& name,
                        const std::vector<std::byte>& data,
                        const Asset::Options&)
    {
        try
        {
            // TODO: move the loader here
            auto font = std::make_unique<gui::BMFont>(data);
            bundle.setFont(name, std::move(font), uuid);
        }
        catch (const std::exception&)
        {
            return false;
        }

        return true;
    }
}

#endif // FLUXION_ASSETS_BMFLOADER_HPP
