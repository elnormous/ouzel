// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_BMFLOADER_HPP
#define OUZEL_ASSETS_BMFLOADER_HPP

#include "Bundle.hpp"
#include "../gui/BMFont.hpp"

namespace ouzel::assets
{
    inline bool loadBmf(Cache&,
                        Bundle& bundle,
                        const std::string& name,
                        const std::vector<std::byte>& data,
                        const Asset::Options&)
    {
        try
        {
            // TODO: move the loader here
            auto font = std::make_unique<gui::BMFont>(data);
            bundle.setFont(name, std::move(font));
        }
        catch (const std::exception&)
        {
            return false;
        }

        return true;
    }
}

#endif // OUZEL_ASSETS_BMFLOADER_HPP
