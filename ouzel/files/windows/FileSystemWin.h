// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include "files/FileSystem.h"

namespace ouzel
{
    class Engine;

    class FileSystemWin: public FileSystem
    {
        friend Engine;
    public:
        virtual std::string getStorageDirectory(bool user = true) const override;
        virtual std::string getTempDirectory() const override;

        virtual bool isAbsolutePath(const std::string& path) const override;

    protected:
        FileSystemWin();
    };
}
