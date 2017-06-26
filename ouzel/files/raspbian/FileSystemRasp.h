// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "files/FileSystem.h"

namespace ouzel
{
    class Engine;

    class FileSystemRasp: public FileSystem
    {
        friend Engine;
    public:

    protected:
        FileSystemRasp();
    };
}
