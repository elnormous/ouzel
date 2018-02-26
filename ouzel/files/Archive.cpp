// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Archive.hpp"
#include "FileSystem.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    Archive::Archive(const std::string& filename)
    {
        open(filename);
    }

    Archive::~Archive()
    {
        if (fileSystem) fileSystem->removeArchive(this);
    }

    bool Archive::open(const std::string& filename)
    {
        if (!file.open(filename, File::READ))
        {
            Log(Log::Level::ERR) << "Failed to open file " << filename;
            return false;
        }

        // TODO: parse

        return true;
    }

    bool Archive::readFile(const std::string& filename, std::vector<uint8_t>&) const
    {
        auto i = entries.find(filename);

        if (i == entries.end()) return false;

        return false;
    }

    bool Archive::fileExists(const std::string& filename) const
    {
        return entries.find(filename) != entries.end();
    }
}
