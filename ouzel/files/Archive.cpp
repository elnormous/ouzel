// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Archive.hpp"
#include "FileSystem.hpp"

namespace ouzel
{
    Archive::~Archive()
    {
        if (fileSystem) fileSystem->removeArchive(this);
    }

    bool Archive::readFile(const std::string&, std::vector<uint8_t>&) const
    {
        return false;
    }

    bool Archive::writeFile(const std::string&, const std::vector<uint8_t>&) const
    {
        return false;
    }

    bool Archive::fileExists(const std::string&) const
    {
        return false;
    }
}
