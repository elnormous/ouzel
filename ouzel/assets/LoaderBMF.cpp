// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderBMF.hpp"
#include "Bundle.hpp"
#include "gui/BMFont.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderBMF::LoaderBMF(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool LoaderBMF::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            try
            {
                std::shared_ptr<BMFont> font = std::make_shared<BMFont>(data);
                bundle.setFont(filename, font);
            }
            catch (...)
            {
                return false;
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
