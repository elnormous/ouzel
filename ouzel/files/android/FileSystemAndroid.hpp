// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "files/FileSystem.hpp"

namespace ouzel
{
    class Engine;

    class FileSystemAndroid: public FileSystem
    {
        friend Engine;
    public:
        virtual std::string getStorageDirectory(bool user = true) const override;
        virtual std::string getTempDirectory() const override;

        virtual bool readFile(const std::string& filename, std::vector<uint8_t>& data, bool searchResources = true) const override;

        virtual bool directoryExists(const std::string& dirname) const override;
        virtual bool fileExists(const std::string& filename) const override;

    protected:
        FileSystemAndroid();
    };
}
