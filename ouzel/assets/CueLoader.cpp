// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "CueLoader.hpp"
#include "Bundle.hpp"
#include "utils/Json.hpp"

namespace ouzel
{
    namespace assets
    {
        CueLoader::CueLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool CueLoader::loadAsset(Bundle& bundle,
                                  const std::string& name,
                                  const std::vector<uint8_t>& data,
                                  bool)
        {
            const json::Data d(data);

            return true;
        }
    } // namespace assets
} // namespace ouzel
