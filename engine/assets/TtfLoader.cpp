// Ouzel by Elviss Strazdins

#include "TtfLoader.hpp"
#include "Bundle.hpp"
#include "../gui/TTFont.hpp"
#include "stb_truetype.h"

namespace ouzel::assets
{
    TtfLoader::TtfLoader():
        Loader{Asset::Type::font}
    {
    }

    bool TtfLoader::loadAsset(Cache&,
                              Bundle& bundle,
                              const std::string& name,
                              const std::vector<std::byte>& data,
                              const Asset::Options& options)
    {
        try
        {
            // TODO: move the loader here
            auto font = std::make_unique<gui::TTFont>(data, options.mipmaps);
            bundle.setFont(name, std::move(font));
        }
        catch (const std::exception&)
        {
            return false;
        }

        return true;
    }
}
