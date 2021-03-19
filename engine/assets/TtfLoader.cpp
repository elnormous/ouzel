// Ouzel by Elviss Strazdins

#include "TtfLoader.hpp"
#include "Bundle.hpp"
#include "../gui/TTFont.hpp"
#include "stb_truetype.h"

namespace ouzel::assets
{
    TtfLoader::TtfLoader(Cache& initCache):
        Loader{initCache, Type::font}
    {
    }

    bool TtfLoader::loadAsset(Bundle& bundle,
                              const std::string& name,
                              const std::vector<std::byte>& data,
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
}
