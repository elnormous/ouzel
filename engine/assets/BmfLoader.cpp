// Ouzel by Elviss Strazdins

#include "BmfLoader.hpp"
#include "Bundle.hpp"
#include "../gui/BMFont.hpp"

namespace ouzel::assets
{
    BmfLoader::BmfLoader():
        Loader{Asset::Type::font}
    {
    }

    bool BmfLoader::loadAsset(Cache& cache,
                              Bundle& bundle,
                              const std::string& name,
                              const std::vector<std::byte>& data,
                              const Asset::Options&)
    {
        (void)cache;

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
