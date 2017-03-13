// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Archive.h"

namespace ouzel
{
    bool Archive::readFile(const std::string& filename, std::vector<uint8_t>& data) const
    {
        return false;
    }

    bool Archive::writeFile(const std::string& filename, const std::vector<uint8_t>& data) const
    {
        return false;
    }

    bool Archive::fileExists(const std::string& filename) const
    {
        return false;
    }
}
