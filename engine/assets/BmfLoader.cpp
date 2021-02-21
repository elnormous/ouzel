// Ouzel by Elviss Strazdins

#include "BmfLoader.hpp"
#include "Bundle.hpp"
#include "../gui/BMFont.hpp"

namespace ouzel::assets
{
    BmfLoader::BmfLoader(Cache& initCache):
        Loader(initCache, Type::font)
    {
    }

    bool BmfLoader::loadAsset(Bundle& bundle,
                              const std::string& name,
                              const std::vector<std::byte>& data,
                              bool)
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
