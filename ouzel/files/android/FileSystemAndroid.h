// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "files/FileSystem.h"

namespace ouzel
{
    class Engine;

    class FileSystemAndroid: public FileSystem
    {
        friend Engine;
    public:
        virtual std::string getStorageDirectory(bool user = true) const override;

        virtual bool readFile(const std::string& filename, std::vector<uint8_t>& data, bool searchResources = true) const;

    protected:
        FileSystemAndroid();
    };
}
