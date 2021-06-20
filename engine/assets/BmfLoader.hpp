// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_BMFLOADER_HPP
#define OUZEL_ASSETS_BMFLOADER_HPP

#include "Loader.hpp"
#include "Bundle.hpp"
#include "../gui/BMFont.hpp"

namespace ouzel::assets
{
    class BmfLoader final: public Loader
    {
    public:
        explicit BmfLoader(): Loader{Asset::Type::font} {}

        bool loadAsset(Cache&,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       const Asset::Options&) override
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
    };
}

#endif // OUZEL_ASSETS_BMFLOADER_HPP
